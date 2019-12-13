/* 
创建两个线程，一个线程用来通信，一个线程用来仿真
仿真线程接收从通信线程来的消息，判断具体做出什么响应

*/
#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <thread>

//  simulation thread
void *simu(void *arg) {

	zmq::context_t * context = static_cast<zmq::context_t*>(arg);

	//  Bind to inproc: endpoint, then start upstream thread
	zmq::socket_t receiver(*context, ZMQ_REP);
	receiver.connect("inproc://simu");

	//  Wait for signal
	std::string str = s_recv(receiver);
	std::cout << "simu: receive signal: " << str << " from main" << std::endl;

	//  Do some work
	Sleep(1000);
	std::cout << "Do some work" << std::endl;

	// Reply to main
	s_send(receiver, "work finished!");

	return (NULL);
}

//  Main program starts simulation and send signal to simulation

int main() {

	zmq::context_t context(1);

	//  Bind to inproc: endpoint, then start upstream thread
	zmq::socket_t sender(context, ZMQ_REQ);
	sender.bind("inproc://simu");

	// Start simu thread
	std::thread t1(simu, &context);

	// Send start signal
	s_send(sender, "start");

	// Receive feedback signal
	std::string str = s_recv(sender);
	std::cout << "main: receive signal: " << str << " from simu" << std::endl;

	t1.join();

	system("pause");
	return 0;
}