#pragma once
#pragma warning(disable:4996)

class Task
{
private:
	enum ComputeStatus { kNotStart = 0, kInComputing, kFinished };
	enum   StoreStatus { kNotSave = 0, kSaved };

public:
	Task();
	Task(unsigned int id);
	int get_id() const;

	ComputeStatus get_compute_status() const;
	StoreStatus get_store_status() const;

	void set_compute_status(ComputeStatus status);
	void set_store_status(StoreStatus status);

private:
	void set_id(unsigned int id);

private:
	unsigned int id_;
	ComputeStatus compute_status_;
	StoreStatus store_status_;
};

