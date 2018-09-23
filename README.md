# Network Buffer Simulation

A tool built for the simulation of a network buffer for packet ingestion written in C++

The performance of a communication network is a key aspect of network engineering. Performance measures like delay (how long a packet stays in a system or takes to be delivered), loss ratio (the percentage of the packets that are lost in the system),
etc. are some of the criteria that network engineers are trying to predict or measure since they affect the Quality of Service (QoS) offered to the users.

The aim of this project is to simulate the behaviour of a network buffer that ingests events. The system is made of 2 components, a buffer and one (or many) server(s).

## Instructions

Simply run 'make' in the designated folder and the binary 'output.exe' will be produced. Run this ./output.exe file to start the simulation.
