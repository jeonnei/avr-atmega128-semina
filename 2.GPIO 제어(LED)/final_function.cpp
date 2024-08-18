#define __DELAY_BACKWARD_COMPATIBLE__										//������ �Լ��ȿ� ������ �ֱ� ���� ����� �����Ѵ�.
#include<avr/io.h>
#include<util/delay.h>
void sw1();																	//����ġ 1���� �����ϱ����� �Լ�
void sw2();																	//����ġ 2���� �����ϱ����� �Լ�
void sw3();																	//����ġ 3���� �����ϱ����� �Լ�
void sw4();																	//����ġ 4���� �����ϱ����� �Լ�
unsigned char led0 = 0x80, led1 = 0x80;
unsigned char led21 = 0xaa, led22 = 0xcc, led23 = 0xee, led24 = 0xee;		//����ġ 3���� �̿��ϴ� ����
int f = 0;																	//�÷��� �̿��ϱ����� ����
int count = 0, t = 200;														//����ġ 4�� ����
int main() {
	DDRA = 0xff;
	DDRD = 0x00;
	PORTA = 0xff;
	while (1) {
		if ((~PIND & 0x0f) == 0x01)f = 100;										//1�� ����ġ�� ��������
		else if ((~PIND & 0x0f) == 0x02)f = 200;								//2�� ����ġ�� ��������
		else if ((~PIND & 0x0f) == 0x04)f = 1;									//3�� ����ġ�� ��������
		if (f == 100)sw1();
		else if (f == 200)sw2();
		else if (f == 1) {														//3�� ����ġ�� 1�� ��������
			while (~PIND & 0x04);												//3�� ����ġ�� �� �� ���� ��ٸ�
			_delay_ms(20);													//ä�͸� ������ ������
			sw3();
			if (~PIND & 0x04) {
				while (~PIND & 0x04);											//����ġ�� �������ִٸ� �� �� ���� ��ٸ�
				_delay_ms(20);												//ä�͸� ������ ������
				f = 2;
			}
		}
		else if (f == 2) {														//3�� ����ġ�� 2�� ��������
			sw3();
			if (~PIND & 0x04) {
				while (~PIND & 0x04);											//����ġ�� �������ִٸ� �� �� ���� ��ٸ�
				_delay_ms(20);												//ä�͸� ������ ������
				f = 3;
			}
		}
		else if (f == 3) {														//3�� ����ġ�� 3�� ��������
			sw3();
			if (~PIND & 0x04) {
				while (~PIND & 0x04);											//����ġ�� �������ִٸ� �� �� ���� ��ٸ�
				_delay_ms(20);												//ä�͸� ������ ������
				f = 4;
			}
		}
		else if (f == 4) {														//3�� ����ġ�� 4�� ��������
			sw3();
			if (~PIND & 0x04) {
				while (~PIND & 0x04);											//����ġ�� �������ִٸ� �� �� ���� ��ٸ�
				_delay_ms(20);												//ä�͸� ������ ������
				f = 1;
			}
		}
		if ((~PIND & 0x0f) == 0x08)sw4();										//4�� ����ġ�� ��������

	}
	return 0;
}
void sw1() {																	//����ġ 1���Լ�
	if ((~PIND & 0x0f) == 0x01)
	{
		_delay_ms(20);
		while (~PIND & 0x0f);													//Ű�� ������ �ִٸ� �� ������ ��ٸ�
		_delay_ms(20);														//ä�͸� ������ ������
		PORTA = ~led0;														//7�� ����ġ ����
		led0 = led0 >> 1;														//�������� ��ĭ ����Ʈ
		if (led0 == 0x00)led0 = 0x80;										//led ���� ��� 0�� �ɶ�, �ʱⰪ���� �ʱ�ȭ
	}
}
void sw2() {																	//����ġ 2���Լ�
	if ((~PIND & 0x0f) == 0x02)
	{
		if (led1 == 0xff) {													//led ���� ff �� �Ǹ�
			PORTA = ~led1;													//ff ������ ������ְ�
			_delay_ms(t);													//������
			led1 = 0x80;													//�ʱⰪ���� �ʱ�ȭ
		}
		else {
			_delay_ms(20);													//ä�͸� ������ ������
			PORTA = ~led1;													//����� led �� ����
			led1 = (led1 >> 1) | 0x80;											//���������� 1ĭ ����Ʈ�ϰ� 0x80�� OR ����
			_delay_ms(t);													//������
		}
	}

}
void sw3() {																	//����ġ 3�� �Լ�
	if (f == 1)																//1�� ��������
	{
		PORTA = ~led21;
		led21 = ~led21;
		_delay_ms(t);
	}
	else if (f == 2)															//2�� ��������
	{
		PORTA = ~led22;
		led22 = ~led22;
		_delay_ms(t);
	}
	else if (f == 3)															//3�� ��������
	{
		PORTA = ~led23;
		led23 = led23 ^ 0x99;
		_delay_ms(t);
	}
	else if (f == 4)															//4�� ��������
	{
		if (led24 == 0x3b)led24 = led24 | 0x80;
		if (led24 == 0x5d)led24 = led24 | 0x80;
		if (led24 == 0x6e)led24 = led24 | 0x80;
		PORTA = ~led24;
		led24 = led24 >> 1;
		_delay_ms(t);
	}
}
void sw4() {
	_delay_ms(20);															//ä�͸� ������ ������
	while (~PIND & 0x08);														//4�� ����ġ�� �������� ��ٸ�
	_delay_ms(20);															//ä�͸� ������ ������
	if (count % 2 == 1) {														//¦���� ��������
		t = 200;															//������ 200���� ����
		count++;
	}
	else {																	//Ȧ���� ��������
		t = 500;															//������ 500���� ����
		count++;
	}

}