#include <locale>
#include <tcp_server/tcp_server.h>
#include <memory>

using namespace std;

int main(int argc, char* argv[])
{

    setlocale (LC_ALL, "ru_RU.UTF-8");
    // Инициалиазация сетевой библиотеки
    if(argc<1)
    {
        printf("usage: ./tcpserver 9002");
        return 0;
    }

    string port=argv[1];
    int port2=std::atoi ( port.c_str());






    auto tcp_s=make_shared<tcp_server>(port2);




    // Создание TCP-сокета
    tcp_s->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if ( tcp_s->server_socket < 0)
        return tcp_s->sock_err("socket",  tcp_s->server_socket);
    // Заполнение адреса прослушивания

    int queue=5; // количество клиентов в очереди(размер очереди запросов)

    // Связывание сокета и адреса прослушивания
    if (bind( tcp_s->server_socket, (struct sockaddr*) &tcp_s->addr, sizeof(tcp_s->addr)) < 0)
        return tcp_s->sock_err("bind",  tcp_s->server_socket);
    // Начало прослушивания
    if (listen( tcp_s->server_socket, queue) < 0)
    {
        return tcp_s->sock_err("listen",  tcp_s->server_socket);
    }
    tcp_s->listen_events();



    tcp_s->socket_close(tcp_s->server_socket);
    tcp_s->~tcp_server();

    return 0;
}
