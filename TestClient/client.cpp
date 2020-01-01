#pragma warning(disable:4996)

#include <thread>
#include "zhelpers.hpp"

#define HEARTBEAT_INTERVAL  2000    //  msecs

using std::endl;
using std::cout;

int main(int argc, char* argv[])
{
	zmq::context_t context(1);
	zmq::socket_t heartbeat_sender(context, ZMQ_PUSH);
	heartbeat_sender.connect("tcp://127.0.0.1:1217");

	uint32_t client_id = std::atoi(argv[1]);

	cout << "client_id: " << client_id << endl;

	std::string signal = "HEARTBEAT_" + std::to_string(client_id);
	// send heartbeat at regular interval
	while (true) {
		s_send(heartbeat_sender, signal);
		std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_INTERVAL));
	}

	return 0;
}