#pragma once
#pragma warning(disable:4996)

typedef unsigned int uint32_t;

class Task
{
private:
	enum ComputeStatus { kNotStart = 0, kInComputing, kFinished };
	enum   StoreStatus { kNotSave = 0, kSaved };

public:
	Task();
	Task(uint32_t id);
	int get_id() const;

	ComputeStatus get_compute_status() const;
	StoreStatus get_store_status() const;

	void set_compute_status(ComputeStatus status);
	void set_store_status(StoreStatus status);

	void set_task_not_start();
	void set_task_in_computing();
	void set_task_finished();

	bool is_not_start();
	bool is_in_computing();

private:
	void set_id(uint32_t id);

private:
	unsigned int id_;
	ComputeStatus compute_status_;
	StoreStatus store_status_;
};

