#include <avr/io.h>
#include <string.h>

//reference: https://marcelmg.github.io/software_uart/
//The code is written for the ATTiny861 Microcontroller

#define TX_PORT PORTB

#define TX_PIN 1

#define TX_DDR DDRB

#define TX_DDR_PIN 1 

#define RX_PORT PORTB

#define RX_PORT_IN PINB

#define RX_PIN 0

#define RX_DDR DDRB

#define RX_DDR_PIN 0 

typedef unsigned char byte;

volatile uint16_t tx_shreg = 0;  //Shift-Register for Transmitter
volatile byte rx_shreg = 0;
volatile byte receiving = 0;
volatile byte receiving_byte = 0;
volatile byte received_ctr = 0;
volatile byte transmitting = 0;
volatile char* receiver_buffer; //Buffer for Receiver... Can only receive BUFFER_SIZE Bytes at a time;
volatile byte buffer_ctr = 0;
volatile byte max_buffer_size = 6;

ISR(TIMER0_COMPA_vect){
  cli();

  if(receiving_byte){
    byte local_rx_shreg = rx_shreg;
    uint16_t local_received_ctr = received_ctr;
    
    TCNT0L = 0;

    if(RX_PORT_IN & (1 << RX_PIN)){
        local_rx_shreg |= (1 << local_received_ctr);
      }
    else{
      local_rx_shreg &= ~(1 << local_received_ctr);
    }
    local_received_ctr += 1;
    
    //Normally you would check if the stop bit is equal to HIGH
    //But this is not necessary
    if(local_received_ctr == 10){      
      receiver_buffer[buffer_ctr] = local_rx_shreg;      
      buffer_ctr += (buffer_ctr < (max_buffer_size - 1)) ? 1 : 0;
      
      local_received_ctr = 0;
      receiving_byte = 0;
      TCCR0B = 0;
    }
    rx_shreg = local_rx_shreg;
    received_ctr = local_received_ctr;    
  }

  else if(transmitting){
    uint16_t local_tx_shreg = tx_shreg;
    if(local_tx_shreg & (1 << 0)){
      TX_PORT |= (1 << TX_PIN);
    }
    else{
      TX_PORT &= ~(1 << TX_PIN);
    }
  
    local_tx_shreg >>= 1;
    tx_shreg = local_tx_shreg;  
  
    if(!local_tx_shreg){
      //Deactivate Timer:
      TCCR0B = 0;
      TCNT0L = 0;
      transmitting = 0;
    }
  }
  
  sei();
}

ISR(PCINT_vect) {  
  //When RX_PIN is low --> receiving
  cli();
  if(!(RX_PORT_IN & (1 << RX_PIN)) && !receiving_byte){

    TCNT0L = 0;           //Set the Timer to 0x00
    TCCR0B = (1 << CS01); //Activate the Baud-Timer
    receiving = 1;        //Set Flags
    receiving_byte = 1;
    
    rx_shreg = 0;   
    
  }
  sei();
}



void UART_init(byte timer_compare){
  receiver_buffer = (char*)malloc(max_buffer_size * sizeof(char));
  
  //TX-Pin as Output
  TX_DDR |= (1 << TX_DDR_PIN);
  //Default High
  TX_PORT |= (1 << TX_PIN);
  
  //RX-Pin as Input (pullup)
  RX_DDR &= ~(1 << RX_DDR_PIN);
  RX_PORT |= (1 << RX_PIN);
  
  //set the hardware timer to CTC mode
  TCCR0A |= (1 << CTC0);
  //enable CTC Interrupt when Timer hits value of OCR0A 
  TIMSK |= (1 << OCIE0A);
  
  TCCR0B = 0; //Deactivate Clock for Timer
  //Calculating Value for OCR0A:
  /*
    The Value of the Output-Compare-Unit depends on the frequency the CPU of the chip is running on
    To achieve a baudrate of 9600 having the cpu run on 8MHz,
    you can calculate the value as follows:
      timer_compare = (f_cpu/(baud*timer_prescaler)) - 1
    if you are using an internal oscillator for timing, you can tweak this value (+- 1/2)
    to achieve the best results.
  */
  OCR0A = timer_compare;

  //Inititialise Pin-Change-Interrupt for RX-Pin
  //PCINT8 is for PB0 at ATTiny861
  GIMSK |= (1 << PCIE0);
  PCMSK1 = 0b00000001;

  
  
  sei();
}

void UART_transmitByte(char character){
  transmitting = 1;
  uint16_t local_tx_shreg = tx_shreg;
  if(local_tx_shreg){                                                                                                                                                                               
    return;
  }
  else{
    //Transmit-Sequence:
    //  0     1 2 3 4 5 6 7 8   9
    //  Start-Bit LSB             MSB   Stop-Bit              
    local_tx_shreg = (character << 1);
    local_tx_shreg &= ~(1 << 0);
    local_tx_shreg |= (1 << 9);
    
    tx_shreg = local_tx_shreg;
    TCCR0B = (1 << CS01);
    TCNT0L = 0;
  }
}

void UART_transmitString(char* string){
  while(*string != 0){
    UART_transmitByte(*(string++));
    while(transmitting){
    }
  }
}

void UART_resetReceiver(){
  cli();
  for(byte i = 0; i < max_buffer_size - 1; i++){
    receiver_buffer[i] = '\0';
  }
      
  receiving = 0;
  receiving_byte = 0;
  buffer_ctr = 0;
  TCCR0B = 0;
  
  sei();
}
