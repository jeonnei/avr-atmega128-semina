#define __DELAY_BACKWARD_COMPATIBLE__
#include<avr/io.h>
#include<util/delay.h>


int main()
{
	int led1 = 0x80, led2 = 0x80, count = 0;					// �ʱ� led �� ����, Ƚ������ ��������
	int delay = 200;									//�ʱ� ������ 200����
	DDRA = 0xff;										//�����Ʈ ����
	PORTA = 0xff;										//����
	DDRD = 0x00;										//�Է���Ʈ ����
	while (1)
	{

		if ((~PIND & 0x0f) == 0x01)						//0�� ����ġ ������
		{
			_delay_ms(20);							//ä�͸� ������ ������
			while (~PIND & 0x01);
			_delay_ms(20);							//ä�͸� ������ ������
			PORTA = ~led1;							//������ led �� ����
			led1 = led1 >> 1;							// ���������� ����Ʈ
			if (led1 == 0x00)led1 = 0x80;				// ���������� �ʱⰪ���� �ʱ�ȭ

		}
		else if (~PIND & 0x02)							// 1�� ����ġ�� ������
		{
			PORTA = ~led2;							//����� led �� ����
			if ((~PIND & 0x0f) == 0x02)				// ����ġ1���� �������� ����
			{
				if (led2 == 0xff) {					//led ���� ff �� �Ǹ�
					PORTA = ~led2;					//ff ������ ������ְ�
					_delay_ms(delay);				//������
					led2 = 0x80;					//�ʱⰪ���� �ʱ�ȭ
				}
				else {
					_delay_ms(20);					//ä�͸� ������ ������
					led2 = (led2 >> 1) | 0x80;			//���������� 1ĭ ����Ʈ�ϰ� 0x80�� OR ����
					_delay_ms(delay);				//������
				}
			}
		}
		else if (~PIND & 0x08) {						// 3�� ����ġ�� ������
			_delay_ms(20);								//ä�͸� ������ ������
			while (~PIND & 0x08);							//3�� ����ġ�� �������� ��ٸ�
			_delay_ms(20);								//ä�͸� ������ ������
			if (count % 2 == 1) {							// ¦���� ��������
				delay = 200;						// ������ 200
			}
			else if (count % 2 == 0) {					// Ȧ���� ��������
				delay = 500;						// ������ 500
			}
			count++;
		}
	}
}