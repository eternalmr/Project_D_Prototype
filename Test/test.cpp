#include <iostream>
#include <windows.h>
#include <thread>
#include <assert.h>

using std::endl;
using std::cout;

enum signal_set {start = 111, stop = 222, pause = 333};

int start_flag = 0;
int pause_flag = 0;
int stop_flag = 0;

int sim(int arg);
signal_set listen_from_server();
bool is_irrelevant(signal_set);

int main()
{	

	std::thread simulation_thread(sim, 5);
	signal_set signal;

	while(true){
		if (stop_flag) break;

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
	}

	simulation_thread.join();

	system("pause");
	return 0;
}

bool has_reached_endpoint(int input, int result)
{
	return (result - input == 10);
}

// ���������+10�����
int sim(int input)
{
	int result = input;

	while (!start_flag) {
		std::this_thread::yield();
	}

	while (true) {
		if (stop_flag) {
			return -1;
		}else if (start_flag == 1 && pause_flag == 1) {
			std::this_thread::yield();
			continue;
		}
		else {
			result++;
			cout << "result: " << result << endl;
			Sleep(1000);
		}

		if (has_reached_endpoint(input, result)) {//��������������յ�
			cout << "simulation finished!" << endl;
			stop_flag = 1;
			break; 
		}
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
	return pause;
}

bool is_irrelevant(signal_set signal)
{
	if ((signal == start) && (start_flag == 1 && pause_flag == 0)) //�ѿ�ʼ��δ��ͣ
		return true;
	if ((signal == pause) && (start_flag == 0 && pause_flag == 0))//δ��ʼ
		return true;
	if ((signal == pause) && (start_flag == 1 && pause_flag == 1))//�ѿ�ʼ������ͣ
		return true;
	if ((signal == stop) && (start_flag == 0 && pause_flag == 0))//δ��ʼ
		return true;
	return false;
}