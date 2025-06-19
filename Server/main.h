#pragma once
void readyConnection(SOCKET clientSocket, char buffer[1024]);

void makeFileByConnection(char buffer[1024], SOCKET clientSocket);

void sendInfoToServer(std::string &host, std::string &diretorio, SOCKET sock);
