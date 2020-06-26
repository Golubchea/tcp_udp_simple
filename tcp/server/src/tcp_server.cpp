
#include <tcp_server/tcp_server.h>

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
#include <c99Vector/C99vector.h>
#include <fcntl.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else // LINUX

#endif

#include <bits_functions/C99bits.h>
#include <memory>
#include <data/data.h>
#include <iostream>
#include <algorithm>
#include <istream>
#include <fstream>
#include <iosfwd>



// функция переключения с блокирующего на неблокирующий режимы
int tcp_server::set_non_block_mode(int s)
{
#ifdef _WIN32
    unsigned long mode = 1;
    return ioctlsocket(s, FIONBIO, &mode);
#else
    int fl = fcntl(s, F_GETFL, 0);
    return fcntl(s, F_SETFL, fl | O_NONBLOCK);
#endif
}

//////////// вспомогательные функции начались

tcp_server::tcp_server(int port )
{
    //выделение памяти под структуру

    memset(&this->addr, 0, sizeof(this->addr));
    this->addr.sin_family = AF_INET;
    this->addr.sin_port = htons(port); // Сервер прослушивает порт 9002
    this->addr.sin_addr.s_addr = htonl(INADDR_ANY); // Все адреса

    this->server_data=new data( );


#ifdef _WIN32
    // Для Windows следует вызвать WSAStartup перед началом использования сокетов
    WSADATA wsa_data;
    return (0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));
#else
    // Для других ОС действий не требуется
#endif

}


tcp_server::~tcp_server()
{
#ifdef _WIN32
    // Для Windows следует вызвать WSACleanup в конце работы
    WSACleanup();
#else
// Для других ОС действий не требуется
#endif
    delete this->server_data;
}



//закрыть сокет (int s - id сокета) //этой функцией сервер закрывает подлючения с клиентами и вызывает на свой id
void tcp_server::socket_close(int s)
{
#ifdef _WIN32
    closesocket(s);
#else

    close(s);
#endif
}

int tcp_server::sock_err(const char* function, int s)
{
    int err;
#ifdef _WIN32
    err = WSAGetLastError();
#elseerr = errno;
#endif
    fprintf(stderr, "%s: socket error: %d\n", function, err);
    return -1;
}

//////////// вспомогательные функции закончились
///
/// дальше 2 функции событий сервера
int tcp_server::recv_row(int cs)
{




    char buffer[1024];
    //memset(buffer,0x00,1024);
    int curlen = 0;
    int rcv;
    do
    {
        int i;

        rcv = recv(cs, buffer, sizeof(buffer), 0);
        if (rcv==-1)
        {
            return -1;
        }
        for (i = 0; i < rcv; i++)
        {
            if (buffer[i] == '\n' )
            {
                // здесь происходит все самое важное

                unsigned int blockSize=curlen+1;
                unsigned int row_size_serv=15;
                row *temp_row=new row();
                memset(temp_row,0x00,row_size_serv);

                char * received_message=new char;
                memcpy( &received_message[0], &buffer[row_size_serv], blockSize-row_size_serv );
                memcpy( temp_row, &buffer[0], row_size_serv );

                // struct на сервере, куда заполняются подключения
                sockets *so=getConnectedSocketById(cs,this->server_data);

                this->server_data->stored_sockets_to_output.push_back(so);
                this->server_data->vec.push_back(temp_row);
                this->server_data->message.push_back(received_message);

                return curlen;
            }
            //return curlen;
            curlen++;
        }
        if (curlen > 5000)
        {
            printf("input string too large\n");
            return 5000;
        }
    } while (rcv > 0);


    return curlen;
}


bool tcp_server::recv_command(int cs)
{

    char buffer[1024];
    int curlen = 0;
    int rcv;
    do
    {
        int i;

        rcv = recv(cs, buffer, sizeof(buffer), 0);
        for (i = 0; i < rcv; i++)
        {
            if (buffer[i] == '\n' )
            {


                printf(" buffer[i] == '\n'");
                // return curlen;
            }
            //return curlen;
            curlen++;
        }
        if (curlen > 5000)
        {
            printf("input string too large\n");
            //return 5000;
        }
    } while (rcv > 0);

    if(buffer[0]=='p' && buffer[1]=='u' && buffer[2]=='t' )
    {
        return true;
    }
    else {
        return false;
    }



}










