#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000L

#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/power.h>

uint8_t Rx(uint8_t ack);
uint8_t Tx(uint8_t dat);
void stop();
void start();
uint8_t SetAllPorts();
uint8_t Conso();
uint8_t SetClock();
void prevent_wdt_reset();
void setup_wdt();