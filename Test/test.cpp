#include <vector>
#include <queue>
#include <iostream>

using std::cout;
using std::endl;



class Task
{
 private:
	enum ComputeStatus { kNotStart = 0, kInComputing, kFinished };
	enum   StoreStatus { kNotSave = 0, kSaved };

 public:
	 Task()
	 {
		 set_id(0);
		 set_compute_status(kNotStart);
		 set_store_status(kNotSave);
	 }

	 explicit Task(unsigned int id)
	 {
		 set_id(id);
		 set_compute_status(kNotStart);
		 set_store_status(kNotSave);
	 }

	 int get_id() const { return id_; }
	 

	 ComputeStatus get_compute_status() const { return compute_status_; }
	 StoreStatus get_store_status() const { return store_status_; }

	 void set_compute_status(ComputeStatus status) { compute_status_ = status; }
	 void set_store_status(StoreStatus status) { store_status_ = status; }

 private:
	void set_id(unsigned int id) { id_ = id; }

 private:
	unsigned int id_;
	ComputeStatus compute_status_;
	StoreStatus store_status_;
};


class ComputeNode
{
private:
	enum NodeStatus { kFree = 0, kInComputing, kBreakdown };

public:
	ComputeNode() : node_id_(0), node_status_(kFree) {}
	explicit ComputeNode(unsigned int id) : node_id_(id), node_status_(kFree) {}

	unsigned int get_node_id() const { return node_id_; }
	NodeStatus get_node_status() const { return node_status_; }
	Task get_task() const { return task_; }

	void set_node_id(unsigned int id) { node_id_ = id; }
	void set_node_status(NodeStatus status) { node_status_ = status; }
	void set_task(Task task) { task_ = task; }

private:
	unsigned int node_id_;
	NodeStatus node_status_;
	Task task_;
};

int main()
{
	//create a tasks container
	std::vector<Task> tasks;
	for (int i = 0; i < 10; ++i) {
		tasks.push_back(Task(i));
	}

	std::vector<Task>::iterator it;
	for (it = tasks.begin(); it != tasks.end(); ++it) {
		//it->set_id(1);
		cout << "Task[" << it->get_id() << "]: compute status: " 
			<< it->get_compute_status() << " store status: " 
			<< it->get_store_status() << endl;
	}

	//create a compute node queue
	std::queue<ComputeNode> workers;
	for (int i = 0; i < 3; ++i) {
		workers.push(ComputeNode(i));

	}

	ComputeNode node;
	for (int i = 0; i < 3; ++i) {
		ComputeNode node = workers.front();
		workers.pop();

		node.set_task(tasks[i]);
		cout << "worker[" << node.get_node_id() << "]: node status: "
		<< node.get_node_status() << " assigned task: "
		<< node.get_task().get_id() << endl;
		
		workers.push(node);
	}



	system("pause");
	return 0;
}