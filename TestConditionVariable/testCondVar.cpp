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
			cout << "����Ԫ��: " << i << endl;
			myList.push_back(i);
			//�ѱ�������wait()���̻߳���
			condition.notify_one();
		}
	}

	void OutMsg()
	{
		int num;
		while (true)
		{
			std::unique_lock<std::mutex> guard(myMutex);
			//�б�Ϊ��ʱ���Ի����������������������ȴ������ѡ�
			//�б�Ϊ��ʱ���������ִ�С�
			condition.wait(guard, [this] {
				if (!myList.empty())
					return true;
				return false;
			});
			//����ִ�е�����б�Ϊ�գ��һ������ѱ�����
			num = myList.front();
			myList.pop_front();
			cout << "�Ƴ�Ԫ��: " << num << endl;
			//���������⻥��������ס̫��ʱ��
			guard.unlock();
			//�������ִ��������ʱ����
		}
	}
private:
	list<int> myList;
	mutex myMutex;
	condition_variable condition;  //������������ͻ��������ʹ��
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