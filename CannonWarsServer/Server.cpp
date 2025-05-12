#include "SocketWrapperPCH.hpp"

int main()
{
	SocketUtil::StaticInit();
	UDPSocketPtr server_socket = SocketUtil::CreateUDPSocket(INET);
	SocketAddress server_address = SocketAddress(Server::ConvertIPToInt("127.0.0.1"), 50005);
	server_socket->Bind(server_address);
	server_socket->SetNonBlockingMode(false);
	Server::ReceivePlayerInputByteStream(server_socket);
	//Server::DoServiceLoop(server_socket);
}

uint32_t Server::ConvertIPToInt(std::string ip_string)
{
	int int_ip = 0;
	for (int i = 0; i < ip_string.length(); ++i)
	{
		if (ip_string[i] == '.')
		{
			ip_string[i] = ' ';
		}
	}
	vector<int> array_tokens;
	std::stringstream ss(ip_string);
	int temp;
	while (ss >> temp)
	{
		array_tokens.emplace_back(temp);
	}

	for (int i = 0; i < array_tokens.size(); ++i)
	{
		int_ip += (array_tokens[i] << ((3 - i) * 8));
	}
	return int_ip;
}

void Server::DoServiceLoop(const UDPSocketPtr& server_socket)
{
	bool service_running = true;

	char receive_buffer[kMaxPacketSize];
	char* begin = receive_buffer;
	char* end = begin + sizeof(receive_buffer);
	std::fill(begin, end, 0);

	SocketAddress sender_address;
	int bytes_received = 0;
	int requests = 0;

	while (service_running)
	{
		bytes_received = server_socket->ReceiveFrom(receive_buffer, sizeof(receive_buffer), sender_address);
		if (bytes_received > 0)
		{
			requests++;
			ProcessReceivedData(receive_buffer, bytes_received, sender_address, server_socket, requests, service_running);
		}
	}
}

void Server::ProcessReceivedData(char* receive_buffer, int bytes_received, const SocketAddress& sender_address, const UDPSocketPtr& server_socket, int requests, bool service_running)
{
	std::cout << "Got " << bytes_received << " from " << sender_address.ToString() << std::endl;
	std::cout << "The message is: " << receive_buffer << std::endl;

	char response_data[kMaxPacketSize] = "";
	int choice = atoi(receive_buffer);
	string current_date_time = "";
	std::string requests_string = "";
	std::string quit_string = "QUIT";

	switch (static_cast<Choice>(choice))
	{
	case Choice::ECHO:
		std::cout << "ECHO request" << std::endl;
		strcpy_s(response_data, receive_buffer);
		break;
	case Choice::DATEANDTIME:
		std::cout << "DATEANDTIME request" << std::endl;
		current_date_time = ReturnCurrentDataAndTime();
		current_date_time.copy(response_data, current_date_time.length(), 0);
		break;
	case Choice::STATS:
		std::cout << "STATS request" << std::endl;
		requests_string = std::to_string(requests);
		requests_string.copy(response_data, requests_string.length(), 0);
		break;
	case Choice::QUIT:
		std::cout << "QUIT request" << std::endl;
		quit_string.copy(response_data, quit_string.length(), 0);
		service_running = false;
		break;
	}
	int bytes_sent = server_socket->SendTo(response_data, sizeof(response_data), sender_address);
}

std::string Server::ReturnCurrentDataAndTime()
{
	// 1) get current time
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);

	// 2) break out into a tm struct in a safe way
	std::tm timeinfo;
#if defined(_MSC_VER)
	localtime_s(&timeinfo, &t);          // MSVC “safe” variant
#else
	localtime_r(&t, &timeinfo);          // POSIX “safe” variant
#endif

	// 3) format it
	std::stringstream ss;
	ss << std::put_time(&timeinfo, "%Y-%m-%d %X");
	return ss.str();
}

void Server::ReceivePlayerInputByteStream(const UDPSocketPtr server_socket)
{
	Player* receiver = new Player();
	SocketAddress sender_address;

	char* temporary_buffer = static_cast<char*>(std::malloc(kMaxPacketSize));
	std::cout << "Waiting for input on the socket from the client" << std::endl;
	int bytes_received = server_socket->ReceiveFrom(temporary_buffer, kMaxPacketSize, sender_address);
	InputMemoryStream in_stream(temporary_buffer, static_cast<uint32_t>(bytes_received));
	receiver->Read(in_stream);
	std::cout << "Recevied: " << bytes_received;
	receiver->ToString();

}
