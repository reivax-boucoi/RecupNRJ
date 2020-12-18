
#define F_CPU 1000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define VCAP_PIN PINB2 			//Analog input, ADC1, supercapacitor voltage monitoring
#define ATIM_PWR_PIN PINB3		//Output, High=ATIM powered
#define ETAS_PWR_PIN PINB1		//Output, High=ETAS powered
#define ATIM_IN1_PIN PINB0		//Output, High=ATIM should take measurement
#define ETAS_ERR_PIN PINB4		//Input, LOW=ETAS ready for measurement

#define ATIM_WAKEUP_DELAY 50 	//delay between ATIM powerup and ETAS powerup, in ms
#define ETAS_ONTIME_DELAY 2200 	//ETAS ON-time in 10ms steps (2200=>22s)


#define VCAP_THRESHOLD 512 		//VCAP_THRESHOLD = Vthreshold(Volts) /Vcc(Volts)*1024 

#define SLEEP_DURATION 0b100001
//  16 ms:  0b000000
//  32 ms:  0b000001
//  64 ms:  0b000010
//  128ms:  0b000011
//  250ms:  0b000100
//  500ms:  0b000101
//  1  s :  0b000110
//  2  s :  0b000111
//  4  s :  0b100000
//  8  s :  0b100001



void gotoSleep(void){
	//ADCSRA &=~ (1<<ADEN);
	MCUSR  &=~ (1<<WDRF);
	WDTCR  |=  (1<<WDCE) | (1<<WDE);
	WDTCR  =   (1<<WDCE) | SLEEP_DURATION;
	WDTCR  |=  (1<<WDIE);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sei();
	sleep_mode();
	sleep_disable();
	//ADCSRA |= (1<<ADEN);
}


int main(void){

	DDRB  |=  (1<<ATIM_PWR_PIN) | (1<<ETAS_PWR_PIN) | (1<<ATIM_IN1_PIN);
	DDRB  &=~((1<<ETAS_ERR_PIN) | (1<<VCAP_PIN));
	PORTB &=~((1<<ATIM_PWR_PIN) | (1<<ETAS_PWR_PIN) | (1<<ATIM_IN1_PIN));

	while ( 1 ){
		
		WDTCR=0x00;	//disable WD
		
		
		//Measure capacitor voltage
		ADMUX =0x01;		//ADC1 channel
		ADCSRA = (1<<ADEN) | (1<<ADPS2);
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC));			//wait for conversion
		uint16_t vCap = ADCL; 
		vCap = ADCH<<8 | vCap;

		if(vCap>VCAP_THRESHOLD){				//measurement is possible
	
			PORTB |= (1<<ATIM_PWR_PIN);			//ATIM powerup
			_delay_ms(ATIM_WAKEUP_DELAY);
			PORTB |= (1<<ETAS_PWR_PIN);			//ETAS powerup
			
			for(uint16_t cnt=0; cnt<ETAS_ONTIME_DELAY; cnt++){	//wait for 22s
				_delay_ms(10);
				if(!(PINB & (1<<ETAS_ERR_PIN))){	//If ETAS sends no error
					PORTB |= (1<<ATIM_IN1_PIN);	//ATIM takes measurement
				}
			}
			
			PORTB &=~ (1<<ETAS_PWR_PIN);		//ETAS powerdown
			PORTB &=~ (1<<ATIM_PWR_PIN);		//ATIM powerdown
			PORTB &=~ (1<<ATIM_IN1_PIN);
		}
		gotoSleep();
	}
  return 0;
}

ISR(WDT_vect){
}