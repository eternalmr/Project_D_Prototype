/* 
创建两个线程，一个线程用来通信，一个线程用来仿真
仿真线程接收从通信线程来的消息，判断具体做出什么响应

*/
#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <thread>
#include <fstream>

using std::cout;
using std::endl;

// simulation thread
void *simu(void *arg);

// receive signal from server
std::string receive_from_server(std::string command = "start");

void execute_start_command(zmq::socket_t &);

//  Main program starts simulation and send signal to simulation
int main()
{
	//*************************** 1. initialize simulation ************************************
	zmq::context_t context(1);

	//// Connect to server(留接口，暂时不用)
	//std::string server_address = "tcp://localhost:5678";
	//zmq::socket_t receiver(context, ZMQ_REQ);
	//receiver.connect(server_address);

	//  Bind to inproc: simu, then start simu thread
	zmq::socket_t sender(context, ZMQ_REQ);
	sender.bind("inproc://simu");

	// Start simu thread
	std::thread t1(simu, &context);

	//*************************** 2. execute command from server ******************************
	std::string command = receive_from_server();

	if (command == "start") {
		cout << "execute start command" << endl;
		execute_start_command(sender);
	}
	else if (command == "pause"){
		cout << "execute pause command" << endl;
	}
	else if (command == "stop"){
		cout << "execute stop command" << endl;
	}
	else if (command == "heartbeat"){
		cout << "reply heartbeat signal" << endl;
	}
	else {
		cout << "unknown command" << endl;
	}

	//*************************** 3. exit simulation ****************************************
	t1.join();

	sender.close();
	context.close();

	system("pause");
	return 0;
}

void initialize()
{

}

void execute_start_command(zmq::socket_t &sender)
{
	// Send start signal
	s_send(sender, "start");
	// Receive feedback signal
	std::string str = s_recv(sender);
	std::cout << "main: receive signal: " << str << " from simu" << std::endl;
}

// simulation thread
void *simu(void *arg)
{
	zmq::context_t * context = static_cast<zmq::context_t*>(arg);

	//  Bind to inproc: endpoint, then start upstream thread
	zmq::socket_t receiver(*context, ZMQ_REP);
	receiver.connect("inproc://simu");

	//  Wait for signal
	std::string str = s_recv(receiver);
	std::cout << "simu: receive signal: " << str << " from main" << std::endl;

	//  Do some work
	std::cout << "Do some work" << std::endl;

	std::ofstream result("result.txt");
	if (result.is_open()) {
		result << "simulation start\n";
		for (int i = 0; i < 5; i++) {//simulation process
			//if (pause_flag == 1)
			//{
			//	sleep(1 second);
			//	continue;
			//}
			time_t currentTime = time(0);
			struct tm *t = localtime(&currentTime);
			result << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << std::endl;//output current time
			Sleep(1000);//do some work
		}
		result << "simulation finished\n";
	}

	// Reply to main
	s_send(receiver, "work finished!");

	receiver.close();

	return (NULL);
}

// receive signal from server
std::string receive_from_server(std::string command)
{
	return command;//暂时模拟一个输入信号
}