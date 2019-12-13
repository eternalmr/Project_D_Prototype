/* 
创建两个线程，一个线程用来通信，一个线程用来仿真
仿真线程接收从通信线程来的消息，判断具体做出什么响应

*/

#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <thread>
#include <fstream>
#include<mutex>

std::mutex mut;
std::condition_variable sim_cond;
std::condition_variable com_cond;
bool simFlag = false;
bool comFlag = false;


void simulation() {
	std::unique_lock<std::mutex> lck(mut);

	std::cout << "this is simulation thread" << std::endl;//
	std::ofstream result("result.txt");

	zmq::context_t context(1);
	zmq::socket_t receiver(context, ZMQ_REQ);
	receiver.connect("tcp://localhost:5555");
	
	s_send(receiver, "I'm ready");
	simFlag = true;
	sim_cond.notify_one();//通知communication线程解锁

	com_cond.wait(
		lck, []{return comFlag;});

	std::string command = s_recv(receiver);
	std::cout << command << std::endl;

	//写输出
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

	receiver.close();
	context.close();
	result.close();
}

void communication() {
	std::cout << "this is communication thread" << std::endl;//

	zmq::context_t context(1);
	zmq::socket_t sender(context, ZMQ_REP);
	sender.bind("tcp://localhost:5555");

	std::unique_lock<std::mutex> lck(mut);
	sim_cond.wait(
		lck, []{return simFlag; });//等待REQ发送ready信号
	s_recv(sender);

	s_send(sender, "start");
	comFlag = true;
	com_cond.notify_one();

	sender.close();
	context.close();
}

int main()
{
	std::thread t1(simulation);
	std::thread t2(communication);

	std::cout << "this is main thread" << std::endl;//

	t1.join();
	t2.join();

	system("pause");
	return 0;
}