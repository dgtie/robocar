/*
 * timer.cpp
 *
 *  Created on: May 28, 2018
 *      Author: ensylam
 */

#include <stm32f10x.h>
#include "lamos.h"

void nextTimer(Service&);

namespace
{

Queue queue;
volatile unsigned tick;

Service timer(nextTimer);

SERVICE constructor(void) {
  SysTick_Config(72000);
  NVIC_SetPriorityGrouping(6U);	//NVIC_PriorityGroup_1
  // 1 bit for preemption, 3 bits for sub priority
  NVIC_SetPriority(SVCall_IRQn, 7);
  NVIC_SetPriority(SysTick_IRQn, 7);
  NVIC_SetPriority(PendSV_IRQn, 15);
  return [](Service&){ tick++; timer.append(); };
}

Service systick(constructor());

} // anonymous

void nextTimer(Service &s) {
	static Timer *t = 0;
	if ((t = (Timer*)queue.getNext(t))) {
		if (!--(t->count)) t->count = (*(t->task))();
		s.append();
	}
}

Timer::Timer(unsigned c, TIMER t) : count(c), task(t) {
	queue.append(*this);
}

unsigned Timer::getTick(void) { return tick; }

namespace user
{

void wait(unsigned ms) { ms += tick; while (tick != ms); }

}

extern "C"
void SysTick_Handler(void) { systick.append(0); }
