#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "NetworkServer.hpp"
#include "NetworkPacket.hpp"
#include "PacketWriter.hpp"
#include "PacketReader.hpp"

using boost::asio::ip::tcp;

class NetworkClient {
public:
	NetworkClient(boost::asio::io_context& io_context, const tcp::resolver::results_type& endpoints, uint32_t id) : io_context_(io_context), socket_(io_context) {
		this->id = id;
		connect(endpoints);
	}
	void write(const NetworkPacket& packet) {
		boost::asio::post(io_context_,
			[this, packet]() {
				bool ready = write_packets_.empty();
				write_packets_.push_back(packet);
				if (ready) write();
			});
	}
	void close() {
		boost::asio::post(io_context_, [this]() { socket_.close(); });
	}
private:
	void connect(const tcp::resolver::results_type& endpoints) {
		uint32_t id = this->id;
		boost::asio::async_connect(socket_, endpoints,
			[this, id](boost::system::error_code ec, tcp::endpoint) {
				if (!ec) {
					std::cout << "connected to server" << std::endl;
					PacketWriter writer;
					writer.writeByte(0);
					writer.writeUint32(id);
					this->write(writer.packet());
					read_header();
				}
			});
	}
	void read_header() {
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_packet_.data(), NetworkPacket::header_length),
			[this](boost::system::error_code ec, std::size_t /* length? */) {
				if (!ec && read_packet_.decode_header()) read_body();
				else socket_.close();
			});
	}
	void read_body() {
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_packet_.body(), read_packet_.body_length()),
			[this](boost::system::error_code ec, std::size_t /* length? */) {
				if (!ec) {
					PacketReader* reader = new PacketReader(read_packet_);
					handlePacket(reader);
					read_header();
				}
				else socket_.close();
			});
	}
	void handlePacket(PacketReader* reader) {
		unsigned char type = reader->readByte();
		switch (type) {
		case 136: { // chat message
			uint32_t userId = reader->readUint32();
			std::string message = reader->readString();
			std::cout << "<" << std::to_string(userId) << "> " << message << std::endl;
			break;
		}
		case 0: { // user join... perchance?
			uint32_t userId = reader->readUint32();
			if (userId != this->id) std::cout << "user " << std::to_string(userId) << " joined" << std::endl;
			else std::cout << "server acknowledged that we exist!!" << std::endl;
			break;
		}
		}
	}
	void write() {
		boost::asio::async_write(socket_,
			boost::asio::buffer(write_packets_.front().data(), write_packets_.front().length()),
			[this](boost::system::error_code ec, std::size_t /* length? */) {
				if (!ec) {
					write_packets_.pop_front();
					if (!write_packets_.empty()) write();
				}
				else socket_.close();
			});
	}
	boost::asio::io_context& io_context_;
	tcp::socket socket_;
	NetworkPacket read_packet_;
	packet_queue write_packets_;
	uint32_t id;
};