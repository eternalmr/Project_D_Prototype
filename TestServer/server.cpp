#pragma warning(disable:4996)

#include <thread>
#include <regex>
#include <map>
#include "zhelpers.hpp"

#define HEARTBEAT_INTERVAL  2000    //  msecs

using std::endl;
using std::cout;

/*
   用delim指定的正则表达式将字符串in分割，返回分割后的字符串数组
   delim 分割字符串的正则表达式
*/
std::vector<std::string> split_string(const std::string& in,
									  const std::string& delim)
{
	std::regex re{ delim };

	return std::vector<std::string> {
		std::sregex_token_iterator(in.begin(), in.end(), re, -1),
			std::sregex_token_iterator()
	};
}

std::tuple<int, std::string> decode_signal(std::string &raw_signal)
{
	auto signal = split_string(raw_signal, "_"); // TODO : use std::tuple to seperate signals
	return std::make_tuple(std::stoi(signal[1]), signal[0]);
}

bool node_is_expiry() // TODO
{
	return false;
}

class Task
{
private:
	enum ComputeStatus { kNotStart = 0, kInComputing, kFinished };
	enum   StoreStatus { kNotSave = 0, kSaved };

public:
	Task()
	{
		set_id(0);
		set_compute_status(kNotStart);
		set_store_status(kNotSave);
	}

	explicit Task(unsigned int id)
	{
		set_id(id);
		set_compute_status(kNotStart);
		set_store_status(kNotSave);
	}

	int get_id() const { return id_; }


	ComputeStatus get_compute_status() const { return compute_status_; }
	StoreStatus get_store_status() const { return store_status_; }

	void set_compute_status(ComputeStatus status) { compute_status_ = status; }
	void set_store_status(StoreStatus status) { store_status_ = status; }

private:
	void set_id(unsigned int id) { id_ = id; }

private:
	unsigned int id_;
	ComputeStatus compute_status_;
	StoreStatus store_status_;
};

class ComputeNode
{
public:
	enum NodeStatus { kFree = 0, kInComputing, kBreakdown };

public:
	ComputeNode() : node_id_(0), node_status_(kFree) {}
	explicit ComputeNode(unsigned int id) : node_id_(id), node_status_(kFree)
	{
		heartbeat_ = s_clock();
		expiry_ = heartbeat_ + 5 * 60 * 1000; // heartbeat moment + 5 mins
	}

	unsigned int get_node_id() const { return node_id_; }
	NodeStatus get_node_status() const { return node_status_; }
	Task get_task() const { return task_; }
	int64_t get_heartbeat() const { return heartbeat_; }
	int64_t get_expiry() const { return expiry_; }

	void set_node_id(unsigned int id) { node_id_ = id; }
	void set_node_status(NodeStatus status) { node_status_ = status; }
	void set_task(Task task) { task_ = task; }
	void set_heartbeat(int64_t heartbeat) { heartbeat_ = heartbeat; }
	void set_expiry(int64_t expiry) { expiry_ = expiry; }

private:
	unsigned int node_id_;
	NodeStatus node_status_;
	Task task_;
	int64_t heartbeat_;
	int64_t expiry_;
};

int main()
{
	zmq::context_t context(1);
	zmq::socket_t heartbeat_receiver(context, ZMQ_PULL);
	heartbeat_receiver.bind("tcp://127.0.0.1:1217");

	//create a client vector or something
	const int node_num = 5;
	std::map<uint32_t, ComputeNode> clients;

	// TODO : how to dynamically add client to client pool
	for (int i = 1; i <= node_num; ++i) {
		clients[i] = ComputeNode(i);
	}

	int id;
	int count = 0;
	int64_t this_moment;
	const int64_t five_minutes_in_milliseconds = 300000;
	std::string heartbeat_signal;
	ComputeNode a_client;

	while (true) {
		auto raw_signal = s_recv(heartbeat_receiver);

		// TODO : if not a valid signal, then continue

		std::tie(id, heartbeat_signal) = decode_signal(raw_signal);

		// find client correspond to the signal
		a_client = clients[id];

		if (heartbeat_signal == "HEARTBEAT") {
			this_moment = s_clock();
			a_client.set_heartbeat(this_moment);
			a_client.set_expiry(this_moment + five_minutes_in_milliseconds);
			std::cout << "Received heartbeat from node " << id << ": " << count << std::endl;
			std::cout << "Heartbeat: " << a_client.get_heartbeat() << std::endl;
			std::cout << "Expiry:    " << a_client.get_expiry() << std::endl;
			count++;
		}

		// 遍历所有clients，判断是否有超时的client，将其从队列中剔除
		std::map<uint32_t, ComputeNode>::iterator iter;
		for (iter = clients.begin(); iter != clients.end(); iter++) {
			if (node_is_expiry()) {
				iter->second.set_node_status(ComputeNode::kBreakdown);
			}
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	return 0;
}