#include <mutex>
#include <thread>
#include <queue>
#include <iostream>

#define data_chunk int

std::mutex mut;
std::queue<data_chunk> data_queue;  // 1
std::condition_variable data_cond;


bool more_data_to_prepare(std::queue<data_chunk> q) {
	return q.size() > 0;
}

data_chunk prepare_data(std::queue<data_chunk> &q) {
	data_chunk qf = q.front();
	q.pop();
	return qf;
}

void process(data_chunk data) {
	std::cout << data << std::endl;
}

bool is_last_chunk() {
	return data_queue.empty();
}

void data_preparation_thread(std::queue<data_chunk> &q)
{
	while (more_data_to_prepare(q))
	{
		data_chunk const data = prepare_data(q);
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(data);  // 2
		data_cond.notify_one();  // 3
	}
}

void data_processing_thread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lk(mut);  // 4
		data_cond.wait(
			lk, [] {return !data_queue.empty(); });  // 5
		data_chunk data = data_queue.front();
		data_queue.pop();
		lk.unlock();  // 6
		process(data);
		if (is_last_chunk())
			break;
	}
}

int main()
{
	std::queue<data_chunk> myData;

	for (int i = 0; i < 10; i++){
		myData.push(i);
	}

	std::thread t1(data_preparation_thread,myData);
	std::thread t2(data_processing_thread);

	t1.join();
	t2.join();

	system("pause");
	return 0;
}