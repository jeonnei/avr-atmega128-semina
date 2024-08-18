#define F_CPU 7372800

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<string.h>
#include<stdio.h>

//Serial ����
void serial_init(unsigned long baud)																//serial ����
{
	unsigned short ubrr;
	ubrr = (unsigned)(F_CPU / (16 * baud) - 1);
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr & 0xff);
	UCSR0C = 0x06;																					//�񵿱�,stop bit:1bit, size:8bit,�и�Ƽx
	UCSR0B = 0x18;																					//RX TX ���
}
void tx_data(unsigned char data)																	//flag �˻�:������ �������� �غ� �Ϸ�
{
	while ((UCSR0A & 0x20) == 0x00);
	UDR0 = data;
}
unsigned char rx_data(void)																			//flag �˻�:���ſϷ�
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
		if (dat == '\r') break;																		//���� �Է½� Ż��

		*str++ = dat;
	}
	*str++ = '\n';
	*str = '\0';
}

//����,�迭 ����
static unsigned char fnd10[10] = { 0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef };				//fnd �Ҽ��� �迭
static unsigned char fnd1[10] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f };				//fnd �迭
static int putty;																					//ǪƼ�� ����
unsigned int adc_value = 0, mode = 0;																	//adc ��ȯ ��,flag ���� ����
int fnd_value = 0;																					//fnd ����
unsigned int temp;
char dat[10] = "";																					//�迭����
//�Լ� ����
void fnd_light1(int i);
void fnd_light34(int k);
unsigned int get_adc();

int main()
{

	serial_init(9600);
	DDRC = 0xff;																						//PC0~7 ���
	DDRD = 0xf0;																					//PD4~5���,PD0~3 �Է�
	DDRF = 0x00;																					//PF1 �������ױ� �Է�
	//ADC
	ADMUX = 0x41;																					//�������л��,��������,CH1�����
	ADCSRA = 0xe7;																					//ADC����,ADC��ȯ����,ADC�������׸��,���������Ϸ�128

	while (1)
	{
		adc_value = get_adc();																		//ADC��ȯ�� ������ ����
		if ((~PIND & 0x0f) == 0x01)mode = 1;
		else if ((~PIND & 0x0f) == 0x02)mode = 2;
		else if ((~PIND & 0x0f) == 0x04)mode = 3;
		else if ((~PIND & 0x0f) == 0x08)mode = 4;

		if (mode == 1)fnd_light1(adc_value);															//FND�� ������ ���а� ���
		else if (mode == 2)
		{
			temp = adc_value / 10;
			tx_data(temp + '0');																		//�۽��ϱ����� �ƽ�Ű�ڵ�� ����ȭ
			tx_data('.');
			temp = adc_value % 10;
			tx_data(temp + '0');
			tx_data(0x0d);																			//carriage return
			tx_data('\n');																			//�ٹٲ�
			_delay_ms(100);																			//��� ������
		}
		else if (mode == 3)
		{
			if ((UCSR0A & 0x80) == 0x80)
			{
				rx_string(dat);
				sscanf(dat, "%d", &putty);															//���ڿ��� ���������� ��ȯ�� ������ ����putty�� �����ϴ� �Լ�
			}
			fnd_light34(putty);

		}
		else if (mode == 4)
		{
			fnd_light34(fnd_value);
			if ((UCSR0A & 0x80) == 0x80)putty = UDR0;													//������ �����̵Ǹ� putty ������ ���ŵ� �����Ͱ� ����										
			if (putty == '.')																		// >������ ����
			{
				fnd_value += 1;
				if (fnd_value == 100)fnd_value = 0;
				putty = 0;																			//putty ���� �ʱ�ȭ ������������ �ݺ��������� �ڵ����� �����ϰԵ�
			}
			else if (putty == ',')																	// <������ ����
			{
				fnd_value -= 1;
				if (fnd_value < 0)fnd_value = 99;														//putty ���� �ʱ�ȭ ������������ �ݺ��������� �ڵ����� �����ϰԵ�
				putty = 0;
			}
		}
	}

}

//�Լ�
unsigned int get_adc()																				//ADC��ȯ �Լ� ��ȯ���� ���������� ������
{
	float fval;
	int ival;
	while ((ADCSRA & 0x10) == 0x10);																		//��ȯ���Ḧ ��ٸ�
	adc_value = ADC;																				//��ȯ�� ����
	ADCSRA = ADCSRA | 0x10;																			//��ȯ�Ϸ� �÷��� ����
	fval = (float)adc_value * 5.0 / 1024.0;																//���а����� ��ȯ
	ival = (int)(fval * 10.0 + 0.5);																	//�ݿø� �� ����ȭ
	return(ival);
}
void fnd_light1(int i)																				//1������ FND���� ����
{
	PORTD = 0x20;																					//�����κ�
	PORTC = ~fnd10[i / 10];
	_delay_ms(1);
	PORTD = 0x10;																					//�Ҽ� ù°�ڸ� ǥ��
	PORTC = ~fnd1[i % 10];
	_delay_ms(1);
}
void fnd_light34(int k)																				//3��,4������ FND���� ����
{
	PORTD = 0x20;
	PORTC = ~fnd1[k / 10];
	_delay_ms(1);
	PORTD = 0x10;
	PORTC = ~fnd1[k % 10];
	_delay_ms(1);
}
