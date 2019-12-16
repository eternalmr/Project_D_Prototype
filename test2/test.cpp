#include <iostream>
#include <windows.h>
#include <thread>
#include <assert.h>

using std::endl;
using std::cout;

enum signal_set {start = 111, stop = 222, pause = 333};

int start_flag = 1;
int pause_flag = 0;
int stop_flag = 0;

int sim(int arg);
signal_set listen_from_server();
bool is_irrelevant(signal_set);

int main()
{	
	std::thread t1(sim, 5);
	signal_set signal;

	while(true){
		signal = listen_from_server();

		assert(!(start_flag == 0 && pause_flag == 1));//impossible situation

		if( is_irrelevant(signal)) continue;

		switch(signal){
			case (start):{
				if(start_flag == 0 && pause_flag == 0){
					start_flag = 1;
					cout<<"execute start command"<<endl;
				}else if(start_flag == 1 && pause_flag ==1){
					pause_flag = 0;
					cout<<"continue simulation"<<endl;
				}else{
					cout<<"impossible to get here"<<endl;
				}
				break;
			}
			case (pause):{
				if(start_flag == 1 && pause_flag ==0){
					pause_flag = 1;
					cout<<"pause simulation"<<endl;
				}else{
					cout<<"impossible to get here"<<endl;
				}
				break;
			}
			case (stop):{
				if(start_flag == 1){
					start_flag = 0;
					pause_flag = 0;
					stop_flag = 1;
					cout<<"stop simulation"<<endl;
				}else{
					cout<<"impossible to get here"<<endl;
				}
				break;
			}
			default:
				cout<<"unknown command"<<endl;
		}

		if (stop_flag) break;
	}

	t1.join();

	system("pause");
	return 0;
}

bool is_irrelevant(signal_set signal)
{
	if((signal == start) && (start_flag == 1 && pause_flag ==0)) //已开始，未暂停
		return true;
	if((signal == pause) && (start_flag == 0 && pause_flag == 0))//未开始
		return true;
	if((signal == pause) && (start_flag == 1 && pause_flag == 1))//已开始，已暂停
		return true;
	if((signal == stop) && (start_flag == 0 && pause_flag == 0))//未开始
		return true;
	return false;
}

int sim(int arg)
{
	int result = arg;
	for (int i = 0; i < 10; i++) {
		result++;
		cout<< "result: "<<result<<endl;
		Sleep(1000);
	}
	return result;
}

signal_set listen_from_server()
{
	char command;
	cout << "please input your command: " << endl;
	std::cin >> command;

	if (command == 's') 
		return start;
	if (command == 'p')
		return pause;
	if (command == 'e')
		return stop;
	return stop;
}
