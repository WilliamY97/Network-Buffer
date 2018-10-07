#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include "random.h"
#include "event.h"

struct result {
	long double numPackets;
	int packetLength;
	int serviceSpeed;
	float utilization;
	float avgPackets;
	float pIdle;
	float pDrop;
};

struct result simResult;

void generateObservers(int alpha, int period)
{
 	float currentTime;
	struct event *cursor;

	while (currentTime <= period) {
		currentTime += expo(alpha);
		
		if (currentTime > period) {
			break;
		}

		cursor = createEvent('O', currentTime, 0);
		addEvent(cursor);
	}
}

void generateArrivals(float lambda, int period, int packetLength)
{
 	float currentTime = 0;
	struct event *cursor;
	struct event *prev = nSEC -> first;

	while (currentTime <= period) {
		currentTime += expo(lambda);
		
		if (currentTime > period) {
			break;
		}

		cursor = createEvent('A', currentTime, packetLength);
		insertEvent(cursor, prev);
		prev = cursor;
	}
}

void freeMem( struct event *cursor )
{
   struct event *temp;

   if( nSEC -> first == NULL ) return; 

   if( cursor == nSEC -> first ) {      
       nSEC -> first = NULL;         
       nSEC -> last = NULL;          
   }
   else {
       temp = nSEC -> first;          
       while( temp->next != cursor )         
           temp = temp->next;
       nSEC -> last = temp;                        
   }

   while( cursor != NULL ) {   
      temp = cursor->next;     
      free( cursor );          
      cursor = temp;           
   }
}

int incrementIdle(int numArrivals, int numDepartures, int numIdle){
	if (numArrivals - numDepartures == 0)
	{
		return numIdle += 1;
	}
	else {
		return numIdle;
	}
}

void updateResult (long double numPackets, int numObservers, int numIdle, int numDrop, int numCreated){
	simResult.avgPackets = (float)numPackets/numObservers;
	simResult.pIdle = (float)numIdle/numObservers;
	simResult.pDrop = (float)numDrop/numCreated;
	simResult.numPackets = numPackets;
}

float incrementDepartureTime(float departureTime, float servicedTime, float arrivalTime, int numArrivals, int numDepartures){
	if (numArrivals - numDepartures != 0) {
		departureTime += servicedTime;
	}
	else {
		departureTime = arrivalTime + servicedTime;
	}
	return departureTime;
}

void executeSimulator(int serviceSpeed, int bufferSize)
{
	struct event *cursor = nSEC -> first;
	struct event *new_departure; 
	struct event *temp;

	float arrivalTime = 0;
	float servicedTime = 0;
	float departureTime = 0;
	int numArrivals = 0;
	int numDepartures = 0;
	int numObservers = 0;
	long double numPackets = 0;
	int numIdle = 0;
	int numDrop = 0;
	int numCreated = 0;

	struct event *buffer[bufferSize];
	struct event *maxTime = nSEC -> first;

	while (cursor != NULL)
	{
		if (cursor->eventType == 'A')
		{
			numCreated += 1;

			if (bufferSize == 0 || numArrivals - numDepartures <= bufferSize)
			{
				// packet can be serviced
				arrivalTime = cursor->val;
				servicedTime = (cursor->len/(float)serviceSpeed);
				departureTime = incrementDepartureTime(departureTime,servicedTime,arrivalTime,numArrivals,numDepartures);
				new_departure = createEvent('D', departureTime, 0);
				insertEvent(new_departure, maxTime);
				if (new_departure->val >= maxTime->val)
				{
					maxTime = new_departure;
				}
				numArrivals += 1;
			}
			else
			{
				// buffer is busy - must drop packet
				numDrop += 1;
			}
		}
		else if (cursor->eventType == 'O')
		{
			numObservers += 1;	
			numPackets = numPackets + (numArrivals - numDepartures);
			numIdle = incrementIdle(numArrivals,numDepartures,numIdle);
		}
		else if (cursor->eventType == 'D')
		{
			numDepartures += 1;
		}
		cursor = cursor->next;
	}
 	updateResult (numPackets, numObservers, numIdle, numDrop, numCreated);
}

