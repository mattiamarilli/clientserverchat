//Mattia Marilli 5BIA -Client-

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
using namespace std;

char szServerIPAddr[20] = "127.0.0.1";     // Indirizzo del server
int nServerPort = 5050;                    // Porta usata dai client per interagire con il server

bool InitWinSock2_0()
{
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 0);

	if (!WSAStartup(wVersion, &wsaData))
		return true;

	return false;
}

int main()
{
	cout << "Inserire l'indirizzo del server: ";
	cin >> szServerIPAddr;
	cout << "Inserisci la porta su cui è in ascolto il server: ";
	cin >> nServerPort;

	if (!InitWinSock2_0())
	{
		cout << "Impossibile inizializzare il socket :" << WSAGetLastError() << endl;
		return -1;
	}

	SOCKET hClientSocket;

	hClientSocket = socket(
		AF_INET,        // La famiglia dell'indirizzo . AF_INET indica il TCP/IP
		SOCK_STREAM,    // Tipo del protocollo. SOCK_STREM indica il TCP
		0               // Nome del protocollo.
	);
	if (hClientSocket == INVALID_SOCKET)
	{
		cout << "Impossibile creare il socket del server" << endl;
		WSACleanup();
		return -1;
	}


	// Creazione della struttura per indicare i parametri del server
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(szServerIPAddr);
	serverAddr.sin_port = htons(nServerPort);

	// Connessione al server
	if (connect(hClientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		cout << "Impossibile connettersi" << szServerIPAddr << " sulla porta " << nServerPort << endl;
		closesocket(hClientSocket);
		WSACleanup();
		return -1;
	}

	cout << "----------------------------------------------INIZIO CHAT-----------------------------------------------" << endl;
	char szBuffer[1024] = "";

	while (strcmp(szBuffer, "ESCI") != 0)
	{
		cout << "Inserire la stringa da inserire, ESCI per chiudere la chat: ";
		cin >> szBuffer;
		
		int nLength = strlen(szBuffer);

		// La funzione send() può non essere in grado di inviare i dati in una sola volta quindi prova in più richieste
		
		int nCntSend = 0;
		char *pBuffer = szBuffer;

		while ((nCntSend = send(hClientSocket, pBuffer, nLength, 0) != nLength))
		{
			if (nCntSend == -1)
			{
				cout << "Impossibile inviare i dati al server" << endl;
				break;
			}
			if (nCntSend == nLength)
				break;

			pBuffer += nCntSend;
			nLength -= nCntSend;
		}

		_strupr(szBuffer);
		if (strcmp(szBuffer, "ESCI") == 0)
		{
			break;
		}
		cout << "In attesa di risposta dal server..." << endl;
		nLength = recv(hClientSocket, szBuffer, sizeof(szBuffer), 0);
		if (nLength > 0)
		{
			szBuffer[nLength] = '\0';
			cout << "Server: " << szBuffer << endl;
		}
	}

	closesocket(hClientSocket);
	WSACleanup();
	system("pause");
	return 0;
}

