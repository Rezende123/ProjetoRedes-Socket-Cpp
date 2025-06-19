#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <unistd.h>
#include <windows.h>
#include <filesystem>
#include <chrono>
#include <cstring>

#pragma execution_character_set("utf-8")

#pragma comment(lib, "Ws2_32.lib")

#define PORT 4200
#define END_CONNECTION "::bye::"

std::string readFilesDir(std::string dir)
{
    std::string filesDir = "";

    if (!std::filesystem::exists(dir))
        return "";

    for (const auto &entry : std::filesystem::directory_iterator(dir))
        filesDir += entry.path().string() + ",";

    return filesDir;
}

double calcThroughput(std::chrono::duration<double> elapsed, std::string command)
{
    // Calcula throughput
    double bytesSent = command.size();
    double throughput = bytesSent / elapsed.count(); // em bytes/segundo
    return throughput;
}

void sendSocketMessage(SOCKET clientSocket, const char * message)
{
    auto start = std::chrono::high_resolution_clock::now();

    send(clientSocket, message, strlen(message), 0);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "+-------------------------------------------------------+" << std::endl;
    std::cout << "Mensagem enviada ao servidor: " << message << std::endl;
    std::cout << "Throughput: " << calcThroughput(elapsed, message) << " B/s" << std::endl;
    std::cout << "+-------------------------------------------------------+\n" << std::endl;
}

std::string recvSocketMessage(SOCKET clientSocket)
{
    auto start = std::chrono::high_resolution_clock::now();

    char buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "+-------------------------------------------------------+" << std::endl;
    std::cout << "Resposta do servidor: " << buffer << std::endl;
    std::cout << "Throughput: " << calcThroughput(elapsed, buffer) << " B/s" << std::endl;
    std::cout << "+-------------------------------------------------------+\n" << std::endl;

    return buffer;
}

void readyConnection(SOCKET clientSocket)
{
    const char *message = "READY";
    std::string response = "";

    do
    {
        sendSocketMessage(clientSocket, message);

        response = recvSocketMessage(clientSocket);
    } while (strcmp(response.c_str(), "READY ACK") != 0);
}

void sendInfo(std::string &host, std::string &dir, SOCKET sock)
{

    std::string server_command = host + " " + dir + " " + readFilesDir(dir);
    sendSocketMessage(sock, server_command.c_str());

    const char *message = END_CONNECTION;

    sendSocketMessage(sock, message);
}

bool createSockaddr(std::string ip, int port, sockaddr_storage& addr) {
    socklen_t addrLen;

    std::memset(&addr, 0, sizeof(addr));

    if (ip.find(':') != std::string::npos) {
        // IPv6
        sockaddr_in6* addr6 = (sockaddr_in6*)&addr;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(port);
        if (inet_pton(AF_INET6, ip.c_str(), &addr6->sin6_addr) != 1)
            return false;
        addrLen = sizeof(sockaddr_in6);
        std::cout << "IPv6 " << ip << ":"<< port << "\n" << std::endl;
    } else {
        // IPv4
        sockaddr_in* addr4 = (sockaddr_in*)&addr;
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(port);
        if (inet_pton(AF_INET, ip.c_str(), &addr4->sin_addr) != 1)
            return false;
        addrLen = sizeof(sockaddr_in);
        std::cout << "IPv4 " << ip << ":"<< port << "\n" << std::endl;
    }

    return true;
}

int server(std::string comando, std::string host, std::string port, std::string dir)
{
    WSADATA wsaData;
    SOCKET sock;
    sockaddr_storage serverAddr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup falhou.\n";
        return 1;
    }

    int family = (host.find(':') != std::string::npos) ? AF_INET6 : AF_INET;
    sock = socket(family, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Erro ao criar socket.\n";
        WSACleanup();
        return 1;
    }

    createSockaddr(host, std::atoi(port.c_str()), serverAddr);
    
    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Falha na conexão com o servidor.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();

    readyConnection(sock);

    sendInfo(host, dir, sock);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << "Tempo total de transmissão: " << elapsed.count() << " s\n" << std::endl;

    closesocket(sock);
    WSACleanup();

    return 0;
}

int main()
{
    SetConsoleOutputCP(65001); // CP_UTF8
    std::cout << R"(
    
    [CLIENTE]  
    +---------------------+
    |  Conectando...      |
    |  Porta: 4200        |
    +---------------------+
        ^_____^ 
         o   o  
     ( ==  ^  == ) 
      )         ( 
     (           ) 
    ( (  )   (  ) ) 
   (__(__)___(__)__)

)" << std::endl;

    std::cout << "Insira as informações para se conectar ao servidor:" << std::endl;
    std::cout << "<host do servidor> <port do servidor> <nome do diretório>" << std::endl;
    std::cout << "" << std::endl;

    std::string comando = "";

    std::getline(std::cin, comando);

    std::string host, port, dir;
    std::istringstream iss(comando);
    iss >> host >> port >> dir;

    std::cout << "+---------------------+" << std::endl;
    std::cout << "|  Host: " << host << std::endl;
    std::cout << "|  Porta: " << port << std::endl;
    std::cout << "|  Diretório: " << dir << std::endl;
    std::cout << "+---------------------+\n" << std::endl;

    server(comando, host, port, dir);
    return 0;
}