//  Least-recently used (LRU) queue device
//  Clients and workers are shown here in-process
//
// Olivier Chamoux <olivier.chamoux@fr.thalesgroup.com>

#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <thread>

//  Basic request-reply client using REQ socket
//
int dealer_thread(int id) {
	zmq::context_t context(1);
	zmq::socket_t aDealer(context, ZMQ_DEALER);

#if (defined (WIN32))
	s_set_id(aDealer, (intptr_t)id);
	aDealer.connect("tcp://localhost:5672");
#else
	s_set_id(client); // Set a printable identity
	client.connect("ipc://frontend.ipc");
#endif

	//  Send request, get reply
	s_send(aDealer, "HELLO");
	std::string reply = s_recv(client);
	std::cout << "Client: " << reply << std::endl;
	return 0;
}

//  Worker using REQ socket to do LRU routing
//
int router_thread(void *arg) {
	zmq::context_t context(1);
	zmq::socket_t aRouter(context, ZMQ_DEALER);

#if (defined (WIN32))
	aRouter.bind("tcp://localhost:5672");
#else
	s_set_id(client); // Set a printable identity
	client.connect("ipc://frontend.ipc");
#endif

	//  Send request, get reply
	s_send(aRouter, "HELLO");
	std::string reply = s_recv(client);
	std::cout << "Client: " << reply << std::endl;
	return 0;
}

int main(int argc, char *argv[])
{
	
	return 0;
}