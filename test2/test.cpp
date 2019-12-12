
//#include <iostream>
//#include <thread>
//#include <windows.h>
//
//void msg()
//{
//	MessageBoxA(0, "对话框内容", "对话框标题", 0);//弹出对话框
//}
//
//void main()
//{
//	auto n = std::thread::hardware_concurrency();//检测CPU有多少个核心
//	std::cout << n << std::endl;
//
//	std::cout << "thread=" << std::this_thread::get_id() << std::endl;//获取当前线程编号
//
//	std::thread thread1(msg);//创建多线程
//	std::thread thread2(msg);
//
//	thread1.join();//开始执行，同时弹出2个对话框
//	thread2.join();
//
//	system("pause");
//}



#include <iostream>
#include <thread>

void fun1()
{
	std::cout << "this is fun1"<< std::endl;
}

void fun2()
{
	std::cout << "this is fun2" << std::endl;
}

void fun3()
{
	std::cout << "this is fun3" << std::endl;
}

void fun4()
{
	std::cout << "this is fun4" << std::endl;
}

void fun5()
{
	std::cout << "this is fun5" << std::endl;
}



int main()
{
	std::thread t1(fun1);
	std::thread t2(fun2);
	std::thread t3(fun3);
	std::thread t4(fun4);
	std::thread t5(fun5);

	
	

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();


	system("pause");

	return 0;
}