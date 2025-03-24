#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "NetworkPacket.hpp"

class PacketWriter {
public:
	PacketWriter() {
		data_.clear();
	}
	PacketWriter(const NetworkPacket& packet) : packet_(packet) {
		data_.insert(data_.end(), packet_.data(), packet_.data() + packet_.length());
	}
	NetworkPacket& packet() {
		writeToPacket();
		return packet_;
	}
	void writeByte(unsigned char value) {
		data_.insert(data_.end(), value);
		writeToPacket();
	}
	void writeUint32(uint32_t value) {
		std::vector<unsigned char> bytes(sizeof(value));
		memcpy(bytes.data(), &value, sizeof(value));
		data_.insert(data_.end(), bytes.begin(), bytes.end());
		writeToPacket();
	}
	void writeInt32(int32_t value) {
		std::vector<unsigned char> bytes(sizeof(value));
		memcpy(bytes.data(), &value, sizeof(value));
		data_.insert(data_.end(), bytes.begin(), bytes.end());
		writeToPacket();
	}
	void writeUint64(uint64_t value) {
		std::vector<unsigned char> bytes(sizeof(value));
		memcpy(bytes.data(), &value, sizeof(value));
		data_.insert(data_.end(), bytes.begin(), bytes.end());
		writeToPacket();
	}
	void writeInt64(int64_t value) {
		std::vector<unsigned char> bytes(sizeof(value));
		memcpy(bytes.data(), &value, sizeof(value));
		data_.insert(data_.end(), bytes.begin(), bytes.end());
		writeToPacket();
	}
	void writeFloat(float value) {
		std::vector<unsigned char> bytes(sizeof(value));
		memcpy(bytes.data(), &value, sizeof(value));
		data_.insert(data_.end(), bytes.begin(), bytes.end());
		writeToPacket();
	}
	void writeDouble(double value) {
		std::vector<unsigned char> bytes(sizeof(value));
		memcpy(bytes.data(), &value, sizeof(value));
		data_.insert(data_.end(), bytes.begin(), bytes.end());
		writeToPacket();
	}
	void writeString(const std::string& value) {
		data_.insert(data_.end(), value.begin(), value.end());
		writeToPacket();
	}
private:
	bool checkIdx(std::size_t idx) {
		if (idx > NetworkPacket::header_length + NetworkPacket::max_body_length) {
			return false;
		}
		return true;
	}
	void writeToPacket() {
		packet_.body_length(data_.size());
		std::memcpy(packet_.body(), data_.data(), packet_.body_length());
		packet_.encode_header();
	}
	NetworkPacket packet_;
	std::vector<unsigned char> data_;
};