sockets *tcp_server::getConnectedSocketById(int id,data *d)
{

    std::vector<int> vec;

    for (int i=0;i<d->connected_sockets.size();++i)
    {
        vec.push_back(d->connected_sockets[i]->id);
    }


    std::vector<int>::iterator it;
    it = std::find( vec.begin(), vec.end(), id);

    if (it !=  vec.end())
    {

        auto index = std::distance(vec.begin(), it);
        std::cout << "Element " << id <<" found at position : "<<index <<std::endl ;
        return d->connected_sockets.at(index);
    }
    else {
        std::cout << "Element " << id <<"not found in sockets *data::getConnectedSocketById" ;
        return d->connected_sockets.at(0);
    }
}

void tcp_server::printToFILE(FILE *f,row* r,sockets * s,char * mess)
{


    std::cout<<"printed in file:"<<std::endl;
    std::cout<<s->ip<<":"<<s->port<<
        " "<<(int)r->day<<"."<<(int)r->month<<"."<<r->year<<" "<<
        (int)r->day2<<"."<<(int)r->month2<<"."<<r->year2 <<
        " "<<(int)r->hours<<":"<<(int)r->min<<":"<<(int)r->sec<< " "<<
        mess <<std::endl;

    fprintf(f,"%s:%i %i.%i.%u %i.%i.%u %i:%i:%i %s", s->ip.c_str(), s->port, (int)r->day,(int)r->month,(unsigned int)r->year,
            (int)r->day2,(int)r->month2,(unsigned int)r->year2,(int)r->hours, (int)r->min, (int)r->sec ,mess );




}


int tcp_server::send_notice(int cs, int len)
{
    char buffer[1024];
    int sent = 0;
    int ret;
#ifdef _WIN32
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif
    sprintf(buffer, "Length of your string: %d chars.", len);
    while (sent < (int) strlen(buffer))
    {
        ret = send(cs, buffer + sent, strlen(buffer) - sent, flags);
        if (ret <= 0)
            return sock_err("send", cs);
        sent += ret;
    }
    return 0;

}

int tcp_server::send_notice_ok(int cs,char* txt, int len)
{
    char buffer[1024];
    int sent = 0;
    int ret;
#ifdef _WIN32
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif
    sprintf(buffer, txt,len);
    while (sent < (int) strlen(buffer))
    {
        ret = send(cs, buffer + sent, strlen(buffer) - sent, flags);
        if (ret <= 0)
            return sock_err("send", cs);
        sent += ret;
    }
    return 0;

}

int tcp_server::send_simple(int cs)
{
    char buffer[2];
    buffer[0]='o';
    buffer[1]='k';


    int ret;
#ifdef _WIN32
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif

    ret = send(cs, buffer  , 2  , flags);

    if (ret <= 0)
        return sock_err("send", cs);


    return ret;

}

int tcp_server::send_simple_no(int cs)
{
    char buffer[2];
    buffer[0]='n';
    buffer[1]='o';


    int ret;
#ifdef _WIN32
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif

    ret = send(cs, buffer  , 2  , flags);

    if (ret <= 0)
        return sock_err("send", cs);


    return ret;

}


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

