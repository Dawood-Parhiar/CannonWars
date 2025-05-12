#include "SocketWrapperPCH.hpp"

/* Client needs to offer the user a menu where they can select
1. Echo
2. DateAndTime
3. Stats
4. Quit
Take input from the user, send it to the server and listen and display the result
*/

int main()
{
	Player* new_player = new Player();
	std::cout << sizeof(new_player) << std::endl;
	SocketUtil::StaticInit();
	UDPSocketPtr client_socket = SocketUtil::CreateUDPSocket(INET);
	client_socket->SetNonBlockingMode(false); //Should block
	//Client::DoServiceLoop(client_socket, new_player);
	Client::SendPlayerOutputByteStream(client_socket, new_player);
	system("pause");
}

void Client::DoServiceLoop(const UDPSocketPtr client_socket, Player* new_player)
{
	bool service_running = true;
	string choice;
	char receive_buffer[kMaxPacketSize];
	char* begin = receive_buffer;
	char* end = begin + sizeof(receive_buffer);
	std::fill(begin, end, 0);
	SocketAddress sender_address;
	int bytes_received = 0;

	while (service_running)
	{
		PrintOptions();
		GetChoice(choice);
		switch (static_cast<Choice>(stoi(choice)))
		{
		case Choice::SENDPLAYERBYTE:
			SendPlayerOutputByteStream(client_socket, new_player);
			break;
		default:
			SendDataToServer(client_socket, (char*)choice.c_str());
		}
		ReceiveDataFromServer(client_socket, receive_buffer, sender_address, bytes_received, service_running);
	}


}

void Client::PrintOptions()
{
	std::cout << "Please enter: " << std::endl;
	std::cout << "1) To use the ECHO service: " << std::endl;
	std::cout << "2) To use the DATEANDTIME service: " << std::endl;
	std::cout << "3) To use the STATS service: " << std::endl;
	std::cout << "4) To send the player using the Byte Stream: " << std::endl;
	std::cout << "5) To QUIT" << std::endl;

}

void Client::GetChoice(std::string& choice)
{
	std::getline(std::cin, choice);
}

void Client::SendDataToServer(UDPSocketPtr client_socket, char* input)
{
	SocketAddress server_address = SocketAddress(Client::ConvertIPToInt("127.0.0.1"), 50005);
	int bytes_send = client_socket->SendTo(input, sizeof(input), server_address);
}

void Client::ReceiveDataFromServer(UDPSocketPtr client_socket, char* receive_buffer, SocketAddress sender_address, int bytes_received, bool& service_running)
{
	std::cout << sizeof(receive_buffer) << std::endl;
	bytes_received = client_socket->ReceiveFrom(receive_buffer, kMaxPacketSize, sender_address);
	if (bytes_received > 0)
	{
		ProcessReceivedData(receive_buffer, bytes_received, sender_address, service_running);
	}
}

int Client::ConvertIPToInt(std::string ip_string)
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

void Client::ProcessReceivedData(char* receive_buffer, int bytes_received, SocketAddress sender_address, bool& service_running)
{
	char key[] = "QUIT";

	if (strcmp(key, receive_buffer) == 0)
	{
		std::cout << "Server says we need to shut down...." << std::endl;
		service_running = false;
	}

	std::cout << "Got " << bytes_received << " from " << sender_address.ToString() << std::endl;
	std::cout << "The message is: " << receive_buffer << std::endl;
}

void Client::SendPlayerOutputByteStream(UDPSocketPtr client_socket, Player* player)
{
	SocketAddress server_address = SocketAddress(ConvertIPToInt("127.0.0.1"), 50005);
	OutputMemoryStream out_stream;
	player->Write(out_stream);
	int bytes_sent = client_socket->SendTo(out_stream.GetBufferPtr(), out_stream.GetLength(), server_address);
	std::cout << "Sent: " << bytes_sent << std::endl;
}
