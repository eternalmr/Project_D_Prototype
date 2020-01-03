#pragma warning(disable:4996)

#include "client_class.h"

// constructor
Client::Client() : node_id_(0), node_status_(kFree) {}
Client::Client(unsigned int id) : node_id_(id), node_status_(kFree)
{
	heartbeat_ = s_clock();
}

// get functions
unsigned int Client::get_node_id() const { return node_id_; }
Client::NodeStatus Client::get_node_status() const { return node_status_; }
Task Client::get_task() const { return task_; }
int64_t Client::get_heartbeat() const { return heartbeat_; }

// set functions
void Client::set_node_id(unsigned int id) { node_id_ = id; }
void Client::set_node_status(NodeStatus status) { node_status_ = status; }
void Client::set_task(Task task) { task_ = task; }
void Client::set_heartbeat(int64_t heartbeat) { heartbeat_ = heartbeat; }

void Client::set_breakdown() { node_status_ = kBreakdown; }
bool Client::is_breakdown() { return node_status_ == kBreakdown; }

bool Client::is_expiry()
{
	const int HEARTBEAT_TIMEOUT = 5000; //millisecond
	return (s_clock() - get_heartbeat() > HEARTBEAT_TIMEOUT); 
}

