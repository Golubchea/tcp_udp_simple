
#include <tcp_client/tcp_client.h>

#include <stdlib.h>


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
// Директива линковщику: использовать библиотеку сокетов
#pragma comment(lib, "ws2_32.lib")
#else // LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include <errno.h>

#include <arpa/inet.h>
#include <string.h>
#endif
#include <stdio.h>




tcp_client * tcp_client_new()
{
    //выделение памяти под структуру
    tcp_client * cli=(tcp_client *)malloc(sizeof (tcp_client));

#ifdef _WIN32
    // Для Windows следует вызвать WSAStartup перед началом использования сокетов
    WSADATA wsa_data;
    return (0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));
#else
    // Для других ОС действий не требуется
#endif
    return cli;

}


void tcp_client_delete(tcp_client *cli)
{
#ifdef _WIN32
    // Для Windows следует вызвать WSACleanup в конце работы
    WSACleanup();
#else
// Для других ОС действий не требуется
#endif

    free(cli);
}



//закрыть сокет (int s - id сокета)
void tcp_client_s_close(int s)
{
#ifdef _WIN32
    closesocket(s);
#else

    close(s);
#endif
}

int tcp_client_sock_err(const char* function, int s)
{
    int err;
#ifdef _WIN32
    err = WSAGetLastError();
#elseerr = errno;
#endif
    fprintf(stderr, "%s: socket error: %d\n", function, err);
    return -1;
}

int tcp_client_send_request(void* request, int s, int size)
{


    int sent = 0;
#ifdef _WIN32
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif
    while (sent < size)
    {

        // Отправка очередного блока данных
        int res = send(s, (unsigned char *)request + sent, size - sent, flags);
        usleep(5000);
        if (res < 0)
            return tcp_client_sock_err("send", s);sent += res;
        printf(" %d bytes sent.\n", sent);
    }
    return 0;
}



int tcp_client_recv_response(int s, FILE* f)
{
    char buffer[1024];
    int res;
    usleep(5000);
    // Принятие очередного блока данных.
    // Если соединение будет разорвано удаленным узлом recv вернет 0
    while ((res = recv(s, buffer, sizeof(buffer), 0)) > 0)
    {
        fwrite(buffer, 1, strlen(buffer), f);
        printf(" %d bytes received:message:%s \n", res,buffer);

    }
    return res;

    if (res < 0)
        return tcp_client_sock_err("recv", s);

}


int tcp_client_recv_simple(int s, FILE* f)
{
    char buffer[2];

    int res;
    res = recv(s, buffer, sizeof(buffer), 0)   ;
    fwrite(buffer, 1, strlen(buffer), f);
    printf(" %d bytes received:message:%s \n", res,buffer);
    return res;
    if (res < 0)
        return tcp_client_sock_err("recv", s);
    return 0;
}







