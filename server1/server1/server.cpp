//Mattia Marilli 5BIA -Server-

#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
using namespace std;
struct CLIENT_INFO
{
	SOCKET hClientSocket;
	struct sockaddr_in clientAddr;
};

char szServerIPAddr[] = "10.0.2.15";     // Inserire l'indirizzo del server
int nServerPort = 5050;                  // Porta usata dai client per interagire con il server

bool InitWinSock2_0()
{
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 0);

	if (!WSAStartup(wVersion, &wsaData))
		return true;

	return false;
}

bool receivemessage(CLIENT_INFO *clientInfo)
{
	char szBuffer[1024];
	int nLength;
	char message[1024];

	while (1)
	{
		nLength = recv(clientInfo->hClientSocket, szBuffer, sizeof(szBuffer), 0);
		if (nLength > 0)
		{
			szBuffer[nLength] = '\0';
			cout << "Client: " << szBuffer << endl; 
			// Converte la stringa in maiuscolo, se non è ESCI
			_strupr(szBuffer);
			if (strcmp(szBuffer, "ESCI") == 0)
			{
				closesocket(clientInfo->hClientSocket);
				return FALSE;
			}
			cout << "Inserisci il messaggio da inviare al client : ";
			cin >> message;
			// La funzione send() può non essere in grado di inviare i dati in una sola volta quindi prova in più richieste
			int nCntSend = 0;
			char *pBuffer = szBuffer;
			char *pMessage = message;

			while ((nCntSend = send(clientInfo->hClientSocket, pMessage, nLength, 0) != nLength))
			{
				if (nCntSend == -1)
				{
					cout << "Impossibile inviare i dati a " << inet_ntoa(clientInfo->clientAddr.sin_addr) << endl;
					break;
				}
				if (nCntSend == nLength)
					break;

				pMessage += nCntSend;
				nLength -= nCntSend;
			}

			cout << "In attesa di risposta dal client..." << endl;
		}
		else
		{
			cout << "Impossibile leggere i dati da " << inet_ntoa(clientInfo->clientAddr.sin_addr) << endl;
		}
	}

	return TRUE;
}

int main()
{
	if (!InitWinSock2_0())
	{
		cout << "Impossibile inizializzare il socket : " << WSAGetLastError() << endl;
		return -1;
	}

	SOCKET hServerSocket;

	hServerSocket = socket(
		AF_INET,        // La famiglia dell'indirizzo . AF_INET indica il TCP/IP
		SOCK_STREAM,    // Tipo del protocollo. SOCK_STREM indica il TCP
		0              // Nome del protocollo.
	);
	if (hServerSocket == INVALID_SOCKET)
	{
		cout << "Impossibile creare il socket del server" << endl;
		WSACleanup();
		return -1;
	}

	// Creazione della struttura per indicare i parametri del server
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;     // The address family. MUST be AF_INET
	serverAddr.sin_addr.s_addr = inet_addr(szServerIPAddr);
	serverAddr.sin_port = htons(nServerPort);

	// Bind del server del socket
	if (bind(hServerSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cout << "Impossibile effettuare il bind " << szServerIPAddr << " porta  " << nServerPort << endl;
		closesocket(hServerSocket);
		WSACleanup();
		return -1;
	}

	// Mette in ascolto il server
	if (listen(hServerSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "Impossibile mettere il server in stato di ascolto" << endl;
		closesocket(hServerSocket);
		WSACleanup();
		return -1;
	}

	// Inizia il loop infinito
	while (true)
	{
		SOCKET hClientSocket;
		struct sockaddr_in clientAddr;
		int nSize = sizeof(clientAddr);

		hClientSocket = accept(hServerSocket, (struct sockaddr *) &clientAddr, &nSize);
		if (hClientSocket == INVALID_SOCKET)
		{
			cout << "accept( ) fallita" << endl;
		}
		else
		{

			struct CLIENT_INFO clientInfo;


			clientInfo.clientAddr = clientAddr;
			clientInfo.hClientSocket = hClientSocket;

			cout << "Client connesso dall'indirizzo " << inet_ntoa(clientAddr.sin_addr) << endl;
			cout << "----------------------------------------------INIZIO CHAT----------------------------------------------" << endl;
			receivemessage(&clientInfo);

		}
	}

	closesocket(hServerSocket);
	WSACleanup();
	return 0;
}