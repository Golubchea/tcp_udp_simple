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
#include <netdb.h>
#include <errno.h>
#endif
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

//#include <data/data.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else // LINUX
#include <sys/poll.h>
#include <fcntl.h>
#endif
int set_non_block_mode(int s)
{
#ifdef _WIN32
    unsigned long mode = 1;
    return ioctlsocket(s, FIONBIO, &mode);
#else
    int fl = fcntl(s, F_GETFL, 0);
    return fcntl(s, F_SETFL, fl | O_NONBLOCK);
#endif
}










//int listen_events()
//{


//    int ls; // Сокет, прослушивающий соединения
//    int cs[N]; // Сокеты с подключенными клиентами
//    struct pollfd pfd[N+1];
//    int i;
//    // В отличие от select, массив pfd не обязательно заполнять перед каждым вызовом poll
//    for (i = 0; i < N; i++)
//    {pfd[i].fd = cs[i];
//        pfd[i].events = POLLIN | POLLOUT;
//    }
//    pfd[N].fd = ls;
//    pfd[N].events = POLLIN;
//    while (1)
//    {
// Ожидание событий в течение 1 сек
//#ifdef _WIN32
//        int ev_cnt = WSAPoll(pfd, sizeof(pfd) / sizeof(pfd[0]), 1000);
//#else
//        int ev_cnt = poll(pfd, sizeof(pfd) / sizeof(pfd[0]), 1000)
//#endif
//        if (ev_cnt > 0)
//        {
//            for (i = 0; i < N; i++)
//            {
//                if (pfd[i].revents & POLLHUP)
//                {
//                    // Сокет cs[i] - клиент отключился. Можно закрывать сокет
//                }
//                if (pfd[i].revents & POLLERR)
//                {
//                    // Сокет cs[i] - возникла ошибка. Можно закрывать сокет
//                }
//                if (pfd[i].revents & POLLIN)
//                {
//                    // Сокет cs[i] доступен на чтение, можно вызывать recv/recvfrom
//                }
//                if (pfd[i].revents & POLLOUT)
//                {
//                    // Сокет cs[i] доступен на запись, можно вызывать send/sendto
//                }
//            }
//            if (pfd[N].revents & POLLIN)
//            {
//                // Сокет ls доступен на чтение - можно вызывать accept, принимать
//                // новое подключение. Новый сокет следует добавить в cs и создать для
//                // него структуру в pfd.
//            }
//        }
//        else
//        {
//            // Тайтмаут или ошибка
//        };

//    }
//}












//#define WEBHOST "google.com"
int init()
{
#ifdef _WIN32
    // Для Windows следует вызвать WSAStartup перед началом использования сокетов
    WSADATA wsa_data;
    return (0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));
#else
    return 1; // Для других ОС действий не требуется
#endif
}
void deinit()
{
#ifdef _WIN32
    // Для Windows следует вызвать WSACleanup в конце работы
    WSACleanup();
#else
// Для других ОС действий не требуется
#endif
}
int sock_err(const char* function, int s)
{
    int err;
#ifdef _WIN32
    err = WSAGetLastError();
#elseerr = errno;
#endif
    fprintf(stderr, "%s: socket error: %d\n", function, err);
    return -1;
}
void s_close(int s)
{
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}
int main()
{
    int s;
    struct sockaddr_in addr;
    int i;
#ifdef _WIN32
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif
    // Инициалиазация сетевой библиотеки
    init();
    // Создание UDP-сокета
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
        return sock_err("socket", s);
    // Заполнение структуры с адресом прослушивания узла
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000); // Будет прослушиваться порт 8000
    addr.sin_addr.s_addr =inet_addr("127.0.0.1");
    // Связь адреса и сокета, чтобы он мог принимать входящие дейтаграммы
    if (bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0)
        return sock_err("bind", s);
    do
    {
        char buffer[1024] = { 0 };
        int len = 0;
        unsigned int addrlen = sizeof(addr);
        // Принятие очередной дейтаграммы
        int rcv = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*) &addr,&addrlen);
        if (rcv > 0)
        {
            unsigned int ip = ntohl(addr.sin_addr.s_addr);

            printf("Datagram :%s",buffer);

            for (i = 0; i < rcv; i++)
            {
                if (buffer[i] == '\n')
                    break;
                len++;
            }
            printf(" string len is: %d, message: %s\n", len,buffer);
        }
        sprintf(buffer, "Length: %d datagram", len);
        // Отправка результата-дейтаграммы клиенту
        sendto(s, buffer, strlen(buffer), flags, (struct sockaddr*) &addr, addrlen);
    } while (1);
    // Закрытие сокета
    s_close(s);
    deinit();
    return 0;
}
