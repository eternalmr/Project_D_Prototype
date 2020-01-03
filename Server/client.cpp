#pragma warning(disable:4996)

#include "client.h"

// constructor
Client::Client() : node_id_(0), node_status_(kFree) {}
Client::Client(unsigned int id) : node_id_(id), node_status_(kFree)
{
	heartbeat_ = s_clock();
	expiry_ = heartbeat_ + 300000; // heartbeat moment + 5 mins
}

// get functions
unsigned int Client::get_node_id() const { return node_id_; }
Client::NodeStatus Client::get_node_status() const { return node_status_; }
Task Client::get_task() const { return task_; }
int64_t Client::get_heartbeat() const { return heartbeat_; }
int64_t Client::get_expiry() const { return expiry_; }

// set functions
void Client::set_node_id(unsigned int id) { node_id_ = id; }
void Client::set_node_status(NodeStatus status) { node_status_ = status; }
void Client::set_task(Task task) { task_ = task; }
void Client::set_heartbeat(int64_t heartbeat) { heartbeat_ = heartbeat; }
void Client::set_expiry(int64_t expiry) { expiry_ = expiry; }