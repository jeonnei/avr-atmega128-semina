#define F_CPU 7372800
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char FND[10] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f };
volatile int f = 0;								//�÷��� ����
volatile int value = 0;							//�� ���� ����
int ten = 0, one = 0;								//10���ڸ�,1���ڸ� ����
void fnd_light();								//fnd �������� �Լ�����

ISR(INT0_vect)									//sw0 ���ͷ�Ʈ
{
	f = 1;
	EIFR = 0x01;
}
ISR(INT1_vect)									//sw1 ���ͷ�Ʈ
{
	if (value <= -1)value = 99;
	fnd_light();
	value--;
	EIFR = 0x02;
}
ISR(INT2_vect)									//sw2 ���ͷ�Ʈ
{
	f = 3;
	value += 5;
	fnd_light();
	EIFR = 0x04;
}
ISR(INT3_vect)									//sw3 ���ͷ�Ʈ
{
	f = 0;
	EIFR = 0x08;
}
int main()
{
	DDRC = 0xff;
	DDRD = 0x30;
	EICRA = 0xf2;								//int2~3 ��¿���, int1 low ����,int0 �Ͼӿ��� ����
	EIMSK = 0x0f;								//int0~3���
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
	for (int s = 0;s < 200;s++)						//200ms ������ s���� ������ ���� ������ ��������
	{
		PORTD = 0x20;
		PORTC = ~FND[ten];
		_delay_us(500);
		PORTD = 0x10;
		PORTC = ~FND[one];
		_delay_us(500);
	}
}