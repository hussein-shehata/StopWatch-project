/*
 * project.c
 *
 *  Created on: Sep 15, 2021
 *      Author: Hussein
 */

#include<avr/io.h>
#include<util/delay.h>
#include <avr/interrupt.h>



#define sec1 0
#define sec2 1
#define min1 2
#define min2 3
#define hr1 4
#define hr2 5


unsigned char time[6]={0};


void display(){
	for (unsigned char no_of_pin=0;no_of_pin<6;no_of_pin++){ //to display all the segments
		PORTA &=0xC0; 		//reset the used PINs
		PORTA |=(1<<no_of_pin);		//enable only one segment at a time
		PORTC &=0XF0;				//reset the used PINs
		PORTC |= time[no_of_pin];	//display one number at a time
		_delay_ms(3);
	}
}
//------------------------------the increment functions-----------------------------

void reset(){
	for(unsigned char i=0;i<6;i++){
		time[i]=0;
	}
	TCNT1=0;		 //make the timer start from zero ticks
}

void increment_hr2(){
	if(time[hr2]==9)
			{
				reset();
			}
			else
			{
				time[hr2]++;
			}
}


void increment_hr1(){
	if(time[hr1]==9)
		{
			time[hr1]=0;
			increment_hr2();
		}
		else
		{
			time[hr1]++;
		}

}

void increment_min2(){
	if(time[min2]==5)
		{
			time[min2]=0;
			increment_hr1();
		}
		else
		{
			time[min2]++;
		}
}

void increment_min1(){
	if(time[min1]==9)
		{
			time[min1]=0;
			increment_min2();
		}
		else
		{
			time[min1]++;
		}
}

void increment_sec2(){
	if(time[sec2]==5)
		{
			time[sec2]=0;
			increment_min1();
		}
		else
		{
			time[sec2]++;
		}
}

void increment_sec1(){

	if(time[sec1]==9)
	{
		time[sec1]=0;
		increment_sec2();
	}
	else
	{
		time[sec1]++;
	}
}





void TIMER1_CMP_INIT(){
	TCCR1B=(1<<WGM12)| (1<<CS12) |(1<<CS10);
	TCNT1=0;
	OCR1A=997;         //insert the comparison value.
	TIMSK=(1<<OCIE1A);  //enabling the compare match interrupt
	SREG |=(1<<7);		//enable the global interrupt
}


void INT0_INIT(){
	MCUCR |=(1<<ISC01) ;
	MCUCR &= ~(1<<ISC00); // make it trigger on falling edge
	GICR |=(1<<INT0); 		// enable the module interrupt
	DDRD &= ~(1<<PD2);		// make it output PIN
	PORTD |=(1<<PD2);		// enable the pull up resistor
}

void INT1_INIT(){
	MCUCR |=(1<<ISC10) |(1<<ISC11);	//make it trigger on rising edge
	GICR |=(1<<INT1);				//enable the module interrupt
}



void INT2_INIT(){
	MCUCSR &= ~(1<<ISC2);		// make it trigger on falling edge
	GICR |=(1<<INT2);			// enable the module interrupt
	DDRB &= ~(1<<PB2);			// make it output PIN
	PORTB |=(1<<PB2);			// enable the pull up resistor
}

int main(){
	TIMER1_CMP_INIT();
	INT0_INIT();
	INT1_INIT();
	INT2_INIT();
	DDRA=0X3F;		// make the PINs output PINS
	PORTA &=0XC0;     //to reset all the used bits only so the programs starts with no LED on
	DDRC=0X0F;		// make the PINs output PINS
	PORTC &=0xf0;    //to reset the first 4 bits only so the programs starts with zero as initial counter
	DDRB &= ~(1<<PB2);
	DDRD &= ~(1<<PD2) & ~(1<<PD3);
	while(1){
		display();			//display the time of the stop watch on the 7-segments

	}

}




ISR(TIMER1_COMPA_vect)
{
	increment_sec1();
}


ISR(INT0_vect){
	reset();
}

ISR(INT1_vect){
	TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS10);	//stop the timer ("pause the stop watch")
}

ISR(INT2_vect){
	TCCR1B |= (1<<CS12) |(1<<CS10);					//start the timer("resume the stop watch")
}
