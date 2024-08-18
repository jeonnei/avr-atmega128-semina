#define __DELAY_BACKWARD_COMPATIBLE__										//딜레이 함수안에 변수를 넣기 위한 기능을 정의한다.
#include<avr/io.h>
#include<util/delay.h>
void sw1();																	//스위치 1번을 동작하기위한 함수
void sw2();																	//스위치 2번을 동작하기위한 함수
void sw3();																	//스위치 3번을 동작하기위한 함수
void sw4();																	//스위치 4번을 동작하기위한 함수
unsigned char led0 = 0x80, led1 = 0x80;
unsigned char led21 = 0xaa, led22 = 0xcc, led23 = 0xee, led24 = 0xee;		//스위치 3번때 이용하는 변수
int f = 0;																	//플래그 이용하기위한 변수
int count = 0, t = 200;														//스위치 4번 변수
int main() {
	DDRA = 0xff;
	DDRD = 0x00;
	PORTA = 0xff;
	while (1) {
		if ((~PIND & 0x0f) == 0x01)f = 100;										//1번 스위치를 눌렸을때
		else if ((~PIND & 0x0f) == 0x02)f = 200;								//2번 스위치를 눌렀을때
		else if ((~PIND & 0x0f) == 0x04)f = 1;									//3번 스위치를 눌렀을때
		if (f == 100)sw1();
		else if (f == 200)sw2();
		else if (f == 1) {														//3번 스위치를 1번 눌렀을때
			while (~PIND & 0x04);												//3번 스위치를 땔 때 까지 기다림
			_delay_ms(20);													//채터링 방지용 딜레이
			sw3();
			if (~PIND & 0x04) {
				while (~PIND & 0x04);											//스위치를 누르고있다면 뗄 때 까지 기다림
				_delay_ms(20);												//채터링 방지용 딜레이
				f = 2;
			}
		}
		else if (f == 2) {														//3번 스위치를 2번 눌렀을때
			sw3();
			if (~PIND & 0x04) {
				while (~PIND & 0x04);											//스위치를 누르고있다면 뗄 때 까지 기다림
				_delay_ms(20);												//채터링 방지용 딜레이
				f = 3;
			}
		}
		else if (f == 3) {														//3번 스위치를 3번 눌렀을때
			sw3();
			if (~PIND & 0x04) {
				while (~PIND & 0x04);											//스위치를 누르고있다면 뗄 때 까지 기다림
				_delay_ms(20);												//채터링 방지용 딜레이
				f = 4;
			}
		}
		else if (f == 4) {														//3번 스위치를 4번 눌렀을때
			sw3();
			if (~PIND & 0x04) {
				while (~PIND & 0x04);											//스위치를 누르고있다면 뗄 때 까지 기다림
				_delay_ms(20);												//채터링 방지용 딜레이
				f = 1;
			}
		}
		if ((~PIND & 0x0f) == 0x08)sw4();										//4번 스위치를 눌렀을때

	}
	return 0;
}
void sw1() {																	//스위치 1번함수
	if ((~PIND & 0x0f) == 0x01)
	{
		_delay_ms(20);
		while (~PIND & 0x0f);													//키를 누르고 있다면 뗄 때까지 기다림
		_delay_ms(20);														//채터링 방지용 딜레이
		PORTA = ~led0;														//7번 스위치 점등
		led0 = led0 >> 1;														//왼쪽으로 한칸 쉬프트
		if (led0 == 0x00)led0 = 0x80;										//led 값이 모두 0이 될때, 초기값으로 초기화
	}
}
void sw2() {																	//스위치 2번함수
	if ((~PIND & 0x0f) == 0x02)
	{
		if (led1 == 0xff) {													//led 값이 ff 가 되면
			PORTA = ~led1;													//ff 까지는 출력해주고
			_delay_ms(t);													//딜레이
			led1 = 0x80;													//초기값으로 초기화
		}
		else {
			_delay_ms(20);													//채터링 방지용 딜레이
			PORTA = ~led1;													//저장된 led 값 점등
			led1 = (led1 >> 1) | 0x80;											//오른쪽으로 1칸 쉬프트하고 0x80을 OR 해줌
			_delay_ms(t);													//딜레이
		}
	}

}
void sw3() {																	//스위치 3번 함수
	if (f == 1)																//1번 눌렀을때
	{
		PORTA = ~led21;
		led21 = ~led21;
		_delay_ms(t);
	}
	else if (f == 2)															//2번 눌렀을때
	{
		PORTA = ~led22;
		led22 = ~led22;
		_delay_ms(t);
	}
	else if (f == 3)															//3번 눌렀을때
	{
		PORTA = ~led23;
		led23 = led23 ^ 0x99;
		_delay_ms(t);
	}
	else if (f == 4)															//4번 눌렀을때
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
	_delay_ms(20);															//채터링 방지용 딜레이
	while (~PIND & 0x08);														//4번 스위치를 땔때까지 기다림
	_delay_ms(20);															//채터링 방지용 딜레이
	if (count % 2 == 1) {														//짝수번 눌렀을때
		t = 200;															//딜레이 200으로 설정
		count++;
	}
	else {																	//홀수번 눌렀을때
		t = 500;															//딜레이 500으로 설정
		count++;
	}

}