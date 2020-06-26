#ifndef TCP_SERVER
#define TCP_SERVER

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
#endif
#include <arpa/inet.h>
#include <string.h>

#include <stdio.h>

#include <data/data.h>
#include <vector>

#include <memory>

class tcp_server
{
public:
    tcp_server(int port);
    ~tcp_server();

    int send_simple_no(int cs);

    int send_simple(int cs);
    void listen_events();
    void printToFILE(FILE* f,row* r,sockets * s,char * message);
    /*!
 * \brief s_close -закрыть сокет
 * \param s
 */
    void socket_close(int id);

    /*!
 * \brief sock_err
 * \param название функции с ошибкой
 * \param id сокета
 * \return  номер ошибки
 */
    int sock_err(const char* function, int s);



    sockets* getConnectedSocketById(int id,data *d);
    // функции обработки сервера
    bool recv_command(int cs); // если прилетит put
    //главная функция где все происходит
    int recv_all_row(int cs);


    int recv_row(int cs);
    int send_notice(int cs, int len);
    int send_notice_ok(int cs,char* txt, int len);

    /*!
 * \brief перевод в неблокирующий режим
 * \param s id сокета
 * \return
 */
    int set_non_block_mode(int s);







    data* server_data;

    int server_socket;
    // id подключеных сокетов

    struct sockaddr_in addr;
} ;












#ifdef __cplusplus
extern "C" {
#endif







#ifdef __cplusplus
}
#endif

#endif // tcp_server



