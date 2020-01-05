#pragma warning(disable:4996)

#include "task_class.h"

// constructors
Task::Task()
{
	set_id(0);
	set_compute_status(kNotStart);
	set_store_status(kNotSave);
}

Task::Task(unsigned int id)
{
	set_id(id);
	set_compute_status(kNotStart);
	set_store_status(kNotSave);
}

// get functions
int Task::get_id() const { return id_; }
Task::ComputeStatus Task::get_compute_status() const { return compute_status_; }
Task::StoreStatus Task::get_store_status() const { return store_status_; }

// set functions
void Task::set_id(unsigned int id) { id_ = id; }
void Task::set_compute_status(ComputeStatus status) { compute_status_ = status; }
void Task::set_store_status(StoreStatus status) { store_status_ = status; }

void Task::set_task_not_start() { compute_status_ = kNotStart; }
void Task::set_task_in_computing() { compute_status_ = kInComputing; }
void Task::set_task_finished() { compute_status_ = kFinished; }

bool Task::is_not_start() { return kNotStart == compute_status_; }
bool Task::is_in_computing() { return kInComputing == compute_status_; }
