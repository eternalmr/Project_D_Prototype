#pragma warning(disable:4996)

#include "client_functions.h"

extern int start_flag = 0;
extern int pause_flag = 0;
extern int stop_flag = 0;

int main(int argc, char* argv[])
{
	SignalSet control_signal;

	// get client id from argv
	uint32_t client_id = std::atoi(argv[1]);
	cout << "client_id: " << client_id << endl;

	// initialize client socket
	zmq::context_t context(1);
	zmq::socket_t client_socket(context, ZMQ_SUB);
	client_socket.connect("tcp://localhost:5555");
	subscribe_specific_signal(client_socket);

	// start simulation and heartbeat thread
	std::thread simulation_thread(simulation_wrap, std::ref(context), client_id);
	std::thread  heartbeat_thread(send_heartbeat,  std::ref(context), client_id);

	// enter control loop 
	while (true) {
		//impossible situation
		assert(!(start_flag == 0 && pause_flag == 1));

		control_signal = listen_from_server(client_socket);
		if (is_irrelevant(control_signal)) continue;

		execute_control_command(control_signal);

		if (stop_flag) break;
	}//end of while

	simulation_thread.join();
	heartbeat_thread.join();

	system("pause");
	return 0;
}


