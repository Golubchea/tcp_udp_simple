#ifndef TCP_CLIENT
#define TCP_CLIENT

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

#ifdef __cplusplus
extern "C" {
#endif






typedef struct tcp_client
{

} tcp_client;


/*!
 * \brief конструктор в си
 * \return
 */
tcp_client *    tcp_client_new(); //

/*!
 * \brief деструктор в си
 * \param cli
 */
void    tcp_client_delete(tcp_client * cli);

/*!
 * \brief s_close -закрыть сокет
 * \param s
 */
void tcp_client_s_close(int s);

/*!
 * \brief sock_err
 * \param название функции с ошибкой
 * \param id сокета
 * \return  номер ошибки
 */
int tcp_client_sock_err(const char* function, int s);


/*!
 * \brief отправить запрос
 * \param массив байтов для передачи сообщения(char *)
 * \param id сокета
 * \return
 */
int tcp_client_send_request(void* request, int s,int size);

/*!
 * \brief получить ответ
 * \param id сокета
 * \param f файл для записи
 * \return
 */
int tcp_client_recv_response(int s, FILE* f);



int tcp_client_recv_simple(int s, FILE* f);


#ifdef __cplusplus
}
#endif

#endif // TCP_CLIENT



