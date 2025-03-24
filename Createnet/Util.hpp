#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>
#include <filesystem>

uint32_t randomId() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
	return dist(gen);
}

std::string readFile(const char* path) {
	std::string contents;
	std::ifstream fStream;
	fStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		fStream.open(path);
		std::stringstream sStream;
		sStream << fStream.rdbuf();
		fStream.close();
		contents = sStream.str();
	}
	catch (std::ifstream::failure & e) {
		std::cout << "[I/O] couldn't read file \"" << path << "\": " << e.what() << std::endl;
		return nullptr;
	}
	return contents;
}

std::string getResource(const std::string& file, const std::string& url) {
	if (std::filesystem::exists(file)) {
		std::cout << "[RESOURCES] file \"" << file << "\" found on disk" << std::endl;
		return readFile(file.c_str());
	}
	std::cout << "[RESOURCES] file \"" << file << "\" not found, downloading..." << std::endl;
	std::string command = "curl -s -o " + file + " " + url;
	int success = std::system(command.c_str());
	if (success != 0) {
		std::cout << "[CURL] curl failed with code: " << success << std::endl;
		return "";
	}
	return readFile(file.c_str());
}