struct result startSystem(int iterations, float utilization, int length, int speed, int size) {

	int bufferSize = size;

	if (bufferSize == -1){
		bufferSize = 0;
	}

	int period = iterations;
	float util = utilization;
	int packetLength = length;
	int serviceSpeed = speed;
	double totalTime;
	float lambda = (util / packetLength) * serviceSpeed; 
	float alpha = lambda + 10;

	simResult.packetLength = packetLength;
	simResult.serviceSpeed = serviceSpeed;
	simResult.utilization = util;

	generateObservers(alpha, period);
	generateArrivals(lambda, period, packetLength);
	executeSimulator(serviceSpeed, bufferSize);
	
	freeMem(nSEC -> first);
	return simResult;
}

void printMetrics(result simResult) {
	printf("\n<< METRICS >>\n");
	printf("Packet Length: %d bits\n", simResult.packetLength);
	printf("Link Rate: %d bps\n", simResult.serviceSpeed);
	printf("Utilization: %f\n", simResult.utilization);
	printf("E[N]: %f\n", simResult.avgPackets);
	printf("P_idle: %f\n", simResult.pIdle);
	printf("%f\n", simResult.pDrop);
	printf("Number of Packets: %10Lf\n", simResult.numPackets);

}

void Question3(int packetLength, int serviceSpeed, int period) {
	struct result simulationData;
	double startUtil = 0.35;
	double endUtil = 0.85;
	printf("<< QUESTION 3 START >>\n");
	for (double Util = startUtil; Util <= endUtil; Util += 0.1)
	{
		simulationData = startSystem(period, Util, packetLength, serviceSpeed, -1);
		printMetrics(simulationData);
	}
	printf("\n<< QUESTION 3 END >>\n");
}

void Question4(int packetLength, int serviceSpeed, int period) {
	struct result simulationData;
	double startUtil = 1.2;
	printf("<< QUESTION 4 START >>\n");
	simulationData = startSystem(period, startUtil, packetLength, serviceSpeed, -1);
	printMetrics(simulationData);
	printf("\n<< QUESTION 4 END >>\n");
}

void Question6a(int packetLength, int serviceSpeed, int period, int bufferSize) {
	struct result simulationData;
	double startUtil = 0.5;
	double endUtil = 2.1;
	printf("<< QUESTION 6 START >>\n");
	for (double Util = startUtil; Util <= endUtil; Util += 0.1)
	{
		simulationData = startSystem(period, Util, packetLength, serviceSpeed, bufferSize);
		printMetrics(simulationData);
	}
	printf("\n<< QUESTION 6 END >>\n");
}

void Question6b(int packetLength, int serviceSpeed, int period, int bufferSize) {
	struct result simulationData;
	double startUtil = 2.2;
	double endUtil = 5.2;
	printf("<< QUESTION 6 START >>\n");
	for (double Util = startUtil; Util <= endUtil; Util += 0.2)
	{
		simulationData = startSystem(period, Util, packetLength, serviceSpeed, bufferSize);
		printMetrics(simulationData);
	}
	printf("\n<< QUESTION 6 END >>\n");
}

void Question6c(int packetLength, int serviceSpeed, int period, int bufferSize) {
	struct result simulationData;
	double startUtil = 5.4;
	double endUtil = 10.2;
	printf("<< QUESTION 6 START >>\n");
	for (double Util = startUtil; Util <= endUtil; Util += 0.4)
	{
		simulationData = startSystem(period, Util, packetLength, serviceSpeed, bufferSize);
		printMetrics(simulationData);
	}
	printf("\n<< QUESTION 6 END >>\n");
}


int main()
{
	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	Question1(1000, 75);
	Question3(12000, 1000000, 12000);
	Question4(12000, 1000000, 12000);
	Question6a(12000, 1000000, 12000, 5);
	Question6b(12000, 1000000, 12000, 5);
	Question6c(12000, 1000000, 12000, 5);
}
