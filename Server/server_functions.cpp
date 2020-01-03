#pragma warning(disable:4996)

#include "server_functions.h"

int assign_tasks(zmq::context_t &context)
{
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
		cout << "Receive request: " << reply << endl;
		s_send(server, std::to_string(workload));
		workload++;
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

bool node_is_expiry() // TODO
{
	return false;
}

void update_client_heartbeat(Client &a_client)
{
	int64_t this_moment = s_clock();
	a_client.set_heartbeat(this_moment);
	a_client.set_expiry(this_moment + HEARTBEAT_TIMEOUT);
	cout << "Heartbeat of node[" << a_client.get_node_id() << "] : " 
		 << a_client.get_heartbeat() << endl;
}

void delete_breakdown_client(std::map<uint32_t, Client> &clients)
{
	std::map<uint32_t, Client>::iterator iter;
	for (iter = clients.begin(); iter != clients.end(); iter++) {
		if (node_is_expiry()) {
			iter->second.set_node_status(Client::kBreakdown);
		}
	}
}

void receive_heartbeat(zmq::context_t &context)
{
	int id;
	string heartbeat_signal;
	typedef std::map<uint32_t, Client> ClientMap;

	zmq::socket_t heartbeat_receiver(context, ZMQ_PULL);
	heartbeat_receiver.bind("tcp://127.0.0.1:1217");

	//create a client vector or something
	const int node_num = 5;
	ClientMap clients;

	// TODO : how to dynamically add client to client pool
	for (int i = 1; i <= node_num; ++i) {
		clients[i] = Client(i);
	}

	while (true) {
		auto raw_signal = s_recv(heartbeat_receiver);

		// TODO : if not a valid signal, then continue

		std::tie(id, heartbeat_signal) = decode_signal(raw_signal);

		if (heartbeat_signal != "HEARTBEAT") {
			cout << "unknown signal" << endl;
			continue;
		}


		update_client_heartbeat(clients[id]);

		// 遍历所有clients，判断是否有超时的client，将其从队列中剔除
		delete_breakdown_client(clients);
	}
}