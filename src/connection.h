#ifndef CONNECTION_H_KUDEQ8CR
#define CONNECTION_H_KUDEQ8CR


class Connection : public std::enable_shared_from_this<Connection> {
public:
	Connection(const Connection &) =delete;
	Connection& operator=(const Connection &) =delete;

	static std::shared_ptr<Connection> create(boost::asio::io_service&, 
		std::string& ip, 
		unsigned int port
		/*std::condition_variable& cv, 
		std::mutex& msg_mut,
		std::deque<std::string>& msg_queue*/);

	void start();
	void stop();
	boost::asio::ip::tcp::socket& socket();

	void send_msg(std::string message);

private:
	explicit Connection(boost::asio::io_service&, 
		std::string& ip, 
		unsigned int port
		/*std::condition_variable& cv, 
		std::mutex& msg_mut,
		std::deque<std::string>& msg_queue*/);

	void start_read();
	void handle_read(const boost::system::error_code &, const size_t);
	void start_write();
	void handle_write(const boost::system::error_code &, const size_t);
	void handle_timeout(const boost::system::error_code &);
	void process_message();
	void send(std::string message);
	void queue_message(std::string message);

private:
	boost::asio::io_service&                     ios_;
	boost::asio::ip::tcp::socket                 socket_;
	boost::asio::io_service::strand              write_strand_;
	unsigned int                                 buffer_len_ = 4096;
	std::array<char, 4096>                       buffer_in_;
	boost::asio::streambuf                       buffer_out_;
	boost::asio::streambuf                       packet_in_;
	boost::asio::streambuf::mutable_buffers_type read_packet_in_ = packet_in_.prepare(4096);

	bool                    message_ongoing_;
	std::deque<std::string> buffer_out_queue_;

	bool showing_message_;

	std::string prev_message_;
	std::string message_buffer_ {};

	boost::asio::deadline_timer timeout_timer_;

	boost::asio::ip::tcp::endpoint remote_endpoint_;

	bool connected_;

	//std::condition_variable& cv_;
	//std::mutex&              msg_mut_;
	//std::deque<std::string>& msg_queue_;

	const std::string msg_begin_str_ = "__MSG_STS__";
	const std::string msg_end_str_ = "__MSG_STE__";
};

#endif /* end of include guard: CONNECTION_H_KUDEQ8CR */