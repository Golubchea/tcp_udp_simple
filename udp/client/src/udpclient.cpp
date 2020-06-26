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
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <string.h>

#define WEBHOST "yandex.ru"
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
// Функция извлекает IPv4-адрес из DNS-дейтаграммы.
// Задание л/р не требует детального изучения кода этой функции
unsigned int get_addr_from_dns_datagram(const char* datagram, int size);
void send_request(int s, struct sockaddr_in* addr)
{
    // Данные дейтаграммы DNS-запроса. Детальное изучение для л/р не требуется.
    char dns_datagram[] = {"0123456"};
#ifdef _WIN32
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif
    int res = sendto(s, dns_datagram, sizeof(dns_datagram), flags, (struct sockaddr*) addr,
                     sizeof(struct sockaddr_in));
    if (res <= 0)
        sock_err("sendto", s);
}
// Функция принимает дейтаграмму от удаленной стороны.
// Возвращает 0, если в течение 100 миллисекунд не было получено ни одной дейтаграммы
unsigned int recv_response(int s)
{
    char datagram[1024];
    struct timeval tv = {0, 100*1000}; // 100 msec
    int res;
    fd_set fds;
    FD_ZERO(&fds); FD_SET(s, &fds);
    // Проверка - если в сокете входящие дейтаграммы
    // (ожидание в течение tv)
    res = select(s + 1, &fds, 0, 0, &tv);
    if (res > 0)
    {
        // Данные есть, считывание их
        struct sockaddr_in addr;
        unsigned int addrlen = sizeof(addr);
        int received = recvfrom(s, datagram, sizeof(datagram), 0, (struct sockaddr*) &addr,&addrlen);
        if (received <= 0)
        {
            // Ошибка считывания полученной дейтаграммы
            sock_err("recvfrom", s);
            return 0;
        }
        printf("%s\n",datagram);
        return received;//get_addr_from_dns_datagram(datagram, sizeof(datagram));
    }
    else if (res == 0)
    {
        // Данных в сокете нет, возврат ошибки
        return 0;
    }
    else
    {
        sock_err("select", s);
        return 0;
    }
}
int main()
{
    int s;
    struct sockaddr_in addr;
    int i;
    // Инициалиазация сетевой библиотеки
    init();
    // Создание UDP-сокета
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
        return sock_err("socket", s);
    // Заполнение структуры с адресом удаленного узла
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000); // Порт DNS - 53
    addr.sin_addr.s_addr = INADDR_ANY;
    // Выполняется 5 попыток отправки и затем получения дейтаграммы.
    // Если запрос или ответ будет потерян - данные будут запрошены повторно
    for (i = 0; i < 15; i++)
    {
        printf(" sending request: attempt %d\n", i + 1);
        // Отправка запроса на удаленный сервер
        send_request(s, &addr);
        // Попытка получить ответ. Если ответ получен - завершение цикла попыток
        if (recv_response(s))
        {
            break;
        }
    }
    // Закрытие сокета
    s_close(s);
    deinit();
    return 0;
}
