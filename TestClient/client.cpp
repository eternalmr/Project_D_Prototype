#pragma warning(disable:4996)

#include <thread>
#include "zhelpers.hpp"
#include <regex>
#include <vector>
#include <iterator>

#define HEARTBEAT_LIVENESS  3       //  3-5 is reasonable
#define HEARTBEAT_INTERVAL  2000    //  msecs
#define INTERVAL_INIT       1000    //  Initial reconnect
#define INTERVAL_MAX       32000    //  After exponential back off

using std::endl;
using std::cout;

/*
   用delim指定的正则表达式将字符串in分割，返回分割后的字符串数组
   delim 分割字符串的正则表达式
 */
std::vector<std::string> s_split(const std::string& in, 
								 const std::string& delim) 
{
	std::regex re{ delim };

	return std::vector<std::string> {
				std::sregex_token_iterator(in.begin(), in.end(), re, -1),
				std::sregex_token_iterator()
	};
}

void send_heartbeat(zmq::context_t &context, unsigned int client_id)
{
	zmq::socket_t heartbeat_sender(context, ZMQ_PUSH);
	heartbeat_sender.connect("tcp://127.0.0.1:1217");

	std::string signal = "HEARTBEAT_" + std::to_string(client_id);
	// send heartbeat at regular interval
	while (true) {
		s_send(heartbeat_sender, signal);
		std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_INTERVAL));
	}
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
private:
	enum NodeStatus { kFree = 0, kInComputing, kBreakdown };

public:
	ComputeNode() : node_id_(0), node_status_(kFree) {}
	explicit ComputeNode(unsigned int id) : node_id_(id), node_status_(kFree) {}

	unsigned int get_node_id() const { return node_id_; }
	NodeStatus get_node_status() const { return node_status_; }
	Task get_task() const { return task_; }

	void set_node_id(unsigned int id) { node_id_ = id; }
	void set_node_status(NodeStatus status) { node_status_ = status; }
	void set_task(Task task) { task_ = task; }

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
	zmq::socket_t recv_heartbeat(context, ZMQ_PULL);

	std::thread heartbeat_thread(send_heartbeat, std::ref(context), 1);

	recv_heartbeat.bind("tcp://127.0.0.1:1217");

	//create a client vector or something
	std::vector<ComputeNode> clients;

	int count = 0;
	while ( true ) {
		auto raw_signal = s_recv(recv_heartbeat);
		auto signal = s_split(raw_signal, "_");
		if (signal[0] == "HEARTBEAT") {
			std::cout << "Received heartbeat from node " << signal[1] << ": "<< count << std::endl;
			count++;
		}
	}

	heartbeat_thread.join();

	return 0;
}