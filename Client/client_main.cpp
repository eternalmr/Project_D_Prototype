#pragma warning(disable:4996)

#include "client_functions.h"

extern int start_flag = 0;
extern int pause_flag = 0;
extern int stop_flag = 0;

int main(int argc, char* argv[])
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_SUB);
	socket.connect("tcp://192.168.100.239:5555");

	SignalSet signal;
	subscribe_specific_signal(socket);

	uint32_t client_id = std::atoi(argv[1]);
	cout << "client_id: " << client_id << endl;

	std::thread simulation_thread(simulation_wrap, std::ref(context));
	std::thread heartbeat_thread(send_heartbeat, std::ref(context), client_id);

	while (true) {
		//impossible situation
		assert(!(start_flag == 0 && pause_flag == 1));

		signal = listen_from_server(socket);

		if (stop_flag) {
			break;
		}

		if (is_irrelevant(signal)) {
			continue;
		}

		switch (signal) {
			case kStart: {
				start_flag = 1;
				cout << "execute start command" << endl;
				break;
			}
			case kContinue: {
				pause_flag = 0;
				cout << "continue simulation" << endl;
				break;
			}
			case kPause: {
				pause_flag = 1;
				cout << "pause simulation" << endl;
				break;
			}
			case kStop: {
				start_flag = 0;
				pause_flag = 0;
				stop_flag = 1;
				cout << "stop simulation" << endl;
				break;
			}
			default: {
				cout << "unknown command" << endl;
			}
		}//end of switch
		if (stop_flag) break;
	}//end of while

	simulation_thread.join();
	heartbeat_thread.join();

	system("pause");
	return 0;
}
