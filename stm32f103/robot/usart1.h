#ifndef USART1_H_
#define USART1_H_

bool usart1_printString(const char *);
bool usart1_tx(char*, unsigned);

// user optionally provide
// void usart1_receive(char);

#endif /* USART1_H_ */
