#pragma warning(disable:4996)

#include <thread>
#include "zhelpers.hpp"

using std::endl;
using std::cout;

enum SignalSet { kStart = 111, kStop = 222, kPause = 333, 
			  kContinue = 444, kUnknow = 555 };
				
int start_flag = 0;
int pause_flag = 0;
int  stop_flag = 0;

int SimulationWrap();
int Simulation(int);
SignalSet ListenFromServer(zmq::socket_t &socket);
bool IsIrrelevant(SignalSet);
bool HasReachedEndpoint(int, int);

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

int main()
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_SUB);
	socket.connect("tcp://192.168.100.239:5555");

	SignalSet signal;
	subscribe_specific_signal(socket);

	std::thread simulation_thread(SimulationWrap);

	while (true) {
		//impossible situation
		assert(!(start_flag == 0 && pause_flag == 1));

		signal = ListenFromServer(socket);

		if (stop_flag) {
			break;
		}

		if (IsIrrelevant(signal)) {
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

	system("pause");
	return 0;
}

int SimulationWrap()
{
	zmq::context_t context(1);
	zmq::socket_t worker(context, ZMQ_REQ);
	worker.connect("tcp://192.168.100.239:5560");

	int task_input;
	int result;
	while (true) {
		// Send ready to server
		std::cout << "**********************************************" << std::endl;
		s_send(worker, "ready");

		// Receive a task from server
		std::string new_task = s_recv(worker);
		std::cout << "Receive a new task: " << new_task << std::endl;

		// Do some 'work'
		stop_flag = 0; //reset stop flag
		task_input = atoi(new_task.c_str());
		result = Simulation(task_input);

		if (result == -1) {
			std::cout << "Simulation interrupt" << std::endl;
			break;
		}

		std::cout << "**********************************************" << std::endl;
	}

	worker.close();
	context.close();
	return 0;
}

int Simulation(int input)
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

		if (HasReachedEndpoint(input, result)) {
			stop_flag = 1;
			cout << "simulation finished!" << endl;
			break;
		}
	}

	return result;
}

SignalSet ListenFromServer(zmq::socket_t &socket)
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

bool IsIrrelevant(SignalSet signal)
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

bool HasReachedEndpoint(int input, int result)
{
	return (result - input == 10);
}