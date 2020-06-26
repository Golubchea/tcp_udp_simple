#include <iostream>
#include <fstream>
#include <tcp_client/tcp_client.h>
#include <time.h>

#include <string.h>
#include <vector>
#include <array>
#include <algorithm>
#include <stdint.h>
//класс с датой
#include <data/data.h>
// вектор в си
#include <c99Vector/C99vector.h>

#include <bits_functions/C99bits.h>
using namespace std;

// самые важные команды для передачи данных
//
// htonl comp->internet 32 bit [4 bytes]
// ntohl comp<-internet 32 bit [4 bytes]

// htons comp->internet 16 bit [2 bytes]
// ntohs comp<-internet 16 bit [2 bytes]



int main(int argc, char **argv)
{


    // tcpserver 9000
    if(argc<2)
    {
        printf("usage: ./tcpclient 192.168.50.7:9000 file1.txt");
        return 0;
    }


    sockets so;
    string s= argv[1]; //"ip:port"


    char separator = ':';

    // Find first occurrence of ':'
    size_t found =s.find(separator);
    if (found != string::npos)
    {
        cout << "First occurrence is " << found << endl;

        so.ip=s.substr (0,found);
        string temp_port=s.substr (found+1);

        so.port=std::atoi (temp_port.c_str());   // std::string -> int
    }
    else
    {
        cout << "exit: bad ip:port"  << endl;
        return 0;
    }



    // в этом классе парсятся строки
    data d;
    d.load(argv[2]);// загрузка файла
    d.print();

    d.setDataUints_to_send();


    // Инициалиазация сетевой библиотеки

    struct sockaddr_in addr;


    tcp_client * tcp_client=tcp_client_new();

    // Создание TCP-сокета
    so.id = socket(AF_INET, SOCK_STREAM, 0);
    if (so.id < 0)// в Си многие функции возвращают число, проверка
        return tcp_client_sock_err("socket", so.id);


    // Заполнение структуры с адресом удаленного узла
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    addr.sin_port = htons(so.port);
    addr.sin_addr.s_addr =  inet_addr(so.ip.c_str()); // Все адреса



    FILE* f;
    f = fopen("../client_received_log.txt", "wb");




    for (int i=0;i<20;++i)
    {
        // Установка соединения с удаленным хостом
        if (connect(so.id, (struct sockaddr*) &addr, sizeof(addr)) == 0)
        {

            break;
        }
        else {
            if(i==19)
            {
                tcp_client_s_close(so.id);
                return tcp_client_sock_err("connect", so.id);

            }
            usleep(1000 *100);
            printf("подключаемся %i раз(а) \n",i);
        }

    }



    char command[3]={'p','u','t'};//,'\n'};   // for test write
    // char command[4]={'p','1','t','\n'};  // for test wrong

    tcp_client_send_request( command,so.id,3);
    //tcp_client_recv_simple(so.id,f);
   // usleep(10^8);
    //отправляем данные, пока не закончится строка
    for (int i=0;i<d.vec.size();++i)
    {
        char buffer[1000];
        char * b_ptr=&buffer[0];

        b_ptr=d.newDataBlock(i,buffer);


        unsigned int row_size=15;
        unsigned int message_size=strlen(d.message[i]);


        // готовим данные к отправке

        tcp_client_send_request( buffer,so.id,row_size+message_size);
         //usleep(10^8);
        usleep(10^4);
        tcp_client_recv_simple(so.id,f);
    }





    fclose(f);
    //  Закрытие соединения
    tcp_client_s_close(so.id);


    cout << "Клиент закончил работу" << endl;
    return 0;
}
