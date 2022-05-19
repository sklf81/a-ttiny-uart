# This is A-Ttiny-UART program
As the name implies this is a library written for the sake of simölicity and efficiency.
The library has got to bne highly integrated into the program and it may be tricky to understand at first glance.

For the Transmitter of the UART library i got the source code and all relevant information from Marcel Meyer-Garcia (https://github.com/MarcelMG/AVR8_BitBang_UART_TX)
If you find yourself trying to get a µC to communicate over UART but don'T have a proper crystal for the timing of it all, this library might come handy. 
With the timer_comperator paramter of the UART_init() function, you can calibrate a hardware timer of the µC to form a Clock-Divider that provides the frequency for the desired baudrate.
When you notice that you receive the wrong symbols on your Serial-Terminal you might consider tweaking the value of timer_compare a little that the counter times at the right pace.
