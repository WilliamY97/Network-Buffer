#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <queue>
#include <vector>
#include <cmath>
#include "random.h"
using namespace std;

int numRepeat = 1;
float kSecondsPerIteration = 0.001;

struct simulationData
{
    float currentEN = NULL; 
    float currentET = NULL;
    float currentPIdle = NULL; 
    float currentPLoss = NULL;
};

struct Packet {
    Packet *next_packet = NULL;

    public:
        int packet_len; 
        float creation_time;
        bool serviced = false;
        float serviced_time = -1;

        void set_next_packet(struct Packet* set_next_packet) { 
            this->next_packet = next_packet; 
        }

        void complete_service(float time) {         
            this->serviced_time = time;
            this->serviced = true; 
        }

        bool operator<(const Packet& rhs) const {
        	return this -> creation_time > rhs.creation_time;
        }
        
        float duration() {
            if (serviced == false || serviced_time == -1 || creation_time == -1 ) {
                 return -1;
            }
            return this->serviced_time - this->creation_time;
        }
};

struct Packet * init_packet(float creation_time, float departure_time, int packet_len) {
    struct Packet *packet_ptr = (Packet *)malloc(sizeof(Packet));
    packet_ptr->creation_time = creation_time;
    packet_ptr->packet_len = packet_len;
    return packet_ptr;    
}

float calculate_lambda(float utilization, float service_speed, int packet_len) {
    return (float)( (utilization*(service_speed*pow(10, 6))) / packet_len );
}

class Buffer {
	public:
	    Buffer(){};
	    Buffer(int packet_limit) { 
	        this->is_limit = true;
	        this->packet_limit = packet_limit;
    	};

	    std::priority_queue<Packet *> Q;

	    struct Packet* pop_packet(){
		    if (!Q.empty()) {
		        this->packet_counter--;
		        Packet * t = Q.top();
		        Q.pop();
		        return t;
		    }
		    else {
		        return NULL;
		    }
	    };
	    void add_packet(struct Packet* new_packet) {
	    	Q.push(new_packet);
	    	this->packet_counter++;
	    }

		int packet_counter = 0;
    	int packet_limit;
		bool is_limit = false;
};

class EventGenerator {

    int packet_len = 2000;
    int packetPerSecond;
    struct Packet *new_packet = NULL;
    float secondsPerIteration = kSecondsPerIteration;
    int currentIteration = 0;
    int requiredIterations = 0;

    public:
        EventGenerator(int packetPerSecond, int packet_len) { 
            this->packetPerSecond = packetPerSecond;
            this->packet_len = packet_len;
            this->requiredIterations = int( expf(packetPerSecond) / this->secondsPerIteration);
        };

        struct Packet * get_new_packet() {
        	// checks for the packet and then returns it
            if (this->new_packet != NULL) {
                return this->new_packet;
            }
            else {
                return NULL;
            }
        };

        int recordIteration(float current_time, float lambda) {
            this->currentIteration++;
            if (this->currentIteration >= this->requiredIterations) {
	            this->currentIteration = 0; 
	            this->requiredIterations = int( expf(lambda) / this->secondsPerIteration );     
                this->new_packet = init_packet(current_time, 0, packet_len);
                return 1;
            }
            else {
                this->new_packet = NULL;
                return 0;
            }  
        };
};


class Server {
    float serviceSpeed = 0;
    float secondsPerIteration = kSecondsPerIteration;
    float packetPerSecond = 0;

    int currentIteration = 0;
    int requiredIterations = 0;

    struct Packet *packet_ptr = NULL;

    public:
        bool server_busy = false;

        Server(float packetPerSecond, float serviceSpeed) {
            this->packetPerSecond = packetPerSecond;
            this->serviceSpeed = serviceSpeed;
        };

        void receive_new_packet(struct Packet *packet_ptr, float time) {
            if (packet_ptr != NULL) {
                float iteration_duration = float((packet_ptr->packet_len) / (serviceSpeed*pow(10, 6)));
                this->requiredIterations = int(iteration_duration / secondsPerIteration);
                this->currentIteration = 0;
                this->server_busy = true;
                this->packet_ptr = packet_ptr;
            }
        };

        //Returns packet if the current packet has been served
        struct Packet * recordIteration(float time) {
            this->currentIteration++;

