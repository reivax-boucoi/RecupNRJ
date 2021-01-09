
#define F_CPU 1000000			//1MHz, match to device fuse settings

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define VCAP_PIN PINB2 			//Analog input, ADC1, supercapacitor voltage monitoring
#define ATIM_PWR_PIN PINB3		//Output, High=ATIM powered
#define ETAS_PWR_PIN PINB1		//Output, High=ETAS powered
#define ATIM_IN1_PIN PINB0		//Output, High=ATIM should take measurement
#define ETAS_ERR_PIN PINB4		//Input, LOW=ETAS ready for measurement


#define ATIM_WAKEUP_DELAY 50 	//delay between ATIM powerup and ETAS powerup, in ms (50ms)
#define ETAS_MEASURE_DELAY 5000 //measurement delay (5s)
#define ETAS_ERR_BLANKING 500 // blanking period at ETAS powerup before reading the ETAS_ERR_PIN, in 10ms steps (here 5s)
#define ETAS_ERR_TIMEOUT 3500 // timeout in case ETAS_ERR_PIN always HIGH (probe never ready), in 10ms steps (here 35s)

#define VCAP_THRESHOLD 512 		//VCAP_THRESHOLD = Vthreshold(Volts) / Vcc(Volts)*1024 (512<=>2.5V)

#define SLEEP_DURATION 0b100001	//delay between every wakeup (8s)
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

#define SLEEP_TIME   (5*60)			//approximate sleep time in seconds (5min)
#define SLEEP_MAXCNT (SLEEP_TIME/8)	//Number of wakeup cycles before taking a measurement (37)


volatile uint16_t sleep_cnt=0;
volatile int16_t timeout_cnt=0;

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
	
	//disable unused peripherals to save power
	power_timer0_disable();
	power_timer1_disable();
	power_usi_disable();
//	power_all_disable();
//  power_adc_enable();

	//initialize GPIOs
	DDRB  |=  (1<<ATIM_PWR_PIN) | (1<<ETAS_PWR_PIN) | (1<<ATIM_IN1_PIN);						//initialize GPIOs as outputs
	DDRB  &=~((1<<ETAS_ERR_PIN) | (1<<VCAP_PIN));												//initialize GPIOs as inputs
	PORTB &=~((1<<ATIM_PWR_PIN) | (1<<ETAS_PWR_PIN) | (1<<ATIM_IN1_PIN) | (1<<ETAS_ERR_PIN));	//disable internal pullups

	while ( 1 ){
		
		
		if(sleep_cnt++ > SLEEP_MAXCNT){
			sleep_cnt=0;
			timeout_cnt=-ETAS_ERR_BLANKING;	//load blanking period
			WDTCR=0x00;	//disable WD
			
			//Measure capacitor voltage
			ADMUX =0x01;		//ADC1 channel
			ADCSRA = (1<<ADEN) | (1<<ADPS2);
			ADCSRA |= (1 << ADSC);
			while (ADCSRA & (1 << ADSC));			//wait for conversion
			uint16_t vCap = ADCL; 					//get conversion result
			vCap = ADCH<<8 | vCap;

			if(vCap > VCAP_THRESHOLD){				//measurement is possible
		
				PORTB |= (1<<ATIM_PWR_PIN);			//ATIM powerup
				_delay_ms(ATIM_WAKEUP_DELAY);
				PORTB |= (1<<ETAS_PWR_PIN);			//ETAS powerup
				
				

				while (timeout_cnt++ <= ETAS_ERR_TIMEOUT){
					_delay_ms(10);
					if (!(PINB & (1<<ETAS_ERR_PIN)) && timeout_cnt>0){//do not sample ETAS_ERR_PIN until blanking period has elapsed
						PORTB |= (1<<ATIM_IN1_PIN);		//ATIM takes measurement
						_delay_ms(ETAS_MEASURE_DELAY); 	//wait for measurement to be done
						break;
					}
				}
				
				PORTB &=~ (1<<ETAS_PWR_PIN);		//ETAS powerdown
				PORTB &=~ (1<<ATIM_PWR_PIN);		//ATIM powerdown
				PORTB &=~ (1<<ATIM_IN1_PIN);
			}
		}
		gotoSleep();
	}
  return 0;
}

ISR(WDT_vect){
}
