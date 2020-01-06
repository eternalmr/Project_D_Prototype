#pragma once
#pragma warning(disable:4996)

#include <thread>
#include <regex>
#include <map>
#include "client_class.h"

const int HEARTBEAT_INTERVAL = 2000;      //  millisecond
const int HEARTBEAT_TIMEOUT = 300000;    //  millisecond

using std::endl;
using std::cout;
using std::string;

typedef std::vector<Client> ClientMap;

Task* get_undo_task(std::vector<Task> &task_queue);
int assign_tasks(zmq::context_t &context, ClientMap &clients, 
				 std::vector<Task> &task_queue);
int collect_result(zmq::context_t &context);

std::vector<string> split_string(const string& in, const string& delim);

std::tuple<int, string> decode_signal(string &raw_signal);

void update_client_heartbeat(Client &a_client);

void mark_breakdown_client(ClientMap &clients);

void receive_heartbeat(zmq::context_t &context, ClientMap &clients);