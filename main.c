/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>

// pins
// 1: PB5 (PCINT5/RESET/ADC0/dW)
// 2: PB3 (PCINT3/CLKI/ADC3)
// 3: PB4 (PCINT4/ADC2)
// 4: GND
// 5: PB0 (MOSI/AIN0/OC0A/PCINT0)
// 6: PB1 (MISO/AIN1/OC0B/INT0/PCINT1)
// 7: PB2 (SCK/ADC1/T0/PCINT2)
// 8: VCC
void setup_pins () {
    // pin3 is analog input, connected to variable registor
    // GND ~ VCC
    DDRB &=~  _BV( DDB4 );

    ADMUX  &=~ _BV( REFS0 ); // 0: Vcc used as analog reference
    ADMUX  |=  _BV( ADLAR ); // ADC Left Adjust Result
    ADMUX  |=  _BV( MUX1 );  // MUX1..0 = 10b ADC2 (PB4)
    ADMUX  &=~ _BV( MUX0 );

    ADCSRA &=~ _BV( ADPS2 ); // ADPS2..0 ADC Prescaler Selection: 1/8
    ADCSRA |=  _BV( ADPS1 );
    ADCSRA |=  _BV( ADPS0 );
    ADCSRA |=  _BV( ADEN );  // enable
    ADCSRA |=  _BV( ADSC );  // start conversion

    // pin5 is OC0A (output compare output)
    // use Timer/Counter0's CTC (Clear Timer on Compare) mode
    // fOCnx = fclk / (2 x N x (1 + OCRnx))
    // fOC0A = 9.6MHz / (2 x 8 x 1)   = 600kHz
    // fOC0A = 9.6MHz / (2 x 8 x 256) = 2.3kHz
    // fOC0A = 9.6MHz / (2 x 64 x 1)   = 75kHz
    // fOC0A = 9.6MHz / (2 x 64 x 256) = 293Hz
    DDRB |= _BV( DDB0 );
    TCCR0A &=~ _BV( COM0A1 ); // Clear OC0A on Compare Match
    TCCR0A |=  _BV( COM0A0 );
    TCCR0A |=  _BV( WGM01 ); // CTC mode
    TCCR0A &=~ _BV( WGM00 );
    TCCR0B &=~ _BV( WGM02 );
    TCCR0B &=~ _BV( CS02 );
    TCCR0B |=  _BV( CS01 );
    // TCCR0B |=  _BV( CS00 ); // clkIO/64
    TCCR0B &=~ _BV( CS00 ); // clkIO/8
    OCR0A   =  128; // Output compare register
}

uint8_t readADC() {
    ADCSRA |= _BV( ADIF ); // clear ADC Interrupt flag
    ADCSRA |= _BV( ADSC ); // start

    // wait til ADIF is 1
    while (! (ADCSRA & _BV( ADIF ))) ;

    return ADCH;
}

void delay_ms(uint8_t ms) {
    uint8_t i;
    for (i=0; i<ms; i++) {
        _delay_us(1);
    }
}

int main(void)
{
    setup_pins();

    for (;;) {
        uint8_t pin3 = readADC();

        OCR0A = pin3; // Output compare register

        delay_ms(10);
    }
    return 0;
}
