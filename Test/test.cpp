//   Request-reply client in C++
//   Connects REQ socket to tcp://localhost:5559
//   Sends "Hello" to server, expects "World" back
//
// Olivier Chamoux <olivier.chamoux@fr.thalesgroup.com>

#pragma warning(disable: 4996)

#include <zhelpers.hpp>
#include <thread>

int task_thread()
{
	zmq::context_t context(1);

	zmq::socket_t requester(context, ZMQ_REQ);
	requester.connect("tcp://localhost:5559");

	for (int request = 0; request < 100; request++) {

		std::string str = "Hello " + std::to_string(request);
		s_send(requester, str);
		std::string string = s_recv(requester);

		std::cout << "Received reply " << request
			<< " [" << string << "]" << std::endl;
	}

	std::cout << "Simulation finished!" << std::endl;
	return 0;
}

int broker_thread()
{
	//  Prepare our context and sockets
	zmq::context_t context(1);
	zmq::socket_t frontend(context, ZMQ_ROUTER);
	zmq::socket_t backend(context, ZMQ_DEALER);

	frontend.bind("tcp://*:5559");
	backend.bind("tcp://*:5560");

	zmq::proxy(frontend, backend, NULL);

	frontend.close();
	backend.close();
	context.close();

	return 0;
}

int main()
{
	std::thread client(task_thread);
	std::thread broker(broker_thread);

	client.join();
	broker.join();

	return 0;
}

#pragma warning(disable:4996)

#include <zhelpers.hpp>

int main(int argc, char *argv[])
{
	zmq::context_t context(1);

	zmq::socket_t responder(context, ZMQ_REP);
	responder.connect("tcp://localhost:5560");

	while (1)
	{
		//  Wait for next request from client
		std::string string = s_recv(responder);

		std::cout << "Received request: " << string << std::endl;

		// Do some 'work'
		Sleep(1000);

		// Send reply back to client
		s_send(responder, "World");

	}
}