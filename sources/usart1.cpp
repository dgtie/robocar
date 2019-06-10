// usart1: pa9=tx pa10=rx

#include <stm32f10x.h>
#include <lamos.h>

void __attribute__((weak)) usart1_receive(char c) { USART1->DR = c; }

namespace
{

const char* string;
unsigned count;

int getChar(void) {
  if (count) {
    unsigned char c = *string++;
    if (!--count) string = 0;
    return c;
  }
  else return string ? (*string ? *string++ : -1) : -1;
}

SERVICE constructor(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  int
  i = GPIOA->CRH & 0xffffff0f;
  GPIOA->CRH = i | 0x000000a0;	// a = AFPP
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;	// PCLK2 = 72 MHz ?
  USART1->BRR = 625;	// = PCLK2 / 115200
  USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
  USART1->CR1 |= USART_CR1_UE;		// USART enabled
  NVIC_EnableIRQ(USART1_IRQn);
  return [](Service&) {
    int sr = USART1->SR;
    if (sr & USART_SR_RXNE) usart1_receive(USART1->DR);
    if (sr & USART_SR_TXE)
      (sr = getChar()) == -1 ?
        USART1->CR1 &= ~USART_CR1_TXEIE : USART1->DR = sr;
    NVIC_EnableIRQ(USART1_IRQn);
  };
}

Service uart(constructor());

} //anonymous

extern "C"
void USART1_IRQHandler(void) {
  NVIC_DisableIRQ(USART1_IRQn);
  uart.append(0);
}

bool usart1_printString(const char* p) {
  if (count) return false;
  if (string) if (*string) return false;
  string = p;
  USART1->CR1 |= USART_CR1_TXEIE;
  return true;
}

bool usart1_tx(char *p, unsigned l) {
  if (count) return false;
  if (string) if (*string) return false;
  if (!(count = l)) return false;
  string = p;
  USART1->CR1 |= USART_CR1_TXEIE;
  return true;
}
