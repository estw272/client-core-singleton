#pragma once
#ifndef _CLIENT_H_8768765
#define _CLIENT_H_8768765

#include "../../src/connection.h"

class Client {
private:
	unsigned int thread_count_;
	std::vector<std::thread> thread_pool_;
	boost::asio::io_service ios_;
	boost::asio::io_service::work work_;
	boost::asio::deadline_timer ping_timer_;

	std::shared_ptr<Connection> conn_ptr_;

	std::condition_variable response_cv_;
	std::mutex              response_mut_;
	std::queue<std::string> response_queue_;

	unsigned int ping_delay_;
	std::string  ping_str_;

public:
	Client(const Client&) = delete;
	Client& operator=(const Client&) = delete;
	Client(Client&&) = delete;
	Client& operator=(Client&&) = delete;

	static Client& instance() {
		static Client instance;
		return instance;
	}

	void connect(std::string ip, unsigned int port);

public:
	template <typename Proto>
	void send_msg(Proto prot) {
		std::string msg_str;
		try {
			prot.SerializeToString(&msg_str);
		} catch (const std::exception& e) {
			return;
		}
		conn_ptr_->send_msg(msg_str);
	}

	template<>
	void send_msg<std::string>(std::string message) {
		conn_ptr_->send_msg(message);
	}

	void add_response(std::string message);
	std::queue<std::string>& response_queue();
	std::mutex& response_mutex();
	std::pair<std::mutex&, std::condition_variable&> get_response_lock();

	void set_ping_delay(unsigned int delay);
	void set_ping(std::string& ping_str);

private:
	explicit Client(unsigned int num_threads=1);
	~Client();

	void send_ping();

};

#endif

