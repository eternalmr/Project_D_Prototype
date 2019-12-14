// this_thread::sleep_for example
#pragma warning(disable:4996)

#include <iostream>       // std::cout
#include <iomanip>        // std::put_time
#include <thread>         // std::this_thread::sleep_until
#include <chrono>         // std::chrono::system_clock
#include <ctime>          // std::time_t, std::tm, std::localtime, std::mktime

int main()
{
	using std::chrono::system_clock;
	std::time_t tt = system_clock::to_time_t(system_clock::now());

	struct std::tm * ptm = std::localtime(&tt);
	std::cout << "Current time: " << std::put_time(ptm, "%X") << '\n';

	std::cout << "Waiting for the next minute to begin...\n";
	++ptm->tm_min; ptm->tm_sec = 0;
	//std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
	std::this_thread::sleep_for(std::chrono::minutes(1));

	std::cout << std::put_time(ptm, "%X") << " reached!\n";

	system("pause");
	return 0;
}


//#pragma warning(disable:4996)
//
//#include "zhelpers.hpp"
//#include <thread>
//#include <fstream>
//#include<mutex>
//
//#define start 111
//#define stop  222
//#define pause 333
//
//int listen_signal_from_main() {
//	return start;
//}
//
//void simulation() {
//	int signal;
//	int count = 0;
//	bool alreadyStarted = false;
//	std::fstream result("result.txt");
//	
//
//	while (count < 5) {
//		signal = listen_signal_from_main();
//
//		if (signal == start) { alreadyStarted = true; }
//		if (signal == stop)  { break; }//stop simulation
//
//		if (alreadyStarted) {//start simulation loop
//			//simulation loop
//			if (result.is_open()) {
//				time_t currentTime = time(0);
//				struct tm *t = localtime(&currentTime);
//				result << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << std::endl;//output current time
//				Sleep(1000);//do some work
//			}
//			count++;
//		}
//	}
//
//	result.close();
//}
//
//int main()
//{
//	std::thread t1(simulation);
//
//	std::cout << "this is main thread" << std::endl;//
//
//	t1.join();
//
//	system("pause");
//	return 0;
//}