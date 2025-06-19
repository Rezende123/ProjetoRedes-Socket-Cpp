# Makefile para TP SOCKETS CLOUD - Windows (MinGW)

CXX = g++
CXXFLAGS = -g -Wall
LIBS = -lws2_32

# Caminhos
CLIENT_DIR = Client
SERVER_DIR = Server

CLIENT_SRC = $(CLIENT_DIR)/main.cpp
SERVER_SRC = $(SERVER_DIR)/main.cpp

CLIENT_EXE = $(CLIENT_DIR)/main.exe
SERVER_EXE = $(SERVER_DIR)/main.exe

# Targets

all: client server

client: $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_EXE) $(CLIENT_SRC) $(LIBS)

server: $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) -o $(SERVER_EXE) $(SERVER_SRC) $(LIBS)

run-client: client
	cd $(CLIENT_DIR) && main.exe

run-server: server
	cd $(SERVER_DIR) && main.exe

clean:
	del /Q $(CLIENT_EXE) $(SERVER_EXE)

.PHONY: all client server run-client run-server clean
