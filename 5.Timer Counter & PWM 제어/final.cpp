#define F_CPU 7372800

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<string.h>
#include<stdio.h>

//Serial 설정
void serial_init(unsigned long baud)				//serial 설정
{
	unsigned short ubrr;
	ubrr = (unsigned)(F_CPU / (16 * baud) - 1);
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr & 0xff);
	UCSR0C = 0x06;									//비동기,stop bit:1bit, size:8bit,패리티x
	UCSR0B = 0x18;									//RX TX 허용
}
void tx_data(unsigned char data)					//송신
{
	while ((UCSR0A & 0x20) == 0x00);
	UDR0 = data;
}
unsigned char rx_data(void)
{
	while ((UCSR0A & 0x80) == 0x00);						//flag 검사:수신완료
	return UDR0;
}
void rx_string(char* str)
{
	tx_data('\n');									//줄바꿈
	tx_data(0x0d);									//좌측정렬
	unsigned char dat;
	while (1)
	{
		dat = rx_data();
		if (dat == '\r') break;						//엔터 입력시 탈출
		*str++ = dat;
		tx_data(dat);								//화면에 입력한 숫자 나오게함
	}
	*str = '\0';										//NULL
}
char dat[10] = "";
unsigned int putty = 0;								//putty 변수
unsigned int gakdo;									//각도 변수


unsigned char fnd[10] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f };	//fnd 배열
volatile unsigned int adc_value = 0;
unsigned int fnd_value = 0;
unsigned int mode = 0;
unsigned int get_adc();
void FND_light(unsigned int i);

int main(void)
{
	serial_init(9600);						//보레이트 9600
	DDRC = 0xff;
	DDRD = 0x30;
	DDRF = 0x00;
	DDRB = 0xff;
	//ADC 설정
	ADMUX = 0x41;							//기준전압 AVCC,ADC1채널
	ADCSRA = 0xe7;							//변환시작,프리러닝,분주비128
	//TIMER 설정
	TCCR1A = 0x82;							//OC1A핀 비교일치시 0으로하고 TOP에서 1출력/WGMn1~0 2대입
	TCCR1B = 0x1c;							//fast PWM모드,분주비256
	ICR1 = 576;								//(1/(7372800/256)*576 = 20ms,최대주기
	OCR1A = 18;								//초기값 0도

	while (1)
	{
		adc_value = get_adc();				//변환값 범위 : 0~18
		if ((~PIND & 0x01) == 0x01)mode = 1;
		else if ((~PIND & 0x02) == 0x02)mode = 2;
		else if ((~PIND & 0x04) == 0x04)mode = 3;
		else if ((~PIND & 0x08) == 0x08)mode = 0;
		if (mode == 0)							//초기화
		{
			FND_light(0);
			OCR1A = 18;
			putty = 0;
			adc_value = 0;
		}
		else if (mode == 1)
		{
			FND_light(adc_value);			//FND에 변환값 출력
		}
		else if (mode == 2)
		{
			FND_light(adc_value);			//FND에 변환값 출력
			OCR1A = (adc_value * 3) + 18;
			_delay_ms(500);
			OCR1A = 18;
			_delay_ms(500);
		}
		else if (mode == 3)					//3번스위치 
		{
			if ((UCSR0A & 0x80) == 0x80)
			{
				rx_string(dat);				//문자열 수신
				sscanf(dat, "%d", &putty);	//문자열 정수형으로 저장
			}
			if (putty > 180)
			{
				putty = putty % 180;			//180으로 나눈 나머지 ex)200입력시 180으로 나눈 나머지 20을 저장
			}
			gakdo = putty * 3.0 / 10.0 + 18;		//입력값 각도에 맞게 변환
			OCR1A = gakdo;
		}
	}
}

//함수
unsigned int get_adc()						//ADC 변환값을 0~18범위로 변환하는 함수
{
	float fval = 0;
	int ival = 0;
	int adc18 = 0;
	if ((ADCSRA & 0x10) == 0x00)
	{
		adc_value = ADC;
		ADCSRA = ADCSRA | 0x10;
		fval = (float)adc_value * 18.0 / 1024.0;	//실수로 0~18단위 바꿈
		ival = (int)(fval * 10.0 + 0.5);		//반올림후 정수화
		adc18 = ival / 10;					//반올림후 원래의 숫자 추출
	}
	return(adc18);							//ADC변환값 0~18 반환
}
void FND_light(unsigned int i)				//FND 동적구동
{
	PORTD = 0x20;
	PORTC = ~fnd[i / 10];
	_delay_us(500);
	PORTD = 0x10;
	PORTC = ~fnd[i % 10];
	_delay_us(500);
}