in_port_t get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}
void tcp_server::listen_events()
{
    printf("Сервер слушает...");

    int row_counter=0;
    bool server_on=true;
    int no_times=20;
    fd_set rfd;
    fd_set wfd;
    int nfds = this->server_socket;

    struct timeval tv = { 1, 0 };
    int len;

    while (server_on)
    {

        FD_ZERO(&rfd);
        FD_ZERO(&wfd);
        FD_SET(this->server_socket, &rfd);


        for (int i=0; i < this->server_data->connected_sockets.size(); i++)
        {
            FD_SET(  this->server_data->connected_sockets[i]->id , &rfd);
            FD_SET(this->server_data->connected_sockets[i]->id, &wfd);
            if (nfds < this->server_data->connected_sockets[i]->id)
                nfds = this->server_data->connected_sockets[i]->id;
        }
        if (select(nfds + 1, &rfd, &wfd, 0, &tv) > 0)
        {
            // Есть события
            if (FD_ISSET(this->server_socket, &rfd))
            {
                // Есть события на прослушивающем сокете, можно вызвать accept, принять
                // подключение и добавить сокет подключившегося клиента в массив cs

                struct sockaddr_storage their_addr;
                socklen_t addr_size;
                addr_size = sizeof their_addr;

                int cs = accept(this->server_socket, (struct sockaddr*) &their_addr, &addr_size);
                set_non_block_mode(cs);



                if (cs < 0)
                {
                    sock_err("accept", this->server_socket);
                    break;
                }

                // Вывод адреса нового клиента
                sockets sock;
                sock.id=cs;

                sock.opened=false;

                char hoststr[NI_MAXHOST];
                char portstr[NI_MAXSERV];



                printf("selectserver: new connection from %s:%d on "
                       "socket  %d\n",
                       inet_ntop(their_addr.ss_family,
                                 get_in_addr((struct sockaddr*)&their_addr),
                                 hoststr, INET6_ADDRSTRLEN),
                       ntohs(get_in_port((struct sockaddr*)&their_addr    )),
                       cs);
 //
                sprintf( portstr,"%d", ntohs(get_in_port((struct sockaddr*)&their_addr    )  )   );


                sock.ip=hoststr;
                sock.port=atoi(portstr);


                server_data->connected_sockets.push_back(&sock);


            }



            for (unsigned i = 0; i < this->server_data->connected_sockets.size(); i++)
            {
                bool closed=false;

                if (FD_ISSET(this->server_data->connected_sockets[i]->id, &rfd))
                {
                    // Сокет cs[i] доступен для чтения. Функция recv вернет данные,       recvfrom - дейтаграмму
                    // printf("сокет доступен для чтения .\n");


                    if(this->server_data->connected_sockets[i]->opened==true)
                    {
                        len=  recv_row(this->server_data->connected_sockets[i]->id);
                    }
                    else
                    {
                        this->server_data->connected_sockets[i]->opened=recv_command(this->server_data->connected_sockets[i]->id);
                    }

                    if (len==0)
                    {
                        closed=true;
                    }
                }

                if (FD_ISSET(this->server_data->connected_sockets[i]->id, &wfd))
                {

                    // printf("сокет доступен для записи .\n");


                    if(this->server_data->connected_sockets[i]->opened==true)
                    {
                        send_simple(this->server_data->connected_sockets[i]->id);
                        row_counter++;
                    }
                    else
                    {
                        // send_simple_no(this->server_data->connected_sockets[i]->id);
                        no_times--;
                        if(no_times==0)
                        {
                            closed=true;
                            no_times=20;
                        }
                    }


                    if (closed)
                    {
                        this->server_data->connected_sockets.erase(this->server_data->connected_sockets.begin() + i);
                        socket_close(this->server_data->connected_sockets[i]->id);
                    }

                }

            }
        }
        else
        {
            if(!this->server_data->message.empty())
            {

                for(int k=0;k<server_data->message.size();++k)
                {
                    char* tex= this->server_data->message[k];
                    bool s=(*(tex+0)== 's');
                    bool t=(*(tex+1)== 't');
                    bool o=(*(tex+2)== 'o');
                    bool p=(*(tex+3)== 'p');
                    bool n=(*(tex+4)== '\n');


                    if(s&&t&&o&&p&&n)
                    {

                        this->server_data->setDataUints_to_receive();

                        FILE* f_serv;
                        f_serv = fopen("../server_received_log.txt", "wb");

                        for (int i=0; i<server_data->vec.size();++i)
                        {
                            row *r= server_data->vec[i];
                            char *t=server_data->message[i];
                            sockets * s=server_data->stored_sockets_to_output[i];

                            server_data->printToFILE(f_serv,r,s,t);

                        }
                        fclose(f_serv);
                        socket_close(this->server_socket);
                        server_on=false;

                        break;

                    }
                }


            }

        }

        //printf("Произошел таймаут или ошибка");
        usleep(10);
    }
}




