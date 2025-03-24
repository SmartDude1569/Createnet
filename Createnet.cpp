#include "Createnet.h"

int main(int argc, char* argv[]) {
	//try {
		std::string buildNumber = "v" + std::to_string(MAJOR_VERSION) + "." + std::to_string(MINOR_VERSION) + "." + std::to_string(PATCH_VERSION) + "." + std::to_string(BUILD_NUMBER) + " " + DEV_STAGE;
		cxxopts::Options options("Createnet", "Createnet client/server " + buildNumber);
		options.add_options()
			("help", "Show help message")
			("s,server", "Run in server mode")
			("p,port", "Port to use", cxxopts::value<std::string>()->default_value("56915"))
			("h,host", "Host to connect to", cxxopts::value<std::string>()->default_value("localhost"));
		auto result = options.parse(argc, argv);
		if (result.count("help")) {
			std::cout << options.help() << std::endl;
			return 0;
		}
		if (result.count("server")) {
			std::cout << "running as a SERVER on port " << result["port"].as<std::string>().c_str() << std::endl;
			boost::asio::io_context io_context;
			std::list<NetworkServer> servers;
			tcp::endpoint endpoint(tcp::v4(), std::atoi(result["port"].as<std::string>().c_str()));
			servers.emplace_back(io_context, endpoint);
			io_context.run();
		}
		else {
			std::cout << "running as a CLIENT, connecting to " << result["host"].as<std::string>().c_str() << ":" << result["port"].as<std::string>().c_str() << std::endl;
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<uint32_t> dis;
			uint32_t myId = dis(gen);
			std::cout << "your ID is " << std::to_string(myId) << std::endl;
			boost::asio::io_context io_context;
			tcp::resolver resolver(io_context);
			auto endpoints = resolver.resolve(result["host"].as<std::string>().c_str(), result["port"].as<std::string>().c_str());
			NetworkClient c(io_context, endpoints, myId);
			std::thread t([&io_context]() { io_context.run(); });
			//char line[NetworkPacket::max_body_length + 1];
			//while (std::cin.getline(line, NetworkPacket::max_body_length + 1)) {
			//	PacketWriter* writer = new PacketWriter();
			//	writer->writeByte(136);
			//	writer->writeUint32(myId);
			//	writer->writeString(std::string(line));
			//	c.write(writer->packet());
			//}
			Client* client = new Client();
			while (!client->shouldClose()) {
				client->loop();
			}
			c.close();
			t.join();
		}
		return 0;
	//}
	//catch (std::exception& e) {
	//	std::cerr << "An exception occurred: " << e.what() << "\n";
	//}
}