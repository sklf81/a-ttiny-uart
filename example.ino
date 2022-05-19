#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "tinyuart.h"

char* serial_in;

void setup(){
  UART_init(103); //103 for baud of 9600
  serial_in = (char*)malloc(max_buffer_size * sizeof(char));
}                             

void loop(){
  if(receiving){
    _delay_ms(1000);  //Timeout
    //Add EOL-Terminator at end of string:
    receiver_buffer[max_buffer_size - 1] = '\0';
    //Copy the buffer to another string and then reset the Receiver
    strcpy(serial_in, receiver_buffer);
    UART_resetReceiver();
  }
  else{
    UART_transmitString(serial_in);
    _delay_ms(1000);
  }
}  
