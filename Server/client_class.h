#pragma once
#pragma warning(disable:4996)

#include "task_class.h"
#include "zhelpers.hpp"

class Client
{
public:
	enum NodeStatus { kFree = 0, kInComputing, kBreakdown };

public:
	Client();
	Client(unsigned int id);

	unsigned int get_node_id() const;
	NodeStatus get_node_status() const;
	Task get_task() const;
	int64_t get_heartbeat() const;

	void set_node_id(unsigned int id);
	void set_node_status(NodeStatus status);
	void set_task(Task task);
	void set_heartbeat(int64_t heartbeat);
	void set_breakdown();

	bool is_expiry();
	bool is_breakdown();

private:
	uint32_t node_id_;
	NodeStatus node_status_;
	Task task_;
	int64_t heartbeat_;
};
