#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "tinyuart.h"

char* serial_in;

void setup(){
  UART_init(103); //103 for baud of 9600 and timeout of 2 second
  serial_in = (char*)malloc(max_buffer_size * sizeof(char));
  
  // 3334 --> 300 BAUD
}                             

void loop(){
  if(receiving){
    _delay_ms(1000);  //Timeout
    receiver_buffer[max_buffer_size - 1] = '\0';
    strcpy(serial_in, receiver_buffer);
    UART_resetReceiver();
  }
  else{
    _delay_ms(100);
    UART_transmitString(serial_in);
    _delay_ms(1000);
  }
}  
