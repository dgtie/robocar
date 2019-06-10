#include <stm32f10x.h>
#include <lamos.h>

#define LED 0x100
#define BUTTON GPIOB->IDR & 0x40

extern "C" void f(void) {}
void on_button_up(void) __attribute__((weak, alias("f")));
void on_button_down(void) __attribute__((weak, alias("f")));

void led_on(void) { GPIOB->BSRR = LED; }
void led_off(void) { GPIOB->BRR = LED; }
void led_toggle(void) { GPIOB->ODR & LED ? led_off() : led_on(); }

namespace
{

int last;

TIMER constructor(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  int
  i = GPIOB->CRH & 0xfffffff0;	// select PB8
  GPIOB->CRH = i | 0x00000002;	// push-pull 2 MHz
  last = BUTTON;
  return [](){
    int b = BUTTON;
    if (b != last) ((last = b)) ? on_button_up() : on_button_down();
    return 100U;
  };
}

Timer button(100, constructor());

} //anonymous
