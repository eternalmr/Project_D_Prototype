#pragma warning(disable:4996)

#include <thread>
#include <regex>
#include <map>
#include "zhelpers.hpp"

const int HEARTBEAT_INTERVAL = 2000;      //  millisecond
const int HEARTBEAT_TIMEOUT  = 300000;    //  millisecond

using std::endl;
using std::cout;
using std::string;

//用delim指定的正则表达式将字符串in分割，返回分割后的字符串数组
//delim 分割字符串的正则表达式
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

class Client
{
public:
	enum NodeStatus { kFree = 0, kInComputing, kBreakdown };

public:
	Client() : node_id_(0), node_status_(kFree) {}
	explicit Client(unsigned int id) : node_id_(id), node_status_(kFree)
	{
		heartbeat_ = s_clock();
		expiry_ = heartbeat_ + 300000; // heartbeat moment + 5 mins
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
	uint32_t node_id_;
	NodeStatus node_status_;
	Task task_;
	int64_t heartbeat_;
	int64_t expiry_;
};

void update_client_heartbeat(Client &a_client)
{
	int64_t this_moment = s_clock();
	a_client.set_heartbeat(this_moment);
	a_client.set_expiry(this_moment + HEARTBEAT_TIMEOUT);
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

int main()
{
	int id;
	string heartbeat_signal;
	typedef std::map<uint32_t, Client> ClientMap;

	zmq::context_t context(1);
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

	return 0;
}