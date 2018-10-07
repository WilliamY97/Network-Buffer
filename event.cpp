#include "event.h"
#include "random.h"

SEC* nSEC = new SEC();

struct event * createEvent(char structType, float val, int packetLength)
{
	struct event *cursor;
	cursor = (struct event *)malloc(sizeof(struct event));
	if (cursor != NULL)
	{
		cursor->eventType = structType;
		cursor->val = val;
		if (structType == 'A') {
			cursor->len = expo((1.0/(float)packetLength));
		}
		else {
			cursor->len = packetLength;
		}
		return cursor;
	}
	else {
		return NULL;
	}
	free(cursor);
}

void addEvent(struct event * newEvent)
{
	if (nSEC -> first != NULL) {
		nSEC->last->next = newEvent;
		newEvent->next = NULL;
		nSEC -> last = newEvent;

	}
	else {
		nSEC -> first = newEvent;
		newEvent->next = NULL;
		nSEC -> last = newEvent;
	}
}

void insertEvent(struct event *newEvent, struct event *pivot)
{
	struct event *tmp;
	struct event *prev;

	if (nSEC -> first != NULL) {
		tmp = pivot;
		while (tmp->val <= newEvent->val) {
			tmp = tmp->next;
			if (tmp == NULL) {
				break;
			}
		}

		if (tmp == nSEC -> first) {
			newEvent -> next = nSEC -> first;
			nSEC -> first = newEvent;
		}
		else {
			prev = pivot;
			while (prev->next != tmp) {
				prev = prev->next;
			}
			newEvent->next = tmp;
			prev->next = newEvent;
			if (nSEC -> last == prev) {
				nSEC -> last = newEvent;
			}
		}
	}
	else {
		nSEC -> first = newEvent;
		nSEC -> first->next = NULL;
		nSEC -> last = newEvent;
	}
}

void deleteEvent(struct event *cursor)
{
	struct event *tmp;
	struct event *prev;
	tmp = cursor;
	prev = nSEC -> first;

	if (tmp == prev)
	{
		nSEC -> first = nSEC -> first->next;
		if (nSEC -> last == tmp)
		{
			nSEC -> last = nSEC -> last->next;
		}
		free(tmp);

	}
	else
	{
		while (prev->next != tmp)
		{
			prev = prev->next;
		} 
		prev->next = tmp->next;
		if (nSEC -> last == tmp)
		{
			nSEC -> last = prev;
		}
		free(tmp);
	}
}