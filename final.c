#define F_CPU 16000000UL
#define BAUD 9600


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <util/setbaud.h>
#include <avr/pgmspace.h>

//@
void    uart_init(void);
void    uart_putchar(unsigned char c);
int     uart_putchar_printf(char var, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar_printf, NULL, _FDEV_SETUP_WRITE);
//@

void init_reader(void);
float read_sensor(void);

void PWM_init(void);
void motor_init(void);
void motor(int left_s,int right_s);


int sensor_1;
int sensor_2;
int sensor_3;
int sensor_4;
int sensor_5;
float err = 0;


//============================================   MAIN CODE   ==================================================

int main(void){
	
	//@
	uart_init();
	stdout = &mystdout;
	printf("\n ");

	int a;
	char str[16];
	a = 0;
	//@
	
	float ERR = 0;
	float last_ERR = 0;
	
	float kp= 0.05;
	float kd= 0;
	float ki= 0;
	
	float t_err = 0;
	
	int dif = 0;
	
	int m1=0;
	int m2=0;
	
	
	PWM_init();
	motor_init();
	
	DDRC &= ~(_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	PORTC |= (_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	PINC &= ~(_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	
	motor(0,0);
	_delay_ms(1000);


 while(1){
 
		
		ERR = read_sensor();
		
		
		m1=120;
		m2=120;
		
		t_err = t_err+ERR;
		
		dif = (ERR*kp)+((ERR-last_ERR)*kd)+(t_err*ki);
		
		last_ERR = ERR;
		
		itoa(ERR, str, 10);
		printf(str);
		printf("     \n");
		
		
		motor(m1+dif,m2-dif);
		
		
		
    }
	return 0;
}

//===============================================      Read Sensors      ==================================================

float read_sensor(void){

	
	err = 0;
	sensor_1 = PINC & (_BV(PC0));
	sensor_2 = PINC & (_BV(PC1));
	sensor_3 = PINC & (_BV(PC2));
	sensor_4 = PINC & (_BV(PC3));
	sensor_5 = PINC & (_BV(PC4));
	
	
	
	if(sensor_1 && sensor_2 && sensor_3 && sensor_4 && sensor_5){ //All black
		err = 1;
	}
	
	else if(sensor_1 && sensor_2 && ~sensor_3 && sensor_4 && sensor_5){
		err = 0;
	}
	else if(sensor_1 && ~sensor_2 && ~sensor_3 && ~sensor_4 && sensor_5){
		err = 0;
	}
	
	else if(sensor_1 && sensor_2 && ~sensor_3 && ~sensor_4 && sensor_5){
		err = 1;
	}
	else if(sensor_1 && ~sensor_2 && ~sensor_3 && sensor_4 && sensor_5){
		err = -1;
	}
	
	else if(sensor_1 && sensor_2 && ~sensor_3 && ~sensor_4 && ~sensor_5){
		err = 2;
	}
	else if(~sensor_1 && ~sensor_2 && ~sensor_3 && sensor_4 && sensor_5){
		err = -2;
	}
	
	else if(sensor_1 && sensor_2 && sensor_3 && ~sensor_4 && ~sensor_5){
		err = 3;
	}
	else if(~sensor_1 && ~sensor_2 && sensor_3 && sensor_4 && sensor_5){
		err = -3;
	}
	
	else if(sensor_1 && sensor_2 && sensor_3 && sensor_4 && ~sensor_5){
		err = 4;
	}
	else if(~sensor_1 && sensor_2 && sensor_3 && sensor_4 && sensor_5){
		err = -4;
	}
	
	return err;
}

//========================================================================================

void PWM_init(void){
	DDRD   |= _BV (PD5) | _BV (PD6);
	// 0 100% / 255 0%
	OCR0A = 100;
	OCR0B = 100;
	// Inverted fast PWM / 64 prescalar
	TCCR0A = (1 << COM0A1) | (1 << COM0A0) | (1 << COM0B1) | (1 << COM0B0) | (1<<WGM01) | (1<<WGM00); 
	TCCR0B = (1<<CS01) | (1 << CS00); 
}


//=================================================       Motor        =================================================

void motor_init(void){
	DDRD   |= _BV (PD3) | _BV (PD4);    // Left direction outputs
	DDRB   |= _BV (PB1) | _BV (PB2);    // Right direction outputs
}

void motor(int left_s,int right_s){
	if(left_s > 0){
		if(left_s > 255){left_s=255;}
		PORTD |= _BV(PD3);
		PORTD &= ~_BV(PD4);
		OCR0A = 255-left_s;
	}
	if(left_s < 0){
		if(left_s < -255){left_s = -255;}
		PORTD |= _BV(PD4);
		PORTD &= ~_BV(PD3);
		OCR0A = 255-(left_s*(-1));
	}
	if(left_s == 0){
		PORTD |= _BV(PD4);
		PORTD |= _BV(PD3);
		OCR0A = 255;
	}
	
	if(right_s > 0){
		if(right_s > 255){right_s=255;}
		PORTB |= _BV(PB1);
		PORTB &= ~_BV(PB2);
		OCR0B = 255-right_s;
	}
	if(right_s < 0){
		if(right_s < -255){right_s = -255;}
		PORTB |= _BV(PB2);
		PORTB &= ~_BV(PB1);
		OCR0B = 255-(right_s*(-1));
	}
	if(right_s == 0){
		PORTB |= _BV(PB1);
		PORTB |= _BV(PB2);
		OCR0B = 255;
	}
	
}


/**********************************************************
*  UART functions											==========================================================================================================
***********************************************************/

void uart_init(void) {

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);  /* 8-bit N 1 Atmega328 */
    UCSR0B |= _BV(RXEN0)  | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putchar(unsigned char c) {
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}


int uart_putchar_printf(char var, FILE *stream) {
    if (var == '\n') uart_putchar('\r');
    uart_putchar(var);
    return 0;
}

//===========================================================================================================================================================================

