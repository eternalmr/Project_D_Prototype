/* 
创建两个线程，一个线程用来通信，一个线程用来仿真
仿真线程接收从通信线程来的消息，判断具体做出什么响应

*/
#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <thread>
#include <fstream>

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
			if (pause_flag == 1)
			{
				sleep(1 second);
				continue;		
			}
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

//  Main program starts simulation and send signal to simulation
pauseflag = 0
int main()
{
	zmq::context_t context(1);

	//  Bind to inproc: simu, then start simu thread
	zmq::socket_t sender(context, ZMQ_REQ);
	sender.bind("inproc://simu");

	// Start simu thread
	std::thread t1(simu, &context);
	byte command = recieve_from_server();
	if (command==start) {
		
		t1.join();
	}
	else if (command == stop)
	{
		t1.kill()
	}
	else if (command = heartbeat)
	{
		reply()
	}
	else if (command == pause)
	{
		pauseflag=1
	}
	// Send start signal
	s_send(sender, "start");

	// Receive feedback signal
	std::string str = s_recv(sender);
	std::cout << "main: receive signal: " << str << " from simu" << std::endl;

	sender.close();
	context.close();

	system("pause");
	return 0;
}