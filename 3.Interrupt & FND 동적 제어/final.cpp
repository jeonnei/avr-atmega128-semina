#define F_CPU 7372800
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char FND[10] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f };
volatile int f = 0;								//플래그 변수
volatile int value = 0;							//총 숫자 변수
int ten = 0, one = 0;								//10의자리,1의자리 변수
void fnd_light();								//fnd 동적구동 함수선언

ISR(INT0_vect)									//sw0 인터럽트
{
	f = 1;
	EIFR = 0x01;
}
ISR(INT1_vect)									//sw1 인터럽트
{
	if (value <= -1)value = 99;
	fnd_light();
	value--;
	EIFR = 0x02;
}
ISR(INT2_vect)									//sw2 인터럽트
{
	f = 3;
	value += 5;
	fnd_light();
	EIFR = 0x04;
}
ISR(INT3_vect)									//sw3 인터럽트
{
	f = 0;
	EIFR = 0x08;
}
int main()
{
	DDRC = 0xff;
	DDRD = 0x30;
	EICRA = 0xf2;								//int2~3 상승엣지, int1 low 레벨,int0 하앙엣지 설정
	EIMSK = 0x0f;								//int0~3사용
	sei();
	while (1)
	{
		if (value >= 100)value = 0;
		fnd_light();
		if (f == 0)
		{
			value = 0;
		}
		else if (f == 1)
		{
			value++;
		}
	}
}
void fnd_light()
{
	ten = (value % 100) / 10;
	one = value % 10;
	for (int s = 0;s < 200;s++)						//200ms 딜레이 s값의 설정에 따라 딜레이 지정가능
	{
		PORTD = 0x20;
		PORTC = ~FND[ten];
		_delay_us(500);
		PORTD = 0x10;
		PORTC = ~FND[one];
		_delay_us(500);
	}
}