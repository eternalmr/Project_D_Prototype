//
//  Simulation server 
//  Connects REQ socket to tcp://localhost:5555
//  Sends commands to client
//
#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <thread>

int GenerateTasks()
{
	zmq::context_t context(1);
	zmq::socket_t requester(context, ZMQ_DEALER);
	requester.connect("tcp://192.168.100.239:5559");

	for (int request = 0; request < 20; request++) {

		s_sendmore(requester, "");
		std::string str = std::to_string(request);
		s_send(requester, str);

		std::string result = s_recv(requester);

		std::cout << "Result of request[" << request
			<< "]: " << result << std::endl;

		if (atoi(result.c_str()) == -1) {
			break;
		}
	}

	std::cout << "Simulation finished!" << std::endl;
	return 0;
}

int AssignTasks()
{
	//  Prepare our context and sockets
	zmq::context_t context(1);
	zmq::socket_t frontend(context, ZMQ_ROUTER);
	zmq::socket_t backend(context, ZMQ_DEALER);

	frontend.bind("tcp://192.168.100.239:5559");
	backend.bind("tcp://192.168.100.239:5560");

	zmq::proxy(frontend, backend, NULL);

	frontend.close();
	backend.close();
	context.close();

	return 0;
}

int main()
{
	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_PUB);
	socket.bind("tcp://192.168.100.239:5555");

	//// Send test message
	//s_send(socket, "Hello");
	//std::cout << "Say Hello to client" << std::endl;

	////  Get the reply.
	//std::string replys = s_recv(socket);
	//std::cout << "Received [" << replys << "] from client" << std::endl;

	// 启动任务线程和代理线程
	std::thread task_thread(GenerateTasks);
	std::thread proxy_thread(AssignTasks);

	char command;
	std::string replys;
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

		//replys = s_recv(socket);
		//if (replys == "finished") {
		//	std::cout << "Simulation finished!" << std::endl;
		//	break;
		//} else if (replys == "interrupt") {
		//	std::cout << "Simulation interrupted!" << std::endl;
		//	break;
		//} else {
		//	std::cout << "Received [" << replys << "] from client" << std::endl;
		//}
	}

	system("pause");
	return 0;
}