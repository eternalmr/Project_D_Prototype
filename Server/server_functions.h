#pragma once
#pragma warning(disable:4996)

#include <thread>
#include <regex>
#include <map>
#include "client.h"

const int HEARTBEAT_INTERVAL = 2000;      //  millisecond
const int HEARTBEAT_TIMEOUT = 300000;    //  millisecond

using std::endl;
using std::cout;
using std::string;

int assign_tasks(zmq::context_t &context);
int collect_result(zmq::context_t &context);

//用delim指定的正则表达式将字符串in分割，返回分割后的字符串数组
//delim 分割字符串的正则表达式
std::vector<string> split_string(const string& in, const string& delim);

std::tuple<int, string> decode_signal(string &raw_signal);

bool node_is_expiry(); // TODO

void update_client_heartbeat(Client &a_client);

void delete_breakdown_client(std::map<uint32_t, Client> &clients);

void receive_heartbeat(zmq::context_t &context);