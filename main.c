#include "header.h"

#define PI2C_SDA    PB2
#define PI2C_SCL    PA7
#define PI2C_VCC    PA6

#define BOUTON_ROUGE    PA4
#define BOUTON_JAUNE    PA5

#define MOSFET_G    PA3

#define LED    PB1


//#define OUT_REG PORTB
//#define IN_REG PINB

#define SDA_ON (PORTB |= (1<< PI2C_SDA))
#define SDA_OFF (PORTB &= ~(1<< PI2C_SDA))
#define SCL_ON (PORTA |= (1<< PI2C_SCL))
#define SCL_OFF (PORTA &= ~(1<< PI2C_SCL))

#define SDA_READ (PINB & (1<<PI2C_SDA))
#define SCL_READ (PINA & (1<<PI2C_SCL))

#define I2C_READ 0x01
#define I2C_WRITE 0x00

#define InitLen 26

#define ADDR 0b01111000 //OLED Address plus write bit

inline void dly(){__asm__("NOP");}; 



const uint8_t Init[InitLen] = {
  0xAE,         // Display OFF
  0xA8, 0x1F,   // set multiplex (HEIGHT-1): 0x1F for 128x32, 0x3F for 128x64
  0xD3, 0x00,   // Display offset to 0
  0x40,         // Set display start line to 0
  0x8D, 0x14,   // Charge pump enabled
  0x20, 0x00,   // Memory addressing mode 0x00 Horizontal 0x01 Vertical
  0xDA, 0x02,   // Set COM Pins hardware configuration to sequential
  0x81, 0x80,   // Set contrast
  0xD9, 0xF1,   // Set pre-charge period
  0xDB, 0x40,   // Set vcom detect  
  
  0x22, 0x00, 0x03, // Page min to max
  0x21, 0x00, 0x7F, // Column min to max

  0xAF  // Display on
};

const uint8_t HELLO[70] ={
    0xff,0x08,0x08,0x08,0x08,0xff,0x00, //H
    0xff,0x89,0x89,0x89,0x89,0x81,0x00, //E
    0xff,0x80,0x80,0x80,0x80,0x80,0x00, //L
    0xff,0x80,0x80,0x80,0x80,0x80,0x00, //L
    0x3c,0x42,0x81,0x81,0x81,0x42,0x3c, //O
    0x00,0x00,0x00,0x00,0x00,0x00,0x00, //' '
    0x01,0x01,0x01,0xff,0x01,0x01,0x01, //T
    0x00,0x81,0x81,0xff,0x81,0x81,0x00, //I
    0xff,0x02,0x04,0x08,0x10,0xff,0x00, //N
    0x01,0x06,0x08,0xf0,0x08,0x06,0x01  //Y
};


// I2C Functions declaration
void start();
void stop();
uint8_t Tx(uint8_t);
uint8_t Rx(uint8_t);

/*
int main(void)
{
    SetClock();
    Conso();
    
    SetAllPorts();
    PORTA |= (1<<PI2C_VCC);
    //DDRB = 3;
    
    
    PORTB |= (1<<LED);
    _delay_ms(100);
    PORTB &= ~(1<<LED);
    _delay_ms(100);
    PORTB |= (1<<LED);
    _delay_ms(100);
    PORTB &= ~(1<<LED);
    _delay_ms(100);
    uint8_t i = 0;

    _delay_ms(100);

    // Display Initialization 
    start();
    Tx(ADDR);
    Tx(0x00);
    for (i = 0; i < InitLen; i++)
    {
        Tx(Init[i]);
    }

    stop();


    // Clear the display 
    start();
    Tx(ADDR);
    Tx(0x40);
    for (i = 0; i < 128; i++)
    {
        Tx(0x00);
        Tx(0x00);
        Tx(0x00);
        Tx(0x00);
    }
    stop();

    // Set position for the text 
    start();
    Tx(ADDR);
    Tx(0x00);
    Tx(0x21);   // Set Column
    Tx(0x19);   // Start at column 25
    Tx(0x7F);   // End at 128
    Tx(0x22);   // Set Page
    Tx(0x01);   // Start at page 1
    Tx(0x01);   // End at page 1
    stop();

    // Write the message
    start();
    Tx(ADDR);
    Tx(0x40);
    for (i = 0; i < 70; i++)
    {
        Tx(HELLO[i]);
    }
    stop();

    _delay_ms(10000);
    SDA_OFF;
    SCL_OFF;
    PORTA &= ~(1<<PI2C_VCC);
            
    for(;;)
    { 
        PORTB |= (1<<LED);
        _delay_ms(5000);
        PORTB &= ~(1<<LED);
        _delay_ms(5000);                
    }
    

}*/


