#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib") 

SOCKET CreateSocket();
void AcceptLoop(SOCKET sock);
void DoIt(SOCKET clientsocket);

using namespace std;

void main() {

    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2); 

    int wsOK = WSAStartup(ver, &wsData);

    if (wsOK != 0) {
        cerr << "Can't Initialize winsock! Quitting" << endl;
        return;
    }

    SOCKET sock = CreateSocket();
    AcceptLoop(sock);

    WSACleanup();
}

SOCKET CreateSocket() {
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); 
    if (listening == INVALID_SOCKET) {
        cerr << "Can't create a socket! Quitting" << endl;
        return -1;
    }

    SOCKADDR_IN hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000); 
    hint.sin_addr.S_un.S_addr = INADDR_ANY; 

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    listen(listening, SOMAXCONN);

    return listening;
}
    
void AcceptLoop(SOCKET sock) {
    // Wait for a connection
    SOCKADDR_IN client = { 0 };
    int clientSize = sizeof(client);
    while (true) {
        SOCKET clientSocket = accept(sock, (sockaddr*)&client, &clientSize);

        char host[NI_MAXHOST];  
        char service[NI_MAXHOST];

        ZeroMemory(host, NI_MAXHOST); 
        ZeroMemory(service, NI_MAXHOST);

        if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) { 
            cout << host << " connected on port " << service << endl;
        }
        else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            cout << host << " connected on port " << ntohs(client.sin_port) << endl;
        }
        DoIt(clientSocket);
    }
    closesocket(sock);
}

void DoIt(SOCKET clientSocket) {

    char buf[4096];

    while (true) {
        ZeroMemory(buf, 4096);

        int byteReceived = recv(clientSocket, buf, 4096, 0);
        if (byteReceived == SOCKET_ERROR) {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        if (byteReceived == 0) {
            cout << "Client disconnected " << endl;
            break;
        }
        
        if (byteReceived > 0) {
            cout << "Client> " << string(buf, 0, byteReceived) << endl;
        }
        
        send(clientSocket, buf, byteReceived + 1, 0);
    }

    closesocket(clientSocket);

}

   

