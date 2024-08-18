#define F_CPU 7372800

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<string.h>
#include<stdio.h>

//Serial ����
void serial_init(unsigned long baud)				//serial ����
{
	unsigned short ubrr;
	ubrr = (unsigned)(F_CPU / (16 * baud) - 1);
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr & 0xff);
	UCSR0C = 0x06;									//�񵿱�,stop bit:1bit, size:8bit,�и�Ƽx
	UCSR0B = 0x18;									//RX TX ���
}
void tx_data(unsigned char data)					//�۽�
{
	while ((UCSR0A & 0x20) == 0x00);
	UDR0 = data;
}
unsigned char rx_data(void)
{
	while ((UCSR0A & 0x80) == 0x00);						//flag �˻�:���ſϷ�
	return UDR0;
}
void rx_string(char* str)
{
	tx_data('\n');									//�ٹٲ�
	tx_data(0x0d);									//��������
	unsigned char dat;
	while (1)
	{
		dat = rx_data();
		if (dat == '\r') break;						//���� �Է½� Ż��
		*str++ = dat;
		tx_data(dat);								//ȭ�鿡 �Է��� ���� ��������
	}
	*str = '\0';										//NULL
}
char dat[10] = "";
unsigned int putty = 0;								//putty ����
unsigned int gakdo;									//���� ����


unsigned char fnd[10] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f };	//fnd �迭
volatile unsigned int adc_value = 0;
unsigned int fnd_value = 0;
unsigned int mode = 0;
unsigned int get_adc();
void FND_light(unsigned int i);

int main(void)
{
	serial_init(9600);						//������Ʈ 9600
	DDRC = 0xff;
	DDRD = 0x30;
	DDRF = 0x00;
	DDRB = 0xff;
	//ADC ����
	ADMUX = 0x41;							//�������� AVCC,ADC1ä��
	ADCSRA = 0xe7;							//��ȯ����,��������,���ֺ�128
	//TIMER ����
	TCCR1A = 0x82;							//OC1A�� ����ġ�� 0�����ϰ� TOP���� 1���/WGMn1~0 2����
	TCCR1B = 0x1c;							//fast PWM���,���ֺ�256
	ICR1 = 576;								//(1/(7372800/256)*576 = 20ms,�ִ��ֱ�
	OCR1A = 18;								//�ʱⰪ 0��

	while (1)
	{
		adc_value = get_adc();				//��ȯ�� ���� : 0~18
		if ((~PIND & 0x01) == 0x01)mode = 1;
		else if ((~PIND & 0x02) == 0x02)mode = 2;
		else if ((~PIND & 0x04) == 0x04)mode = 3;
		else if ((~PIND & 0x08) == 0x08)mode = 0;
		if (mode == 0)							//�ʱ�ȭ
		{
			FND_light(0);
			OCR1A = 18;
			putty = 0;
			adc_value = 0;
		}
		else if (mode == 1)
		{
			FND_light(adc_value);			//FND�� ��ȯ�� ���
		}
		else if (mode == 2)
		{
			FND_light(adc_value);			//FND�� ��ȯ�� ���
			OCR1A = (adc_value * 3) + 18;
			_delay_ms(500);
			OCR1A = 18;
			_delay_ms(500);
		}
		else if (mode == 3)					//3������ġ 
		{
			if ((UCSR0A & 0x80) == 0x80)
			{
				rx_string(dat);				//���ڿ� ����
				sscanf(dat, "%d", &putty);	//���ڿ� ���������� ����
			}
			if (putty > 180)
			{
				putty = putty % 180;			//180���� ���� ������ ex)200�Է½� 180���� ���� ������ 20�� ����
			}
			gakdo = putty * 3.0 / 10.0 + 18;		//�Է°� ������ �°� ��ȯ
			OCR1A = gakdo;
		}
	}
}

//�Լ�
unsigned int get_adc()						//ADC ��ȯ���� 0~18������ ��ȯ�ϴ� �Լ�
{
	float fval = 0;
	int ival = 0;
	int adc18 = 0;
	if ((ADCSRA & 0x10) == 0x00)
	{
		adc_value = ADC;
		ADCSRA = ADCSRA | 0x10;
		fval = (float)adc_value * 18.0 / 1024.0;	//�Ǽ��� 0~18���� �ٲ�
		ival = (int)(fval * 10.0 + 0.5);		//�ݿø��� ����ȭ
		adc18 = ival / 10;					//�ݿø��� ������ ���� ����
	}
	return(adc18);							//ADC��ȯ�� 0~18 ��ȯ
}
void FND_light(unsigned int i)				//FND ��������
{
	PORTD = 0x20;
	PORTC = ~fnd[i / 10];
	_delay_us(500);
	PORTD = 0x10;
	PORTC = ~fnd[i % 10];
	_delay_us(500);
}