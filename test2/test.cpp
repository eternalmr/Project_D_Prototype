//
//  Multithreaded relay in C++
//
// Olivier Chamoux <olivier.chamoux@fr.thalesgroup.com>

#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <pthread.h>
//#include <iostream>

//  Step 1 pushes one message to step 2

void *step1(void *arg) {

	zmq::context_t * context = static_cast<zmq::context_t*>(arg);

	//  Signal downstream to step 2
	zmq::socket_t sender(*context, ZMQ_PAIR);
	sender.connect("inproc://step2");

	std::cout << "step1: send signal from step1" << std::endl;
	s_send(sender, "step1");

	return (NULL);
}

//  Step 2 relays the signal to step 3

void *step2(void *arg){

	zmq::context_t * context = static_cast<zmq::context_t*>(arg);

	//  Bind to inproc: endpoint, then start upstream thread
	zmq::socket_t receiver(*context, ZMQ_PAIR);
	receiver.bind("inproc://step2");

	pthread_t thread;
	pthread_create(&thread, NULL, step1, context);

	//  Wait for signal
	std::string str = s_recv(receiver);
	std::cout << "step2: receive signal "<< str <<" from step1" << std::endl;

	//  Signal downstream to step 3
	zmq::socket_t sender(*context, ZMQ_PAIR);
	sender.connect("inproc://step3");
	std::cout << "step2: send signal from step2" << std::endl;
	s_send(sender, "step2");

	return (NULL);
}

//  Main program starts steps 1 and 2 and acts as step 3

int main() {

	zmq::context_t context(1);

	//  Bind to inproc: endpoint, then start upstream thread
	zmq::socket_t receiver(context, ZMQ_PAIR);
	receiver.bind("inproc://step3");

	pthread_t thread;
	pthread_create(&thread, NULL, step2, &context);

	//  Wait for signal
	std::string str = s_recv(receiver);
	std::cout << "step3: receive signal " << str << " from step2" << std::endl;

	std::cout << "Test successful!" << std::endl;

	system("pause");
	return 0;
}