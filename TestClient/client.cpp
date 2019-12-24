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

int Simulation(int);
SignalSet ListenFromServer(zmq::socket_t &socket);
bool IsIrrelevant(SignalSet);
bool HasReachedEndpoint(int, int);

int main()
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	socket.connect("tcp://localhost:5555");

	std::string command = s_recv(socket);
	cout << "Received [" << command << "] from server" << endl;

	s_send(socket, "I'm ready");
	cout << "Telling server that I'm ready'" << endl;

	SignalSet signal;
	std::thread simulation_thread(Simulation, 5);

	while (true) {
		//impossible situation
		assert(!(start_flag == 0 && pause_flag == 1));

		signal = ListenFromServer(socket);

		if (stop_flag) {
			s_send(socket, "finished");
			break;
		}

		if (IsIrrelevant(signal)) {
			s_send(socket, "Wrong Command");
			continue;
		}

		switch (signal) {
			case kStart: {
				start_flag = 1;
				cout << "execute start command" << endl;
				s_send(socket, "");
				break;
			}
			case kContinue: {
				pause_flag = 0;
				cout << "continue simulation" << endl;
				s_send(socket, "");
				break;
			}
			case kPause: {
				pause_flag = 1;
				cout << "pause simulation" << endl;
				s_send(socket, "");
				break;
			}
			case kStop: {
				start_flag = 0;
				pause_flag = 0;
				stop_flag = 1;
				s_send(socket, "interrupt");
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

		if (HasReachedEndpoint(input, result)) {
			stop_flag = 1;
			cout << "simulation finished!" << endl;
			break;
		}

		result++;
		Sleep(1000);
		cout << "result: " << result << endl;
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