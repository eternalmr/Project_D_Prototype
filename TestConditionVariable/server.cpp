//
//  Simulation server 
//  Connects REQ socket to tcp://localhost:5555
//  Sends commands to client
//
#pragma warning(disable:4996)

#include "zhelpers.hpp"

int main()
{
	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);
	socket.bind("tcp://*:5555");

	// Send test message
	s_send(socket, "Hello");
	std::cout << "Say Hello to client" << std::endl;

	//  Get the reply.
	std::string replys = s_recv(socket);
	std::cout << "Received [" << replys << "] from client" << std::endl;

	char command;
	while (true) {
		std::cout << "Please input your command: ";
		std::cin >> command;

		if (command == 's') {
			s_send(socket, "start");
		} else if (command == 'p') {
			s_send(socket, "pause");
		} else if (command == 'e') {
			s_send(socket, "stop");
			std::cout << "Simulation stop!" << std::endl;
			break;
		} else {
			std::cout << "Wrong command!" << std::endl;
			continue;
		}

		replys = s_recv(socket);
		if (replys == "finished") {
			std::cout << "Simulation finished!" << std::endl;
			break;
		} else if (replys == "interrupt") {
			std::cout << "Simulation interrupted!" << std::endl;
			break;
		} else {
			std::cout << "Received [" << replys << "] from client" << std::endl;
		}
	}

	system("pause");
	return 0;
}