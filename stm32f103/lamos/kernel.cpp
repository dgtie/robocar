/*
 * kernel.cpp
 *
 *  Created on: 28 May 2018
 *      Author: rclab
 */

#include <stm32f10x.h>
#include "lamos.h"

namespace
{

class KQueue : Queue {
public:
	KQueue(KQueue *n) { next = n; }
	void append(Service &s) {
		if (popping == this) next->push(s);
		else Queue::append(s);
		if (next) SCB->ICSR |=  SCB_ICSR_PENDSVSET_Msk;
	}
	Service *pop(void) {
		Service *s;
		popping = this; s = (Service*)Queue::pop(); popping = 0;
		if (!s) if (next) return next->pop();
		return s;
	}
private:
	KQueue *next;
	static KQueue *popping;
} kQueue(0), iQueue0(&kQueue);

KQueue *KQueue::popping;

Service service(0);

} // anonymous

Service::Service(SERVICE s) { service = s; }

void Service::append(void) { kQueue.append(*this); }

void Service::append(SERVICE s) { service = s; kQueue.append(*this); }

void Service::append(int) { iQueue0.append(*this); }

void Service::set(SERVICE s) { service = s; }

void Service::syscall(SERVICE s) {
	::service.set(s);
	asm("svc 0");
}

extern "C"
{

void PendSV_Handler(void) {
	Service *s;
	SCB->ICSR |=  SCB_ICSR_PENDSVCLR_Msk;
	while ((s = iQueue0.pop())) (*s->service)(*s);
}

void SVC_Handler(void) { service.append(0); }

}
