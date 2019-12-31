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

void send_heartbeat(zmq::context_t &context)
{
	zmq::socket_t heartbeat_sender(context, ZMQ_PUSH);
	heartbeat_sender.connect("tcp://127.0.0.1:1217");

	// send heartbeat at regular interval
	while (true) {
		s_send(heartbeat_sender, "HEARTBEAT_1");
		std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_INTERVAL));
	}
}

int main()
{
	zmq::context_t context(1);
	zmq::socket_t test_socket(context, ZMQ_PULL);

	std::thread heartbeat_thread(send_heartbeat, std::ref(context));

	test_socket.bind("tcp://127.0.0.1:1217");
	//should receive 10 heartbeat in specific time interval
	int64_t test_end = s_clock() + 10000;
	int64_t current_time = s_clock();

	int count = 0;
	while (current_time < test_end) {
		auto raw_signal = s_recv(test_socket);
		auto signal = s_split(raw_signal, "_");
		if (signal[0] == "HEARTBEAT") {
			std::cout << "Received heartbeat from node " << signal[1] << ": "<< count << std::endl;
			count++;
		}
		current_time = s_clock();
	}

	heartbeat_thread.join();

	return 0;
}