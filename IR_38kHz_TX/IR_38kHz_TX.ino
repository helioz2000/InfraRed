yes,/*
 * IR_38kHz_TX.ino
 * 38kHz Pule Width Modulation for Infrared transmission
 * 
 * Credit: jimknapp (http://forum.arduino.cc/index.php/topic,10555.0.html)
 */


// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define SYSCLOCK 16000000  // main system clock (Hz)
#define PULSECLOCK 38000  // Hz

#define IROUT 11    // Interrupt output pin (do not change OC2 = 11, fixed)

uint8_t timer2top(unsigned long freq) ;

void setup() {
 Serial.begin(9600) ;

 // Set timer2 to produce wave form
 /*
  * Compare Output Mode non PWM
  * Toggle OC2A on compare match
  * COM2A0 =1; COM2A1 =0
  */ 
 cbi(TCCR2A,COM2A1) ; // connect OC2A (COM2A0 = 1)
 sbi(TCCR2A,COM2A0) ;

 /*
  * Waveform Generation Mode
  * Mode of operation = CTC - Clear Timer on Compare Match
  * TOP (max counter value) = OCRA register
  * Update of OCR02 = Immediate
  * TOV FLag Set on = MAX
  * WGM22 =0; WGM21=1; WGM20-0
  */
 cbi(TCCR2B,WGM22) ;  // CTC mode for TIMER2
 sbi(TCCR2A,WGM21) ;
 cbi(TCCR2A,WGM20) ;
 
 TCNT2 = 0 ;

 /*
  * Asyncronous Status register
  * Asyncronous Timer clocked from I/O clock (clkI/O)
  * AS2 =0;
  */
 cbi(ASSR,AS2) ;  // use system clock for timer 2

  /*
   * The maximum value, to be overwritten further on
   */
 OCR2A = 255 ;   // set TOP to 255 for now

 /*
  * Clock Set
  * Prescale: 1 (no prescale)
  * CS20 =1; CS21 =0; CS22 =0;
  */
 cbi(TCCR2B,CS22) ;  // TIMER2 prescale = 1
 cbi(TCCR2B,CS21) ;
 sbi(TCCR2B,CS20) ;

 /*
  * Force Output Compare A + B
  * clear
  */
 cbi(TCCR2B,FOC2A) ;  // clear forced output compare bits
 cbi(TCCR2B,FOC2B) ;

 pinMode(IROUT, OUTPUT) ;  // set OC2A to OUPUT

 // set calculate TOP value
 OCR2A = timer2top(PULSECLOCK) ;

 // enable interrupts
 sei();
 
}

// main loop
void loop() {
 Serial.println("Hello, world!") ;  
 delay(5000) ;
}

// return TIMER2 TOP value per given desired frequency (Hz)
uint8_t timer2top(unsigned long freq) {
 return((byte)((unsigned long)SYSCLOCK/2/freq) - 1) ;
}
