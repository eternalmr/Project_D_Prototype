//
//  Simulation server 
//  Connects REQ socket to tcp://localhost:5555
//  Sends commands to client
//
#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <thread>


int assign_tasks() {
	zmq::context_t context(1);
	zmq::socket_t server(context, ZMQ_REP);
	server.bind("tcp://192.168.100.239:5560");

	// TODO : create task pool

	// TODO : create worker queue

	int workload = 0;
	while (true) { // while tasks not finished
		// TODO : a_free_worker = get_free_worker(workers queue)

		// TODO : a_undo_task = get_undo_task(tasks pool)

		// TODO : assign_task_to(a_free_worker, a_undo_task)

		std::string reply = s_recv(server);
		std::cout << "Receive request: " << reply << std::endl;
		s_send(server, std::to_string(workload));
		workload++;
	}

	return 0;
}

int collect_result()
{
	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t collector(context, ZMQ_PULL);
	collector.bind("tcp://*:5558");

	// Collect result from workers
	std::string result;
	while (true) {
		result = s_recv(collector);
		std::cout << result << std::endl;
	}

	return 0;
}


int main()
{
	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_PUB);
	socket.bind("tcp://192.168.100.239:5555");

	// 启动任务线程
	std::thread task_thread(assign_tasks);
	std::thread result_thread(collect_result);
	// TODO : start heartbeat thread

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