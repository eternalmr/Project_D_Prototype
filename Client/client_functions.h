#pragma once

#include <thread>
#include "zhelpers.hpp"

using std::endl;
using std::cout;

const int HEARTBEAT_INTERVAL = 2000;

enum SignalSet {
	kStart = 111, kStop = 222, kPause = 333,
	kContinue = 444, kUnknow = 555
};

extern int start_flag;
extern int pause_flag;
extern int stop_flag;

void subscribe_specific_signal(zmq::socket_t &socket);
void send_heartbeat(zmq::context_t &, uint32_t);

int simulation_wrap(zmq::context_t &);
int simulation(int);
bool simulation_is_not_finished();

SignalSet listen_from_server(zmq::socket_t &socket);
bool is_irrelevant(SignalSet);
void execute_control_command(SignalSet &control_signal);
bool has_reached_endpoint(int, int);