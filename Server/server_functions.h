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

typedef std::map<uint32_t, Client> ClientMap;

bool task_is_not_start(Task &task);
std::vector<Task>::iterator get_undo_task(std::vector<Task> &task_queue);
int assign_tasks(zmq::context_t &context, ClientMap &clients, std::vector<Task> &task_queue);
int collect_result(zmq::context_t &context);

//用delim指定的正则表达式将字符串in分割，返回分割后的字符串数组
//delim 分割字符串的正则表达式
std::vector<string> split_string(const string& in, const string& delim);

std::tuple<int, string> decode_signal(string &raw_signal);

void update_client_heartbeat(Client &a_client);

void mark_breakdown_client(ClientMap &clients);

void receive_heartbeat(zmq::context_t &context, ClientMap &clients);