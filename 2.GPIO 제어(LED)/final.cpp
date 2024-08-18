#define __DELAY_BACKWARD_COMPATIBLE__
#include<avr/io.h>
#include<util/delay.h>


int main()
{
	int led1 = 0x80, led2 = 0x80, count = 0;					// 초기 led 값 설정, 횟수세는 변수선언
	int delay = 200;									//초기 딜레이 200설정
	DDRA = 0xff;										//출력포트 설정
	PORTA = 0xff;										//꺼짐
	DDRD = 0x00;										//입력포트 설정
	while (1)
	{

		if ((~PIND & 0x0f) == 0x01)						//0번 스위치 누를떄
		{
			_delay_ms(20);							//채터링 방지용 딜레이
			while (~PIND & 0x01);
			_delay_ms(20);							//채터링 방지용 딜레이
			PORTA = ~led1;							//지정된 led 값 점등
			led1 = led1 >> 1;							// 오른쪽으로 쉬프트
			if (led1 == 0x00)led1 = 0x80;				// 끝까지가면 초기값으로 초기화

		}
		else if (~PIND & 0x02)							// 1번 스위치를 누를때
		{
			PORTA = ~led2;							//저장된 led 값 점등
			if ((~PIND & 0x0f) == 0x02)				// 스위치1번을 눌렀을때 실행
			{
				if (led2 == 0xff) {					//led 값이 ff 가 되면
					PORTA = ~led2;					//ff 까지는 출력해주고
					_delay_ms(delay);				//딜레이
					led2 = 0x80;					//초기값으로 초기화
				}
				else {
					_delay_ms(20);					//채터링 방지용 딜레이
					led2 = (led2 >> 1) | 0x80;			//오른쪽으로 1칸 쉬프트하고 0x80을 OR 해줌
					_delay_ms(delay);				//딜레이
				}
			}
		}
		else if (~PIND & 0x08) {						// 3번 스위치를 누를떄
			_delay_ms(20);								//채터링 방지용 딜레이
			while (~PIND & 0x08);							//3번 스위치를 땔때까지 기다림
			_delay_ms(20);								//채터링 방지용 딜레이
			if (count % 2 == 1) {							// 짝수번 눌렀을떄
				delay = 200;						// 딜레이 200
			}
			else if (count % 2 == 0) {					// 홀수번 눌렀을떄
				delay = 500;						// 딜레이 500
			}
			count++;
		}
	}
}