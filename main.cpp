#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>

#pragma comment(lib, "WSock32.lib")

//終了コマンド用文字列
static const char* endCmdText = "cmd:end";

void UpdateServer() {

	//ポート番号設定
	u_short uport;
	printf("ポート番号を入れてください\n");
	scanf_s("%hd", &uport);
	//改行文字まで読み飛ばし
	while (getchar() != '\n');

	//ソケット作成
	SOCKET varSocket;

	//リッスンソケット作成
	SOCKET listenSocket;
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN varSockaddr;
	memset(&varSockaddr, 0, sizeof(SOCKADDR_IN));
	varSockaddr.sin_family = AF_INET;
	varSockaddr.sin_port = htons(uport);
	varSockaddr.sin_addr.s_addr = INADDR_ANY;

	SOCKADDR_IN fromSockaddr;
	int fromLen;

	if (bind(listenSocket, reinterpret_cast<SOCKADDR*>(&varSockaddr), sizeof(SOCKADDR)) == 0) {

		printf("bindが成功しました\n");

	}
	else {
		//失敗したら実行終了
		printf("bindが失敗しました\n");
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	//接続待ちの状態にする
	if (listen(listenSocket, 0) == 0) {

		printf("listenが成功しました\n");

	}
	else {
		//失敗したら実行終了
		printf("listenが失敗しました\n");
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	//接続待機
	printf("acceptで待機中\n");

	fromLen = (int)sizeof(fromSockaddr);

	varSocket = accept(listenSocket, reinterpret_cast<SOCKADDR*>(&fromSockaddr), &fromLen);

	if (varSocket == INVALID_SOCKET) {
		printf("acceptで失敗しました\n");
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	printf("%sが接続してきました\n", inet_ntoa(fromSockaddr.sin_addr));
	printf("acceptが成功しました\n");

	//接続できたのでリッスンソケットは不要
	closesocket(listenSocket);

	//会話開始
	printf("会話を開始します。<cmd:end>を入力で終了\n");

	while (true)
	{

		int nRcv;
		char szBuf[1024];

		//終端文字が必要なので、サイズを最大数 - 1にしておく
		nRcv = recv(varSocket, szBuf, sizeof(szBuf) - 1, 0);

		szBuf[nRcv] = '\0';

		if (strcmp(szBuf, endCmdText) == 0) {
			//終了コマンドが打たれていたら
			printf("終了コマンドを受け取りました。会話を終了します\n");
			break;
		}

		printf("受信\n%s\n", szBuf);
		printf("送信\n");

		scanf_s("%s", szBuf, 1024);
		//改行文字まで読み飛ばし
		while (getchar() != '\n');

		send(varSocket, szBuf, (int)strlen(szBuf), 0);

		if (strcmp(szBuf, endCmdText) == 0) {
			//終了コマンドが打たれていたら
			printf("終了コマンドを受け取りました。会話を終了します\n");
			break;
		}

	}

	closesocket(varSocket);
	WSACleanup();

}

void UpdateClient() {

	SOCKET varSocket;
	SOCKADDR_IN varSockaddr;
	u_short uport;
	char szServer[1024] = { 0 };
	unsigned int addr;

	//ポート番号入力
	printf("ポート番号を入れてください\n");
	scanf_s("%hd", &uport);
	//改行文字まで読み飛ばし
	while (getchar() != '\n');

	//サーバー名またはサーバーのIPアドレス入力
	printf("IPアドレスを入れてください\n");
	scanf_s("%s", szServer, 1024);
	//改行文字まで読み飛ばし
	while (getchar() != '\n');

	//ソケットを開く
	varSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (varSocket == INVALID_SOCKET) {
		printf("ソケットを開くのに失敗しました\n");
		return;
	}

	//サーバーを名前で取得する
	HOSTENT* lpHost;

	lpHost = gethostbyname(szServer);

	if (lpHost == NULL) {
		//サーバーをIPアドレスで取得
		addr = inet_addr(szServer);
		lpHost = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	//クライアントソケットをサーバーに接続
	memset(&varSockaddr, 0, sizeof(SOCKADDR_IN));
	varSockaddr.sin_family = lpHost->h_addrtype;
	varSockaddr.sin_port = htons(uport);
	varSockaddr.sin_addr.s_addr = *((u_long*)lpHost->h_addr);

	if (connect(varSocket, (SOCKADDR*)&varSockaddr, sizeof(varSockaddr)) != SOCKET_ERROR) {

		printf("サーバーとの接続に成功しました\n");

	}
	else {

		//失敗した場合
		printf("サーバーと接続できませんでした\n");
		closesocket(varSocket);
		return;

	}

	//会話開始
	printf("会話を開始します。<cmd:end>を入力で終了\n");

	while (true)
	{

		int nRcv;
		char szBuf[1024];

		printf("送信\n");
		scanf_s("%s", szBuf, 1024);
		//改行文字まで読み飛ばし
		while (getchar() != '\n');

		send(varSocket, szBuf, (int)strlen(szBuf), 0);

		if (strcmp(szBuf, endCmdText) == 0) {
			//終了コマンドが打たれていたら
			printf("終了コマンドを受け取りました。会話を終了します\n");
			break;
		}

		nRcv = recv(varSocket, szBuf, sizeof(szBuf) - 1, 0);

		szBuf[nRcv] = '\0';

		if (strcmp(szBuf, endCmdText) == 0) {
			//終了コマンドが打たれていたら
			printf("終了コマンドを受け取りました。会話を終了します\n");
			break;
		}

		printf("受信\n%s\n", szBuf);

	}

	closesocket(varSocket);
	WSACleanup();

}

int main() {

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {

		printf("WinSockの初期化に失敗しました\n");
		return 1;

	}

	printf("サーバーは0、クライアントは1を入力\n");

	int mode = -1;

	//初期設定の入力受付
	while (true)
	{

		scanf_s("%d", &mode);
		//入力クリア
		while (getchar() != '\n');

		if (mode == 0 or mode == 1) {
			break;
		}

		printf("正しい値を入力してください\n");

	}

	if (mode == 0) {
		UpdateServer();
	}
	else if (mode == 1) {
		UpdateClient();
	}

	WSACleanup();

	return 0;

}
