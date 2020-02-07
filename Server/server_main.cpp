//
//  Simulation server 
//  Connects REQ socket to tcp://localhost:5555
//  Sends commands to client
//
#pragma warning(disable:4996)

#include "server_functions.h"

int main()
{
	// initialize control socket
	zmq::context_t context(1);
	zmq::socket_t control_sender(context, ZMQ_PUB);
	control_sender.bind("tcp://*:5555");

	// initialize task queue
	int task_num = 5;
	std::vector<Task> task_queue;
	for (int i = 0; i < task_num; ++i) {
		task_queue.push_back(Task(i+1));
	}

	// initialize client queue
	int max_client_num = 2;
	ClientMap clients;
	for (int i = 0; i < max_client_num; ++i) {
		clients.push_back(Client(i + 1));
	}

	// start task, heartbeat and result threads 
	std::thread heartbeat_thread(receive_heartbeat, std::ref(context),
													std::ref(clients));
	std::thread    result_thread(collect_result,	std::ref(context));
	std::thread      task_thread(assign_tasks,		std::ref(context), 
								 std::ref(clients), std::ref(task_queue));

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