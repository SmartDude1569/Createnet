#pragma once

#include <cstdint>
#include <string>

#include "NetworkPacket.hpp"

class PacketReader {
public:
	PacketReader(const NetworkPacket& packet) : packet_(packet) {
		pos_ = NetworkPacket::header_length;
		data_.insert(data_.end(), packet_.data(), packet_.data() + packet_.length());
	}
	const NetworkPacket& packet() const {
		return packet_;
	}
	NetworkPacket& packet() {
		return packet_;
	}
	unsigned char readByte() {
		return data_[pos_++];
	}
	uint32_t readUint32() {
		uint32_t value;
		std::vector<unsigned char> bytes = std::vector<unsigned char>(data_.begin() + pos_, data_.begin() + pos_ + sizeof(uint32_t));
		memcpy(&value, bytes.data(), sizeof(uint32_t));
		pos_ += sizeof(uint32_t);
		return value;
	}
	int32_t readInt32() {
		int32_t value;
		std::vector<unsigned char> bytes = std::vector<unsigned char>(data_.begin() + pos_, data_.begin() + pos_ + sizeof(int32_t));
		memcpy(&value, bytes.data(), sizeof(int32_t));
		pos_ += sizeof(int32_t);
		return value;
	}
	uint64_t readUint64() {
		uint64_t value;
		std::vector<unsigned char> bytes = std::vector<unsigned char>(data_.begin() + pos_, data_.begin() + pos_ + sizeof(uint64_t));
		memcpy(&value, bytes.data(), sizeof(uint64_t));
		pos_ += sizeof(uint64_t);
		return value;
	}
	int64_t readInt64() {
		int64_t value;
		std::vector<unsigned char> bytes = std::vector<unsigned char>(data_.begin() + pos_, data_.begin() + pos_ + sizeof(int64_t));
		memcpy(&value, bytes.data(), sizeof(int64_t));
		pos_ += sizeof(int64_t);
		return value;
	}
	float readFloat() {
		float value;
		std::vector<unsigned char> bytes = std::vector<unsigned char>(data_.begin() + pos_, data_.begin() + pos_ + sizeof(float));
		memcpy(&value, bytes.data(), sizeof(float));
		pos_ += sizeof(float);
		return value;
	}
	double readDouble() {
		double value;
		std::vector<unsigned char> bytes = std::vector<unsigned char>(data_.begin() + pos_, data_.begin() + pos_ + sizeof(double));
		memcpy(&value, bytes.data(), sizeof(double));
		pos_ += sizeof(double);
		return value;
	}
	std::string readString() {
		std::size_t length = packet_.length() - pos_;
		if (length == 0) return "";
		std::string value(data_.begin() + pos_, data_.begin() + pos_ + length);
		pos_ += length;
		return value;
	}
private:
	bool checkIdx(std::size_t idx) {
		return (idx < packet_.length());
	}
	NetworkPacket packet_;
	std::vector<unsigned char> data_;
	std::size_t pos_;
};