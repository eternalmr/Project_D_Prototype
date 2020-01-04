#pragma warning(disable:4996)

#include "server_functions.h"
#include <algorithm>

bool task_is_not_start(Task &task)
{
	return task.is_not_start();
}

std::vector<Task>::iterator get_undo_task(std::vector<Task> &task_queue)
{
	return std::find_if(task_queue.begin(), task_queue.end(), task_is_not_start);
}

int assign_tasks(zmq::context_t &context, 
				 ClientMap &clients, std::vector<Task> &task_queue)
{
	zmq::socket_t task_assigner(context, ZMQ_REP);
	task_assigner.bind("tcp://*:5560");

	int workload = 0;
	Task a_task;
	while (task_queue.size() != 0) {
		// TODO : a_undo_task = get_undo_task(tasks pool)
		a_task = task_queue.back();
		task_queue.pop_back();

		workload = a_task.get_id();
		//a_task.set_compute_status(Task::kInComputing);

		// TODO : a_free_worker = get_free_worker(workers queue)

		// TODO : assign_task_to(a_free_worker, a_undo_task)

		std::string reply = s_recv(task_assigner);
		cout << "Receive request: " << reply << endl;

		s_send(task_assigner, std::to_string(workload));
	}

	return 0;
}

int collect_result(zmq::context_t &context)
{
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

std::vector<string> split_string(const string& in, const string& delim)
{
	std::regex re{ delim };
	return std::vector<string> {
		std::sregex_token_iterator(in.begin(), in.end(), re, -1),
			std::sregex_token_iterator()
	};
}

std::tuple<int, string> decode_signal(string &raw_signal)
{
	auto signal = split_string(raw_signal, "_");
	return std::make_tuple(std::stoi(signal[1]), signal[0]);//output is [client_id, signal]
}

void update_client_heartbeat(Client &a_client)
{
	int64_t this_moment = s_clock();
	a_client.set_heartbeat(this_moment);
	//a_client.set_expiry(this_moment + HEARTBEAT_TIMEOUT);
	cout << "Heartbeat of node[" << a_client.get_node_id() << "] : " 
		 << a_client.get_heartbeat() << endl;
}

void mark_breakdown_client(ClientMap &clients)
{
	ClientMap::iterator iter;
	
	for (iter = clients.begin(); iter != clients.end(); iter++) {
		Client &a_client = iter->second;
		if (a_client.is_expiry() && !a_client.is_breakdown()) {//TODO : 有问题
			a_client.set_breakdown();
			cout << "client[" << a_client.get_node_id() << "] is breakdown!" << endl;
		}
	}
}

void receive_heartbeat(zmq::context_t &context, ClientMap &clients)
{
	int id;
	string heartbeat_signal;

	zmq::socket_t heartbeat_receiver(context, ZMQ_PULL);
	heartbeat_receiver.bind("tcp://*:1217");

	while (true) {
		auto raw_signal = s_recv(heartbeat_receiver);

		// TODO : if not a valid signal, then continue

		std::tie(id, heartbeat_signal) = decode_signal(raw_signal);

		if (heartbeat_signal != "HEARTBEAT") {
			cout << "unknown signal" << endl;
			continue;
		}

		update_client_heartbeat(clients[id]);

		// go through all clients, mark breakdown client
		mark_breakdown_client(clients);
	}
}