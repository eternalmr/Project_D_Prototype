#pragma warning(disable:4996)

#include "client_functions.h"

void subscribe_specific_signal(zmq::socket_t &socket)
{
	const char *start_filter = "start";
	const char *continue_filter = "continue";
	const char *pause_filter = "pause";
	const char *stop_filter = "stop";
	socket.setsockopt(ZMQ_SUBSCRIBE, start_filter, strlen(start_filter));
	socket.setsockopt(ZMQ_SUBSCRIBE, continue_filter, strlen(continue_filter));
	socket.setsockopt(ZMQ_SUBSCRIBE, pause_filter, strlen(pause_filter));
	socket.setsockopt(ZMQ_SUBSCRIBE, stop_filter, strlen(stop_filter));
}

void send_heartbeat(zmq::context_t &context, uint32_t client_id)
{
	zmq::socket_t heartbeat_sender(context, ZMQ_PUSH);
	heartbeat_sender.connect("tcp://localhost:1217");

	std::string signal = "HEARTBEAT_" + std::to_string(client_id);
	// send heartbeat at regular interval
	while (true) {
		s_send(heartbeat_sender, signal);
		std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_INTERVAL));
	}

	heartbeat_sender.close();
}

int simulation_wrap(zmq::context_t &context, uint32_t client_id)
{
	int task_input;
	int result;

	// initialize task requester and result sender
	zmq::socket_t task_requester(context, ZMQ_REQ);
	task_requester.connect("tcp://localhost:5560");
	zmq::socket_t result_sender(context, ZMQ_PUSH);
	result_sender.connect("tcp://localhost:5558");

	while (simulation_is_not_finished()) {
		// Send ready to server
		// TODO : send "client_id_ready" to server
		s_send(task_requester, std::to_string(client_id));

		// Receive a task from server
		std::string new_task = s_recv(task_requester);
		std::cout << "**********************************************" << std::endl;
		std::cout << "Receive a new task: " << new_task << std::endl;

		// Do some work
		stop_flag = 0; //reset stop flag
		task_input = atoi(new_task.c_str());
		result = simulation(task_input);

		if (result == -1) {
			std::cout << "Simulation interrupt" << std::endl;
			break;
		}

		//  Send results to sink
		std::string result_info = "result of task[" + new_task
			+ "] is: " + std::to_string(result);
		s_send(result_sender, result_info);

		std::cout << "**********************************************" << std::endl;
	}//end of while

	task_requester.close();
	return 0;
}

int simulation(int input)
{
	int result = input;

	while (!start_flag) {
		std::this_thread::yield();
	}

	while (true) {
		if (stop_flag) return -1;//interrupt simulation

		if (start_flag == 1 && pause_flag == 1) {
			std::this_thread::yield();
			continue;
		}

		result++;
		Sleep(1000);
		cout << "result: " << result << endl;

		if (has_reached_endpoint(input, result)) {
			stop_flag = 1;
			cout << "Task finished!" << endl;
			break;
		}
	}

	return result;
}

bool simulation_is_not_finished()
{
	return true;
}

SignalSet listen_from_server(zmq::socket_t &socket)
{
	std::string command = s_recv(socket);

	if (command == "start")
		return kStart;
	if (command == "pause")
		return kPause;
	if (command == "stop")
		return kStop;
	if (command == "continue")
		return kContinue;
	return kUnknow;
}

bool is_irrelevant(SignalSet signal)
{
	if ((signal == kStart) && (start_flag == 0 && pause_flag == 0))
		return false;
	if ((signal == kPause) && (start_flag == 1 && pause_flag == 0))
		return false;
	if ((signal == kStop) && (start_flag == 1))
		return false;
	if ((signal == kContinue) && (start_flag == 1 && pause_flag == 1))
		return false;
	return true;
}

void execute_control_command(SignalSet &control_signal)
{
	switch (control_signal) {
	case kStart: {
		start_flag = 1;
		cout << "start simulation" << endl;
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
}

bool has_reached_endpoint(int input, int result)
{
	return (result - input == 10);
}