/*
 * lamos.h
 *
 *  Created on: 28 May 2018
 *      Author: rclab
 */

#ifndef LAMOS_H_
#define LAMOS_H_

#include "queue.h"

typedef void (*SERVICE)(class Service &);
typedef unsigned (*TIMER)(void);

extern "C" void PendSV_Handler(void);
class Service : public Queuer {
public:
	Service(SERVICE);
	void append(void);
	void append(SERVICE);
	void append(int);
	static void syscall(SERVICE);
private:
	SERVICE service;
	void set(SERVICE);
	friend void PendSV_Handler(void);
};

class Timer : public Queuer {
public:
	Timer(unsigned, TIMER);
	static unsigned getTick(void);
private:
	unsigned count;
	TIMER task;
	friend void nextTimer(Service&);
};

namespace user { void wait(unsigned); }

#endif /* LAMOS_H_ */
