#include<iostream>
#include<thread>
#include<mutex>
#include<list>
using namespace std;

class MsgManage
{
public:
	MsgManage() {}
	~MsgManage() {}
	void InMsg()
	{
		for (int i = 0; i < 10; i++)
		{
			
			std::unique_lock<std::mutex> guard(myMutex);
			cout << "插入元素: " << i << endl;
			myList.push_back(i);
			//把被阻塞在wait()的线程唤醒
			condition.notify_one();
		}
	}

	void OutMsg()
	{
		int num;
		while (true)
		{
			std::unique_lock<std::mutex> guard(myMutex);
			//列表为空时，对互斥量解锁，程序阻塞，等待被唤醒。
			//列表不为空时，程序继续执行。
			condition.wait(guard, [this] {
				if (!myList.empty())
					return true;
				return false;
			});
			//程序执行到这里，列表不为空，且互斥量已被加锁
			num = myList.front();
			myList.pop_front();
			cout << "移除元素: " << num << endl;
			//解锁，避免互斥量被锁住太长时间
			guard.unlock();
			//程序继续执行其它耗时代码
		}
	}
private:
	list<int> myList;
	mutex myMutex;
	condition_variable condition;  //条件变量对象和互斥量配合使用
};

int main()
{
	MsgManage manage;
	thread outMsg(&MsgManage::OutMsg, &manage);
	thread inMsg(&MsgManage::InMsg, &manage);
	inMsg.join();
	outMsg.join();
	return 0;
}