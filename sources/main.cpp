#include "led.h"
#include "usart1.h"
#include "wheel.h"
#include "i2cm.h"
#include <lamos.h>

char b[2];

int main(void) {
  user::wait(200);
  if (user::i2cmWrite(60, 2, b)) led_on();
  while(1);
}

int x = 0x12345678;

//void on_button_down(void) { x++; usart1_tx((char*)&x, 4); }
void on_button_down(void) {
  b[1] ^= 10;
  led_toggle();
  i2cm_write(60, 2, b);
}

int zero = 0;
int one = -1;

void usart1_receive(char c) {
  if (c == 'n') led_on();
  if (c == 't') led_toggle();
  if (c == 'x') usart1_tx((char*)&x, sizeof(x));
  if (c == 'y') usart1_tx((char*)&zero, sizeof(x));
  if (c == 'z') usart1_tx((char*)&one, sizeof(x));
  if (c == 'f') wheel(FORWARD);
  if (c == 'b') wheel(BACKWARD);
  if (c == 'l') wheel(LEFT);
  if (c == 'r') wheel(RIGHT);
  if (c == 's') wheel(STOP);
}
