#define F_CPU 7372800

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<string.h>
#include<stdio.h>

//Serial 설정
void serial_init(unsigned long baud)																//serial 설정
{
	unsigned short ubrr;
	ubrr = (unsigned)(F_CPU / (16 * baud) - 1);
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr & 0xff);
	UCSR0C = 0x06;																					//비동기,stop bit:1bit, size:8bit,패리티x
	UCSR0B = 0x18;																					//RX TX 허용
}
void tx_data(unsigned char data)																	//flag 검사:데이터 레지스터 준비 완료
{
	while ((UCSR0A & 0x20) == 0x00);
	UDR0 = data;
}
unsigned char rx_data(void)																			//flag 검사:수신완료
{
	while ((UCSR0A & 0x80) == 0x00);
	return UDR0;
}
void rx_string(char* str)
{
	unsigned char dat;
	while (1)
	{
		dat = rx_data();
		if (dat == '\r') break;																		//엔터 입력시 탈출

		*str++ = dat;
	}
	*str++ = '\n';
	*str = '\0';
}

//변수,배열 선언
static unsigned char fnd10[10] = { 0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef };				//fnd 소숫점 배열
static unsigned char fnd1[10] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f };				//fnd 배열
static int putty;																					//푸티값 변수
unsigned int adc_value = 0, mode = 0;																	//adc 변환 값,flag 변수 설정
int fnd_value = 0;																					//fnd 변수
unsigned int temp;
char dat[10] = "";																					//배열선언
//함수 원형
void fnd_light1(int i);
void fnd_light34(int k);
unsigned int get_adc();

int main()
{

	serial_init(9600);
	DDRC = 0xff;																						//PC0~7 출력
	DDRD = 0xf0;																					//PD4~5출력,PD0~3 입력
	DDRF = 0x00;																					//PF1 가변저항기 입력
	//ADC
	ADMUX = 0x41;																					//내부전압사용,우측정렬,CH1번사용
	ADCSRA = 0xe7;																					//ADC가능,ADC변환시작,ADC프리러닝모드,프리스케일러128

	while (1)
	{
		adc_value = get_adc();																		//ADC변환값 변수에 저장
		if ((~PIND & 0x0f) == 0x01)mode = 1;
		else if ((~PIND & 0x0f) == 0x02)mode = 2;
		else if ((~PIND & 0x0f) == 0x04)mode = 3;
		else if ((~PIND & 0x0f) == 0x08)mode = 4;

		if (mode == 1)fnd_light1(adc_value);															//FND에 센서의 전압값 출력
		else if (mode == 2)
		{
			temp = adc_value / 10;
			tx_data(temp + '0');																		//송신하기위해 아스키코드로 문자화
			tx_data('.');
			temp = adc_value % 10;
			tx_data(temp + '0');
			tx_data(0x0d);																			//carriage return
			tx_data('\n');																			//줄바꿈
			_delay_ms(100);																			//출력 딜레이
		}
		else if (mode == 3)
		{
			if ((UCSR0A & 0x80) == 0x80)
			{
				rx_string(dat);
				sscanf(dat, "%d", &putty);															//문자열을 정수형으로 변환후 정수형 변수putty에 저장하는 함수
			}
			fnd_light34(putty);

		}
		else if (mode == 4)
		{
			fnd_light34(fnd_value);
			if ((UCSR0A & 0x80) == 0x80)putty = UDR0;													//데이터 수신이되면 putty 변수에 수신된 데이터값 저장										
			if (putty == '.')																		// >누를시 증가
			{
				fnd_value += 1;
				if (fnd_value == 100)fnd_value = 0;
				putty = 0;																			//putty 값을 초기화 해주지않으면 반복문때문에 자동으로 증가하게됨
			}
			else if (putty == ',')																	// <누를시 감소
			{
				fnd_value -= 1;
				if (fnd_value < 0)fnd_value = 99;														//putty 값을 초기화 해주지않으면 반복문때문에 자동으로 감소하게됨
				putty = 0;
			}
		}
	}

}

//함수
unsigned int get_adc()																				//ADC변환 함수 반환값은 정수형으로 설정함
{
	float fval;
	int ival;
	while ((ADCSRA & 0x10) == 0x10);																		//변환종료를 기다림
	adc_value = ADC;																				//변환값 저장
	ADCSRA = ADCSRA | 0x10;																			//변환완료 플래그 리셋
	fval = (float)adc_value * 5.0 / 1024.0;																//전압값으로 변환
	ival = (int)(fval * 10.0 + 0.5);																	//반올림 후 정수화
	return(ival);
}
void fnd_light1(int i)																				//1번문제 FND동적 구동
{
	PORTD = 0x20;																					//정수부분
	PORTC = ~fnd10[i / 10];
	_delay_ms(1);
	PORTD = 0x10;																					//소수 첫째자리 표시
	PORTC = ~fnd1[i % 10];
	_delay_ms(1);
}
void fnd_light34(int k)																				//3번,4번문제 FND동적 구동
{
	PORTD = 0x20;
	PORTC = ~fnd1[k / 10];
	_delay_ms(1);
	PORTD = 0x10;
	PORTC = ~fnd1[k % 10];
	_delay_ms(1);
}