            if (currentIteration >= requiredIterations && this->packet_ptr != NULL) {
                this->server_busy = false;
                this->packet_ptr->complete_service(time);
                return this->packet_ptr;
            }
            else {
                return NULL;
            }
        };
};

int add_packet_to_buffer(struct Packet* new_packet, Buffer* buffer) {
	// adds the given packet to the buffer
    if (new_packet != NULL) {
        if (buffer->is_limit == false) {
            buffer->add_packet(new_packet);
            return 1;
        }
        else {
        	// if buffer is full then it fails
            if (buffer->packet_counter == buffer->packet_limit) {
                return 0;
            }
            else {
            	// otherwise success
                buffer->add_packet(new_packet);
                return 1;                
            }
        }
    }
    return 0;
}

simulationData run_simulation(int iterations, float utilization, int packet_len, float serviceSpeed, int buffer_size) {
    
    Buffer* buffer;

    if (buffer_size == -1) {
        buffer = new Buffer();
    }
    else {
    	buffer = new Buffer(buffer_size);
    }

    int num_serviced = 0;
    float totalPackets = 0.0;
    int totalCreatedPackets = 0;
    float totalServiceTime = 0.0;
    int totalIdleIterations = 0;
    int totalDroppedPackets = 0;
    int currIter = 0;
    int lambda = calculate_lambda(utilization, serviceSpeed, packet_len);
    
    EventGenerator* pack_gen = new EventGenerator(lambda, packet_len);
    Server* pack_serv = new Server(lambda, serviceSpeed);

    while (currIter <= iterations) {
    	currIter += 1;
        float current_time =  float(currIter * kSecondsPerIteration);
        if ( pack_gen->recordIteration(current_time, lambda) == 1 ) {
        	// when record iteration is successful aka a packet is made then
            totalCreatedPackets++;
            // when buffer can't fit anymore packets
            if ( add_packet_to_buffer(pack_gen->get_new_packet(), buffer) != 1 ) {
                totalDroppedPackets++;
            }
        }
        // if server is busy
        if ( pack_serv->server_busy == true ) {
            struct Packet *serviced_packet = pack_serv->recordIteration(current_time);
            if (serviced_packet != NULL) {
                num_serviced++;
                totalServiceTime += serviced_packet->duration();
                free(buffer->pop_packet());
            }
        }
        else {
        	// if server is not busy

        	// if buffer has no packets to send over
            if (buffer->packet_counter == 0) {
                totalIdleIterations++;
            }
            else {
            	// if buffer has packets to send over we send the packet
                struct Packet *packet = buffer->Q.top();
                // server receives it making it busy now
                pack_serv->receive_new_packet(packet, current_time);
                // server tries to record the iteration (this is a departure)
                struct Packet *serviced_packet = pack_serv->recordIteration(current_time);
                if ( serviced_packet != NULL ) {
                    num_serviced += 1;
                    totalServiceTime += serviced_packet->duration();
                    free(buffer->pop_packet());
                }
            }      
        }
        totalPackets += buffer->packet_counter; 
    }
    simulationData result;
    result.currentEN = float(totalPackets) / iterations;
    result.currentET = float(totalServiceTime) / num_serviced;
    result.currentPIdle = float(totalIdleIterations) / iterations;
    result.currentPLoss = float(totalDroppedPackets) / totalCreatedPackets;
    free(buffer);
    return result;
}

void evaluateMetrics(simulationData result) {
    float EN = 0;
    float ET = 0;
    float totalPIdle = 0;
    float totalPLoss = 0;

    for (int i=1; i<=numRepeat; i++) {
        EN += result.currentEN;
        ET += result.currentET;
        totalPIdle += result.currentPIdle;
        totalPLoss += result.currentPLoss;
    }
    
    result.currentEN = float(EN / numRepeat);
    result.currentET  = float(ET / numRepeat);
    result.currentPIdle  = float(totalPIdle / numRepeat);
    result.currentPLoss = float(totalPLoss / numRepeat);
}

void outputMetrics(simulationData result) {
    printf(" E[N]: %f% \n E[T]: %f% \
    \n PercentLoss: %f \n PercentIdle: %f \n", \
    result.currentEN, result.currentET, result.currentPLoss, result.currentPIdle);
}

int main()
{
	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	Question1(1000, 75);
    	simulationData result = run_simulation(1000000, 0.25, 12000, 1, -1);
	evaluateMetrics(result);
	outputMetrics(result);
}
