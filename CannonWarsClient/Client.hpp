#pragma once
class Client
{
public:
	static void DoServiceLoop(const UDPSocketPtr client_socket, Player* new_player);
	static void PrintOptions();
	static void GetChoice(std::string& choice);
	static void SendDataToServer(UDPSocketPtr client_socket, char* input);
	static void ReceiveDataFromServer(UDPSocketPtr client_socket, char* receive_buffer, SocketAddress sender_address, int bytes_received, bool& service_running);
	static int ConvertIPToInt(std::string ip_string);
	static void ProcessReceivedData(char* receive_buffer, int bytes_received, SocketAddress sender_address, bool& service_running);
	static const int32_t kMaxPacketSize = 1300;
	static void SendPlayerOutputByteStream(UDPSocketPtr client_socket, Player* player);
	enum class Choice { ECHO = 1, DATEANDTIME, STATS, SENDPLAYERBYTE, QUIT };
};

