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
	//������ ����
	TCCR3B = 0x0a;  //0b00001010 CTC���, ���ֺ� 8
	ETIMSK = 0x10;  //0b00010000 ����ġ ���ͷ�Ʈ ���
	EICRB = 0x03;  //0b00000011 ��� �𼭸����� ���ͷ�Ʈ �䱸
	EIMSK = 0x10;  //0b00010000 INT4 ���ͷ�Ʈ ���
	OCR3A = 0;     //����ġ �������� �ʱ�ȭ
}

//Serial ����
void serial_init(unsigned long baud)				//serial ����
{
	unsigned short ubrr;
	ubrr = (unsigned)(F_CPU / (16 * baud) - 1);
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr & 0xff);
	//UCSR0C = 0x06;									//�񵿱�,stop bit:1bit, size:8bit,�и�Ƽx
	//UCSR0B = 0x18;									//RX TX ���
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);			//RX���ź� ����,TX �۽ź� ����
	UCSR0C = (3 << UCSZ00);
}

void tx_data(unsigned char data)					//�۽�
{
	while ((UCSR0A & 0x20) == 0x00);
	UDR0 = data;
}
void tx_string(char* str)//���ڿ��۽�
{

	while (*str != '\0')    //���ڿ��� ��(NULL����)�������� �����͸� ����
	{
		tx_data(*str++);
	}
	tx_data(0x0d);
	tx_data('\n');
}
unsigned char rx_data(void) { // �����Ͱ� �� ��� �Ë� ���� ��� ������
	return UDR0;				// �ø��� ��Ʈ�� ���� ������ ����
}

ISR(TIMER3_COMPA_vect)
{
	//Ÿ�̸�ī����3 ����ġ���
	if (count_pulse < 10) {  //10uS�� �Ǳ� ��
		PORTE = 0x08;  //E3(Trigger)
		OCR3A = 1;  //1us

	}
	else {  // 10uS�� �Ǿ��� ��
		PORTE = 0x00;
		OCR3A = 55296;  //(7372800/8)*0.06=55296 (���� �ֱ�: 60ms �̻����� �� ��-�����ͽ�Ʈ)
		count_pulse = 0;
	}
	count_pulse++;
}

ISR(INT4_vect) {  //E4(Echo)
	if (flag == 0) {
		TCNT3 = 0;  // TCNT�� ����
		EICRB = 0x02;  // �ϰ��𼭸� ���ͷ�Ʈ
		flag = 1;
	}
	else {
		distance = TCNT3 / 58;	//���� ��ȣ ���� ����:uS/58=cm
		sprintf(dat, "%d", distance);	//���ڿ� ���������� ����
		EICRB = 0x03;  //��¸𼭸� ���ͷ�Ʈ
		flag = 0;
	}
}



int main(void)
{
	serial_init(9600);						//������Ʈ 9600
	Register_seting();

	DDRB = 0b01100000;
	DDRE = 0x08;  //E3 Ʈ���� ���, E4 ���� �Է�
	DDRD = 0x01;
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011010;

	ICR1 = 18432;

	OCR1A = 0;
	OCR1B = 1500;								//�ʱⰪ 90��

	sei(); //�������ͷ�Ʈ ���

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


		}	//���� �����ͺ�Ʈ�� ���� two �� ����


		tx_string(dat);
		_delay_ms(100);


	}
}