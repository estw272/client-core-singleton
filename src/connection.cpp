#include "pch.h"
#include "connection.h"
#include "../include/clientcore/client.h"

std::shared_ptr<Connection> Connection::create(boost::asio::io_service& ios, 
	std::string& ip, 
	unsigned int port) {

    return std::shared_ptr<Connection>(new Connection(ios, ip, port/*, cv, msg_mut, msg_queue*/));
}

Connection::Connection(boost::asio::io_service& ios, 
	std::string& ip, 
	unsigned int port) :
	ios_(ios), 
	remote_endpoint_(boost::asio::ip::address::from_string(ip), port), 
    socket_(ios, boost::asio::ip::tcp::v4()),
	write_strand_(ios), 
	message_ongoing_(false),
	showing_message_(false),
    timeout_timer_(ios), 
	connected_(false) {

}

void Connection::start() {
	socket_.connect(remote_endpoint_);

	connected_ = true;

	//start_write();
    start_read();
}

void Connection::stop() {
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec) {
    }
    socket_.close();
	connected_ = false;
}

boost::asio::ip::tcp::socket & Connection::socket() {
    return socket_;
}

void Connection::start_read() {

    socket_.async_read_some(boost::asio::buffer(buffer_in_), bind(
            &Connection::handle_read,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

void Connection::handle_read(const boost::system::error_code& ec, const size_t bytes) {
    if (!ec) {
		std::string temp_msg = std::string(buffer_in_.begin(), buffer_in_.begin() + bytes);

		message_buffer_ += temp_msg;

		process_message();

        start_read();
    } else if (boost::asio::error::eof == ec) {
    } else if (boost::asio::error::operation_aborted != ec) {
		this->stop();
    }
}

void Connection::start_write() {
	buffer_out_queue_.front() = msg_begin_str_ + buffer_out_queue_.front() + msg_end_str_;
    boost::asio::async_write(socket_, 
                            boost::asio::buffer(buffer_out_queue_.front()), 
							//boost::asio::buffer(message),
                            write_strand_.wrap([me=shared_from_this()](const boost::system::error_code& ec, std::size_t bytes_sent) {
                                me->handle_write(ec, bytes_sent);
                            }));
}

void Connection::handle_write(const boost::system::error_code& ec, size_t size) {
	if (ec) {
		LOG->error("error sending request to server (sent {} bytes): \n{}", size, ec.message());
		return;
	}

	buffer_out_queue_.pop_front();

	if (!buffer_out_queue_.empty()) {
		start_write();
	}
}

void Connection::handle_timeout(const boost::system::error_code & ec) {
    if (!ec) {
		this->stop();
    }
}

void Connection::process_message() {
	auto msg_start = message_buffer_.find(msg_begin_str_);
	if (msg_start != std::string::npos) {
		message_ongoing_ = true;

		auto msg_end = message_buffer_.find(msg_end_str_);
		if (msg_end != std::string::npos) {

			std::string complete_message = message_buffer_.substr(msg_start + msg_begin_str_.size(), msg_end - msg_start - msg_begin_str_.size());

			message_ongoing_ = false;

			Client::instance().add_response(complete_message);

			message_buffer_ = message_buffer_.substr(msg_end + msg_end_str_.size());
			process_message();
		} else {
			//if msg started but not ended just wait for msg end in next packets
		}
	} else {
		//if msg not started delete everything except for last x chars to track partial msg start (if buffer size more than x bytes
		if (message_buffer_.size() > 4096) {
			message_buffer_ = message_buffer_.erase(0, message_buffer_.size() - msg_begin_str_.size());
		}
	}
}

void Connection::send(std::string message) {

	ios_.post(write_strand_.wrap([me=shared_from_this(), message](){
        me->queue_message(message);
    }));
}

void Connection::queue_message(std::string message) {
	bool writing = !buffer_out_queue_.empty();
    buffer_out_queue_.push_back(std::move(message));

    if (!writing) {
        start_write();
    }
}

void Connection::send_msg(std::string message) {
	this->send(message);
}

