#pragma once
#pragma warning(disable:4996)

#include "task.h"
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
	int64_t get_expiry() const;

	void set_node_id(unsigned int id);
	void set_node_status(NodeStatus status);
	void set_task(Task task);
	void set_heartbeat(int64_t heartbeat);
	void set_expiry(int64_t expiry);

private:
	uint32_t node_id_;
	NodeStatus node_status_;
	Task task_;
	int64_t heartbeat_;
	int64_t expiry_;
};

