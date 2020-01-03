//
//  Simulation server 
//  Connects REQ socket to tcp://localhost:5555
//  Sends commands to client
//
#pragma warning(disable:4996)

#include "server_functions.h"

int main()
{
	//  initialize control socket
	zmq::context_t context(1);
	zmq::socket_t control_sender(context, ZMQ_PUB);
	control_sender.bind("tcp://192.168.100.239:5555");

	// initialize task queue
	int task_num = 10;
	std::vector<Task> task_queue;
	for (int i = task_num; i >= 1; --i) {
		task_queue.push_back(Task(i));
	}

	// initialize client queue
	int max_client_num = 2;
	ClientMap clients;
	for (int i = 1; i <= max_client_num; ++i) {
		clients[i] = Client(i);
	}

	// start task, heartbeat and result threads 
	std::thread task_thread(assign_tasks, std::ref(context), 
							std::ref(clients), std::ref(task_queue));
	std::thread result_thread(collect_result, std::ref(context));
	std::thread heartbeat_thread(receive_heartbeat, std::ref(context), 
								 std::ref(clients));

	char command;
	while (true) {
		std::cout << "Please input your command: ";
		std::cin >> command;

		if (command == 's') {
			s_send(control_sender, "start");
		} else if (command == 'c') {
			s_send(control_sender, "continue");
		} else if (command == 'p') {
			s_send(control_sender, "pause");
		} else if (command == 'e') {
			s_send(control_sender, "stop");
			std::cout << "Simulation stop!" << std::endl;
			break;
		} else {
			std::cout << "Wrong command!" << std::endl;
			continue;
		}
	}

	task_thread.join();
	result_thread.join();
	heartbeat_thread.join();

	system("pause");
	return 0;
}