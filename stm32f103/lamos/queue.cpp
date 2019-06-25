/*
 * queue.cpp
 *
 *  Created on: 28 May 2018
 *      Author: rclab
 */

#include "queue.h"

Queuer::Queuer() : next(0) {}

bool Queuer::isFree() { return !next; }

Queue::Queue() : last(0), length(0) {}

int Queue::getLength() { return length; }

Queuer &Queue::push(Queuer &p) {
	if (!p.next) {
		length++;
		if (last) p.next = last->next; else last = &p;
		last->next = &p;
	}
	return p;
}

Queuer &Queue::append(Queuer &p) {
	if (!p.next) last = &push(p);
	return p;
}

Queuer *Queue::pop() {
	if (!last) return 0;
	length--;
	Queuer *p = last->next;
	last->next = p->next;
	p->next = 0;
	if (!last->next) last = 0;
	return p;
}

Queuer *Queue::getNext(Queuer *p) {
	if (p) return p == last ? 0 : p->next;
	return last ? last->next : 0;
}

Queuer *Queue::get(int i) {
	if (i >= length) return 0;
	Queuer *p = last;
	while (i--) p = p->next;
	return p->next;
}

Queuer *Queue::remove(int i) {
	if (i >= length) return 0;
	Queuer *p, *q;
	p = q = last;
	while (i--) p = p->next;
	last = p;
	p = pop();
	if (p != q) last = q;
	return p;
}

Queuer &Queue::remove(Queuer &q) {
	if (!last) return q;
	Queuer *p = last, *r = last;
	while (p->next != &q) if ((p = p->next) == last) return q;
	last = p;
	p = pop();
	if (p != r) last = r;
	return q;
}

int Queue::getIndex(Queuer &q) {
	if (!last) return -1;
	Queuer *p = last;
	int i = 0;
	while (p->next != &q) { if ((p = p->next) == last) return -1; i++; }
	return i;
}


