#include <stm32f10x.h>
#include <lamos.h>
#include "wheel.h"

/*
PA1 = TIM2_CH2, PB7 = TIM4_CH2

PA6 = TIM3_CH1 = AENBL
PA7 = TIM3_CH2 = BENBL
PA0 = BPHASE
PC15 = APHASE
PC14 = M1

PB12 = IR LED / MODE
PB13 = SPI2_SCK
PB14 = SPI2_MISO
PB15 = SPI2_MOSI
*/

#define PERIOD 44999	// period = 2.5 ms (400 Hz)

void __attribute__((weak)) wheel(wheel_struct &p) { p.left = p.right = 0; }

namespace
{

wheel_mode mode = FORWARD;

}

void wheel(wheel_mode m) {
  switch (mode = m) {
  case FORWARD:
    GPIOA->BRR = 1; GPIOC->BRR = 0x8000;
    break;
  case BACKWARD:
    GPIOA->BSRR = 1; GPIOC->BSRR = 0x8000;
    break;
  case LEFT:
    GPIOA->BSRR = 1; GPIOC->BRR = 0x8000;
    break;
  case RIGHT:
    GPIOA->BRR = 1; GPIOC->BSRR = 0x8000;
    break;
  default:;
  }
}

namespace
{

wheel_struct val;
struct {unsigned short left, right;} last, current;

SERVICE constructor(void) {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;
  TIM3->PSC = 3;			// 72/4 = 18 MHz
  TIM3->ARR = PERIOD;
  TIM2->SMCR = TIM4->SMCR = 0x67;	// TI2FP2
  TIM2->CCMR1 |= 0x100;			// CC2 as input, IC2 mapped on TI2
  TIM4->CCMR1 |= 0x100;
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM4->CR1 |= TIM_CR1_CEN;

  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;
  int
  i = GPIOA->CRL & 0x00fffff0;		// select PA7, PA6, & PA0
  GPIOA->CRL = i | 0xaa000002;		// a = AFPP, 2 = PP(2MHz)
  i = GPIOC->CRH & 0x00ffffff;		// select PC15 & PC14
  GPIOC->CRH = i | 0x22000000;		// 2 = PP(2MHz)
  TIM3->CCMR1 = 0x6868;				// ch2 & ch1: PWM1, preload
  TIM3->CCER = TIM_CCER_CC2E | TIM_CCER_CC1E;	// OC output enable

  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;		// enable clock for PORTB
  i = GPIOB->CRH & 0xff00ffff;			// select PB13, PB12
  GPIOB->CRH = i | 0x00a20000;			// a = AFPP, 2 = PP 2MHz
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;		// power up SPI2
  SPI2->CR2 |= SPI_CR2_RXNEIE;
  SPI2->CR1 = SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_MSTR | SPI_CR1_BR |
              SPI_CR1_SSM | SPI_CR1_SSI;
						// 36 MHz / 256 = 140.625 kHz
  SPI2->CR1 |= SPI_CR1_SPE;

  TIM3->CR1 |= TIM_CR1_CEN;		// turn on timer
  TIM3->SR &= ~TIM_SR_UIF;		// clear UIF
  TIM3->DIER |= TIM_DIER_UIE;		// update interrupt enable
  NVIC_EnableIRQ(TIM3_IRQn);

  last.right = TIM2->CNT;
  last.left = TIM4->CNT;

  return [](Service&) {
    if (mode == STOP) { TIM3->CCR1 = TIM3->CCR2 = 0; return; }
    val.left = current.left - last.left; last.left = current.left;
    val.right = current.right - last.right; last.right = current.right;
    wheel(val);
    TIM3->CCR2 = val.left > PERIOD ? PERIOD : val.left;
    TIM3->CCR1 = val.right > PERIOD ? PERIOD : val.right;
  };
}

Service _wheel(constructor());

}

extern "C"
{

void TIM3_IRQHandler(void) {
  current.right = TIM2->CNT;
  current.left = TIM4->CNT;
  val.floor = 0xffffffff;
  GPIOB->BSRR = 0x1000;			// IR LED ON, MODE = parallel load
  SPI2->DR = 0;
  NVIC_EnableIRQ(SPI2_IRQn);
  TIM3->SR &= ~TIM_SR_UIF;
}

void SPI2_IRQHandler(void) {
  char *c = (char*)&val.floor;
  *c = SPI2->DR;
  if (GPIOB->ODR & 0x1000) {
    if (!(val.floor>>=8)) GPIOB->BRR = 0x1000;
    SPI2->DR = 0;
  } else {
    NVIC_DisableIRQ(SPI2_IRQn);
    _wheel.append(0);
  }
}

}
