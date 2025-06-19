#include <iostream>
#include <sstream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <fstream>
#include <regex>
#include <sys/stat.h>  // Para mkdir()
#include <sys/types.h> // Para mkdir()
#include <chrono>
#include "main.h"

#pragma execution_character_set("utf-8")
#pragma comment(lib, "Ws2_32.lib")

#define PORTA 8080
#define END_CONNECTION "::bye::"

int makeFile(std::string filename, std::string content)
{
    // Formata o nome do arquivo, removendo caracteres especiais
    filename = std::regex_replace(filename, std::regex(R"([./:\\])"), "") + ".txt";

    std::cout << "Salvando arquivo " << filename << "\n" << std::endl;

    std::ofstream MyFile(filename);

    if (MyFile.is_open())
    {
        // Write content to the file
        MyFile << content << std::endl;

        // Close the file
        MyFile.close();
        std::cout << "Arquivo salvo com sucesso.\n" << std::endl;
    }
    else
    {
        // Print an error message if the file could not be opened
        std::cerr << "Error: Não foi possível criar ou alterar o arquivo.\n" << std::endl;
    }

    return 0;
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
    std::cout << "Mensagem enviada ao cliente: " << message << std::endl;
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
    std::cout << "Resposta do cliente: " << buffer << std::endl;
    std::cout << "Throughput: " << calcThroughput(elapsed, buffer) << " B/s" << std::endl;
    std::cout << "+-------------------------------------------------------+\n" << std::endl;

    return buffer;
}

void makeFileByConnection(SOCKET clientSocket)
{
    std::string host, dir, files;
    std::string response = "";

    do {
        response = recvSocketMessage(clientSocket);

        std::istringstream iss(response);
        iss >> host >> dir >> files;

        if (!files.empty() && strcmp(response.c_str(), END_CONNECTION) != 0)
        {
            makeFile(host + dir, files);
        }
    } while (strcmp(response.c_str(), END_CONNECTION) != 0);    
}

void readyConnection(SOCKET clientSocket)
{
    std::string message = "NOT FOUND";
    std::string response = "";

    do
    {
        response = recvSocketMessage(clientSocket);
    } while (strcmp(response.c_str(), "READY") != 0);

    message = (char *)"READY ACK";

    sendSocketMessage(clientSocket, message.c_str());
}

int server()
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_storage clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    // Inicializa o Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup falhou.\n";
        return 1;
    }

    serverSocket = socket(AF_INET6, SOCK_STREAM, 0);
    int no = 0;
    setsockopt(serverSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&no, sizeof(no));
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Erro ao criar socket.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in6 serverAddr{};
    serverAddr.sin6_family = AF_INET6;
    serverAddr.sin6_port = htons(PORTA);
    serverAddr.sin6_addr = in6addr_any;

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Erro no bind.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Erro no listen.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Servidor aguardando conexão na porta " << PORTA << "...\n";

    clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Erro no accept.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();

    readyConnection(clientSocket);

    makeFileByConnection(clientSocket);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << "Tempo total de transmissão: " << elapsed.count() << " s\n" << std::endl;

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    std::cout << "Servidor desligado" << std::endl;

    return 0;
}

int main()
{
    SetConsoleOutputCP(65001); // CP_UTF8
    std::cout << R"(

    [SERVIDOR]  
    +---------------------+
    |  Online             |
    |  Porta: 8080        |
    +---------------------+
    |                     |
    |     SERVER CLOUD    |
    |    #############    |
    |    #############    |
    |_____________________|

)" << std::endl;

    server();

    return 0;
}