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

    // pin5 is digital out
    DDRB |= _BV( DDB0 );
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

void delay_us(uint16_t us) {
    uint16_t i;
    for (i=0; i<us; i++) {
        _delay_us(1);
    }
}

/*
 0-255 [-] -> 50 - 5000 [us] -> 20k - 200 [Hz]
 return 25-2575
 */
uint16_t toMicroSecond(uint8_t input) {
    return 25 + ((uint16_t)(input) * 10);
}

int main(void)
{
    setup_pins();

    for (;;) {
        uint8_t pin3 = readADC();
        uint16_t us  = toMicroSecond(pin3);

        PORTB |=  _BV( PORTB0 );
        delay_us(us);

        PORTB &=~ _BV( PORTB0 );
        delay_us(us);
    }
    return 0;   /* never reached */
}
