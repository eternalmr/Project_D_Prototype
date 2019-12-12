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
	std::cout << "put " << qf << " into queue" << std::endl;
	return qf;
}

void process(data_chunk data) {
	std::cout << data << std::endl;
}

bool is_last_chunk() {
	return data_queue.empty();
}

void data_preparation(std::queue<data_chunk> q)
{
	while (more_data_to_prepare(q))
	{
		data_chunk const data = prepare_data(q);
		data_queue.push(data);  // 2
	}
}

void data_processing()
{
	while (true)
	{
		data_chunk data = data_queue.front();
		data_queue.pop();
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

	data_preparation(myData);
	data_processing();

	system("pause");
	return 0;
}