uint8_t SetAllPorts()                       
{
    DDRB |= (1 << DDB0);
    DDRB |= (1 << DDB1);         // led
    DDRB |= (1 << DDB2);         // sda
    DDRB |= (1 << DDB3);         // sda
    
    DDRA |= (1 << DDA0);
    DDRA |= (1 << DDA1);
    DDRA |= (1 << DDA2);
    DDRA |= (1 << DDA3);         // mosfet
    DDRA &= ~(1 << DDA4);        // red
    DDRA &= ~(1 << DDA5);        // yellow
    DDRA |= (1 << DDA6);         // vcc    
    DDRA |= (1 << DDA7);         // scl           
    
    // pull-up
    PORTA |= (1<<PA4);
    PORTA |= (1<<PA5);
    return 1;
}

/*  i2c start sequence */
void start(){
    SDA_ON;
    dly();
    SCL_ON;
    dly();
    SDA_OFF;
    dly();
    SCL_OFF;
    dly();
}


/*  i2c stop sequence */
void stop(){
    SDA_OFF;
    dly();
    SCL_ON;
    dly();
    SDA_ON;
    dly();
}

/* Transmit 8 bit data to slave */
uint8_t Tx(uint8_t dat)
{
    uint8_t i = 0;
    
    for(i = 0; i<8; i++){
        (dat & 0x80) ? SDA_ON : SDA_OFF;
        dat<<=1;    
        dly();
        SCL_ON;
        dly();
        SCL_OFF;
        dly();
    }

    SDA_ON;
    SCL_ON;
    dly();
    uint8_t ack = !SDA_READ;    // Acknowledge bit
    SCL_OFF;
    return ack;
}

/* Receive 8 bit packet data from the slave. Check for clock stretching*/
uint8_t Rx(uint8_t ack){
    uint8_t dat = 0;
    uint8_t i = 0;
    
    SDA_ON;
    for( i =0; i<8; i++){
        dat <<= 1;
        do{
            SCL_ON;
        }while(SCL_READ == 0);  //SCL stretching
        dly();
        if(SDA_READ) dat |=1;
        dly();
        SCL_OFF;
    }
    ack ? SDA_OFF : SDA_ON;
    SCL_ON;
    dly();
    SCL_OFF;
    SDA_ON;
    return(dat);
}

uint8_t Conso()
{
    PRR|=(1<<PRTIM0);             // Turn timer0 off.
    PRR|=(1<<PRTIM1);             // Turn timer1 off.
    
    ADCSRA &= ~(1 << ADEN);       // disable the ADC
    PRR|=(1<<PRADC);              // Turn ADC off.
    PRR|=PRUSI;                   // Turn USI off.
    ACSR|=(1<<ACD);               // Turn off Analog comparator.    
    return 1;
}


/*
CLKPS3 CLKPS2 CLKPS1 CLKPS0 Clock Division Factor
0 0 0 0 1
0 0 0 1 2
0 0 1 0 4
0 0 1 1 8
0 1 0 0 16
0 1 0 1 32
0 1 1 0 64
0 1 1 1 128
1 0 0 0 256
*/
uint8_t SetClock()
{
    cli(); // Disable interrupts
    CLKPR = (1<<CLKPCE); // Prescaler enable
    CLKPR = ((0<<CLKPS3) | (0<<CLKPS2) | (1<<CLKPS1) | (1<<CLKPS0)); // Clock division factor 8
    sei(); // Enable interrupts
    return 1;
}


int main(void)
{    
    
    SetClock();
    Conso();
    
    SetAllPorts();
    PORTA &= ~(1<<PI2C_VCC);
    SDA_OFF;
    SCL_OFF;
    
    
    PORTB &= ~(1<<LED);
    _delay_ms(1000);
    PORTB |= (1<<LED);
    _delay_ms(1000);
    PORTB &= ~(1<<LED);
    _delay_ms(1000);
    PORTB |= (1<<LED);
    _delay_ms(1000);
    
    PORTB &= ~(1<<LED);
    _delay_ms(300);
    PORTB |= (1<<LED);
    _delay_ms(300);
    PORTB &= ~(1<<LED);
    _delay_ms(300);
    PORTB |= (1<<LED);
    _delay_ms(300);
    
    uint16_t i = 0;
    while(1)
    {
        PORTA |= (1<<MOSFET_G);
        _delay_ms(12000);
        PORTA &= ~(1<<MOSFET_G);
        // 1 jour = 86400s , = 17280*5s, = 2880*30s, =5760*15s
        for(i = 0;i < 5760;i++)
        {             
            _delay_ms(14900);
            PORTB &= ~(1<<LED);
            _delay_ms(100);
            PORTB |= (1<<LED);
        }
    }
    
}