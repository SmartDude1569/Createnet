#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <deque>
#include <memory>
#include <cstdlib>
#include <set>
#include <utility>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

#include "NetworkPacket.hpp"
#include "PacketReader.hpp"

using boost::asio::ip::tcp;

typedef std::deque<NetworkPacket> packet_queue;

class NetworkParticipant {
public:
	virtual ~NetworkParticipant() {}
	virtual void send(const NetworkPacket& packet) = 0;
};

typedef std::shared_ptr<NetworkParticipant> participant_ptr;

class NetworkRoom {
public:
	void join(participant_ptr participant) {
		participants_.insert(participant);
		for (auto packet : packets_) participant->send(packet);
	}
	void leave(participant_ptr participant) {
		participants_.erase(participant);
	}
	void send(const NetworkPacket& packet) {
		packets_.push_back(packet);
		while (packets_.size() > max_packets) packets_.pop_front();
		for (auto participant : participants_) participant->send(packet);
	}
private:
	std::set<participant_ptr> participants_;
	enum { max_packets = 128 };
	packet_queue packets_;
};

class NetworkSession : public NetworkParticipant, public std::enable_shared_from_this<NetworkSession> {
public:
	NetworkSession(tcp::socket socket, NetworkRoom& room) : socket_(std::move(socket)), room_(room) {}
	void start() {
		room_.join(shared_from_this());
		read_header();
	}
	void send(const NetworkPacket& packet) {
		bool ready = write_packets_.empty();
		write_packets_.push_back(packet);
		if (ready) write();
	}
private:
	void read_header() {
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_packet_.data(), NetworkPacket::header_length),
			[this, self](boost::system::error_code ec, std::size_t /* length? */) {
				if (!ec && read_packet_.decode_header()) {
					read_body();
				}
				else {
					room_.leave(shared_from_this());
				}
			});
	}
	void read_body() {
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_packet_.body(), read_packet_.body_length()),
			[this, self](boost::system::error_code ec, std::size_t /* length? */) {
				if (!ec) {
					PacketReader* reader = new PacketReader(read_packet_);
					bool shouldEcho = this->handlePacket(reader);
					if (shouldEcho) room_.send(read_packet_);
					read_header();
				}
				else {
					room_.leave(shared_from_this());
				}
			});
	}
	bool handlePacket(PacketReader* reader) {
		unsigned char type = reader->readByte();
		bool shouldEcho = false;
		switch (type) {
		case 136: { // chat message
			uint32_t id = reader->readUint32();
			std::string message = reader->readString();
			std::cout << "<" << std::to_string(id) << "> " << message << std::endl;
			shouldEcho = true;
			break;
		}
		case 0: { // user join... perchance?
			uint32_t id = reader->readUint32();
			std::cout << "user " << std::to_string(id) << " joined" << std::endl;
			shouldEcho = true;
		}
		}
		return shouldEcho;
	}
	void write() {
		auto self(shared_from_this());
		boost::asio::async_write(socket_,
			boost::asio::buffer(write_packets_.front().data(), write_packets_.front().length()),
			[this, self](boost::system::error_code ec, std::size_t /* length? */) {
				if (!ec) {
					write_packets_.pop_front();
					if (!write_packets_.empty()) write();
				}
				else {
					room_.leave(shared_from_this());
				}
			});
	}
	tcp::socket socket_;
	NetworkRoom& room_;
	NetworkPacket read_packet_;
	packet_queue write_packets_;
};

class NetworkServer {
public:
	NetworkServer(boost::asio::io_context& io_context, const tcp::endpoint& endpoint) : acceptor_(io_context, endpoint) {
		accept();
	}
private:
	void accept() {
		acceptor_.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket) {
				if (!ec) {
					std::make_shared<NetworkSession>(std::move(socket), room_)->start();
					std::cout << "new client" << std::endl;
				}
				accept();
			}
		);
	}
	tcp::acceptor acceptor_;
	NetworkRoom room_;
};