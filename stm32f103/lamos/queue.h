/*
 * queue.h
 *
 *  Created on: 28 May 2018
 *      Author: rclab
 */

#ifndef QUEUE_H_
#define QUEUE_H_

class Queuer {
public:
    Queuer();
    bool isFree();
private:
    Queuer *next;
    friend class Queue;
};

class Queue {
public:
    Queue();
    int getLength();
    Queuer &push(Queuer&);      // at the beginning
    Queuer &append(Queuer&);    // at the end
    Queuer *pop();
    Queuer *getNext(Queuer*);   // input 0 to get first, return 0 for last
    Queuer *get(int);
    Queuer *remove(int);
    Queuer &remove(Queuer&);
    int getIndex(Queuer&);
private:
    Queuer *last;
    int length;
};

#endif /* QUEUE_H_ */
