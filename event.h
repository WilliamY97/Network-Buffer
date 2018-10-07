#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

struct event {
    char eventType;
    float val;
    float len;
    struct event* next;
};

class SEC {
	public:
		struct event *first = (struct event *) NULL;
		struct event *last = (struct event *) NULL;
};

extern SEC* nSEC;

struct event * createEvent(char type, float val, int packetLength);
void addEvent(struct event * newEvent);
void insertEvent(struct event *newEvent, struct event *pivot);
void deleteEvent(struct event *cursor);
#endif // EVENT_H_INCLUDED