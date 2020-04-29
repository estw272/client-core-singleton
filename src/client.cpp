#include "pch.h"
#include "../include/clientcore/client.h"


Client::Client(unsigned int num_threads) : 
	thread_count_(num_threads), 
	work_(ios_), 
	ping_timer_(ios_), 
	ping_delay_(0), 
	ping_str_("") {
}

Client::~Client() {
	conn_ptr_->stop();
}

void Client::send_ping() {
	send_msg(ping_str_);
	ping_timer_.expires_from_now(boost::posix_time::seconds(ping_delay_));
	ping_timer_.async_wait(boost::bind(&Client::send_ping, this));
}

void Client::connect(std::string ip, unsigned int port) {

	//spawn threads
	for (unsigned int i = 0; i < thread_count_; ++i) {
		thread_pool_.emplace_back([me=this](){
			me->ios_.run();
		});
	}

	conn_ptr_ = Connection::create(ios_, ip, port);

	if (ping_delay_ > 0 && !ping_str_.empty()) {
		ping_timer_.expires_from_now(boost::posix_time::seconds(ping_delay_));
		ping_timer_.async_wait(boost::bind(&Client::send_ping, this));
	}

	conn_ptr_->start();
}

void Client::add_response(std::string message) {
	{
		std::lock_guard lk(response_mut_);
		response_queue_.push(message);
		response_cv_.notify_one();
	}
}

std::queue<std::string>& Client::response_queue() {
	return response_queue_;
}

std::mutex& Client::response_mutex() {
	return response_mut_;
}

std::pair<std::mutex&, std::condition_variable&> Client::get_response_lock() {
	return std::make_pair(std::ref(response_mut_), std::ref(response_cv_));
}

void Client::set_ping_delay(unsigned int delay) {
	ping_delay_ = delay;
}

void Client::set_ping(std::string& ping_str) {
	ping_str_ = ping_str;
}
