#pragma warning(disable:4996)

#include "zhelpers.hpp"
#include <thread>
#include <fstream>
#include<mutex>

#define start 111
#define stop  222
#define pause 333

int listen_signal_from_main() {
	return start;
}

void simulation() {
	int signal;
	int count = 0;
	bool alreadyStarted = false;
	std::fstream result("result.txt");
	

	while (count < 5) {
		signal = listen_signal_from_main();

		if (signal == start) { alreadyStarted = true; }
		if (signal == stop)  { break; }//stop simulation

		if (alreadyStarted) {//start simulation loop
			//simulation loop
			if (result.is_open()) {
				time_t currentTime = time(0);
				struct tm *t = localtime(&currentTime);
				result << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << std::endl;//output current time
				Sleep(1000);//do some work
			}
			count++;
		}
	}

	result.close();
}

int main()
{
	std::thread t1(simulation);

	std::cout << "this is main thread" << std::endl;//

	t1.join();

	system("pause");
	return 0;
}