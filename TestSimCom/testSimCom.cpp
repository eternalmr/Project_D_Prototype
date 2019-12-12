/* 
创建两个线程，一个线程用来通信，一个线程用来仿真
仿真线程接收从通信线程来的消息，判断具体做出什么响应

*/

#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <thread>
#include <fstream>


void simulation() {
	std::cout << "this is simulation thread" << std::endl;//
	std::ofstream result("result.txt");

	zmq::context_t context(1);
	zmq::socket_t receiver(context, ZMQ_REQ);
	receiver.connect("tcp://*:5555");

	s_send(receiver, "I'm ready");
	std::string command = s_recv(receiver);
	std::cout << command << std::endl;

	receiver.close();
	context.close();

	
	if (result.is_open()) {

		result << "simulation start\n";

		for (int i = 0; i < 5; i++) {//simulation process
			time_t currentTime = time(0);
			struct tm *t = localtime(&currentTime);
			result << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << std::endl;//output current time
			Sleep(1000);//do some work
		}

		result << "simulation finished\n";
	}

	result.close();
}

void communication() {
	std::cout << "this is communication thread" << std::endl;//

	zmq::context_t context(1);
	zmq::socket_t sender(context, ZMQ_REP);
	sender.bind("tcp://*:5555");

	s_recv(sender);//接收ready信号
	s_send(sender, "start");

	sender.close();
	context.close();
}


int main()
{
	std::thread t1(simulation);
	std::thread t2(communication);


	std::cout << "this is main thread" << std::endl;//

	t1.detach();
	t2.detach();
	//Sleep(100);
	

	system("pause");
	return 0;
}