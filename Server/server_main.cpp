//
//  Simulation server 
//  Connects REQ socket to tcp://localhost:5555
//  Sends commands to client
//
#pragma warning(disable:4996)

#include "server_functions.h"

int main()
{
	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_PUB);
	socket.bind("tcp://192.168.100.239:5555");

	// Start threads 
	std::thread task_thread(assign_tasks, std::ref(context));
	std::thread result_thread(collect_result, std::ref(context));
	std::thread heartbeat_thread(receive_heartbeat, std::ref(context));

	char command;
	while (true) {
		std::cout << "Please input your command: ";
		std::cin >> command;

		if (command == 's') {
			s_send(socket, "start");
		} else if (command == 'c') {
			s_send(socket, "continue");
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
	}

	task_thread.join();
	result_thread.join();

	system("pause");
	return 0;
}