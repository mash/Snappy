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

    // pin5 is digital out
    DDRB |= _BV( PB0 );
}

int main(void)
{
    setup_pins();

    for (;;) {
        PORTB |=  _BV( PB0 );
        _delay_ms(200);

        PORTB &=~ _BV( PB0 );
        _delay_ms(200);
    }
    return 0;   /* never reached */
}
