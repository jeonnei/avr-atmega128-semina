#define F_CPU 7372800UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<string.h>
#include<stdio.h>

volatile unsigned int distance = 0, flag = 0, count_pulse = 0;
char dat[10] = "";
unsigned char control_data = 0;

void Register_seting()
{
	//초음파 센서
	TCCR3B = 0x0a;  //0b00001010 CTC모드, 분주비 8
	ETIMSK = 0x10;  //0b00010000 비교일치 인터럽트 허용
	EICRB = 0x03;  //0b00000011 상승 모서리에서 인터럽트 요구
	EIMSK = 0x10;  //0b00010000 INT4 인터럽트 허용
	OCR3A = 0;     //비교일치 레지스터 초기화
}

//Serial 설정
void serial_init(unsigned long baud)				//serial 설정
{
	unsigned short ubrr;
	ubrr = (unsigned)(F_CPU / (16 * baud) - 1);
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr & 0xff);
	//UCSR0C = 0x06;									//비동기,stop bit:1bit, size:8bit,패리티x
	//UCSR0B = 0x18;									//RX TX 허용
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);			//RX수신부 가능,TX 송신부 가능
	UCSR0C = (3 << UCSZ00);
}

void tx_data(unsigned char data)					//송신
{
	while ((UCSR0A & 0x20) == 0x00);
	UDR0 = data;
}
void tx_string(char* str)//문자열송신
{

	while (*str != '\0')    //문자열이 끝(NULL문자)날때까지 데이터를 보냄
	{
		tx_data(*str++);
	}
	tx_data(0x0d);
	tx_data('\n');
}
unsigned char rx_data(void) { // 데이터가 다 들어 올떄 까지 대기 데이터
	return UDR0;				// 시리얼 포트를 통해 데이터 받음
}

ISR(TIMER3_COMPA_vect)
{
	//타이머카운터3 비교일치모드
	if (count_pulse < 10) {  //10uS가 되기 전
		PORTE = 0x08;  //E3(Trigger)
		OCR3A = 1;  //1us

	}
	else {  // 10uS가 되었을 때
		PORTE = 0x00;
		OCR3A = 55296;  //(7372800/8)*0.06=55296 (측정 주기: 60ms 이상으로 할 것-데이터시트)
		count_pulse = 0;
	}
	count_pulse++;
}

ISR(INT4_vect) {  //E4(Echo)
	if (flag == 0) {
		TCNT3 = 0;  // TCNT값 리셋
		EICRB = 0x02;  // 하강모서리 인터럽트
		flag = 1;
	}
	else {
		distance = TCNT3 / 58;	//에코 신호 수신 공식:uS/58=cm
		sprintf(dat, "%d", distance);	//문자열 정수형으로 저장
		EICRB = 0x03;  //상승모서리 인터럽트
		flag = 0;
	}
}



int main(void)
{
	serial_init(9600);						//보레이트 9600
	Register_seting();

	DDRB = 0b01100000;
	DDRE = 0x08;  //E3 트리거 출력, E4 에코 입력
	DDRD = 0x01;
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011010;

	ICR1 = 18432;

	OCR1A = 0;
	OCR1B = 1500;								//초기값 90도

	sei(); //전역인터럽트 허용

	while (1)
	{

		if ((UCSR0A & 0X80) == 0X80)
		{
			control_data = rx_data();
			_delay_ms(50);

			switch (control_data)
			{
			case '2':
				PORTD = 0x01;
				OCR1A = 4000;
				break;

			case '4':
				OCR1B = 1200;
				break;

			case '5':
				OCR1B = 1500;
				break;

			case '6':
				OCR1B = 1900;
				break;

			case '8':
				PORTD = 0x00;
				OCR1A = 4000;
				break;

			case '9':
				OCR1A = 0;
				break;

			default:
				OCR1A = 0;
				OCR1B = 1500;
				break;
			}


		}	//받은 데이터비트를 변수 two 에 저장


		tx_string(dat);
		_delay_ms(100);


	}
}