


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"
typedef int socklen_t;
//typedef char* receiveBufer_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
//typedef void* receiveBufer_t;
typedef int SOCKET;
const int NO_ERROR = 0;
const int INVALID_SOCKET = -1;
const int WSAECONNRESET = ECONNRESET;
const int WSAEWOULDBLOCK = EAGAIN;
const int SOCKET_ERROR = -1;
#endif


#include "memory"

#include "vector"
#include "unordered_map"
#include "string"
#include "list"
#include "queue"
#include "deque"
#include "unordered_set"
#include "cassert"
#include <fstream>

using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::queue;
using std::list;
using std::deque;
using std::unordered_map;
using std::string;
using std::unordered_set;
using std::ifstream;

class Ship;
class GameObject;
class OutputMemoryStream;
class InputMemoryStream;


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>



#include "Math.hpp"

#include "StringUtils.hpp"
#include "SocketAddress.hpp"
#include "SocketAddressFactory.hpp"
#include "UDPSocket.hpp"
#include "TCPSocket.hpp"
#include "SocketUtil.hpp"

#include "ByteSwap.hpp"
#include "LinkingContext.hpp"
#include "MemoryBitStream.hpp"

#include "TransmissionData.hpp"
#include "InFlightPacket.hpp"
#include "AckRange.hpp"
#include "DeliveryNotificationManager.hpp"

#include "InputAction.h"
#include "InputState.hpp"
#include "Move.h"
#include "MoveList.h"

#include "GameObject.hpp"
#include "GameObjectRegistry.hpp"
#include "Ship.hpp"
#include "World.hpp"
#include "Timing.hpp"
#include "Mouse.h"
#include "Yarn.h"
#include "Mountain.h"

#include "StringUtils.hpp"
#include "ScoreBoardManager.h"

#include "SoundManager.h"

#include "WeightedTimedMovingAverage.h"
#include "ReplicationCommand.hpp"
#include "NetworkManager.h"
#include "Engine.hpp"

#include "Trigonometry.hpp"
#include "VectorAlgebra2D.hpp"
//#include "Line.hpp"
//#include "ShadowFactory.hpp"
#include "ConnectionDetails.hpp"


#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
