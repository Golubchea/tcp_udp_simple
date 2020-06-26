#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#pragma packed(1)
typedef struct row
{

    uint32_t row_number;

    unsigned char day;
    unsigned char month;
    uint16_t year;
    //separator
    unsigned char day2;
    unsigned char month2;
    uint16_t year2;
    //separator
    unsigned char hours;
    unsigned char min;
    unsigned char sec;
    //char* message;
}row;


typedef struct sockets
{
    int id; //дескриптор сокета
    std::string ip;
    int port;
}sockets;

class data
{
public:
    data();
    int load(std::string path="../sample.txt");
    int parse_row(row * r,FILE* f2, uint32_t row,char * message);
    bool isNotEmpty_str(FILE* f2);
    //в этих двух функциях переводим uint32_t в сетевой порядок байт
    void setDataUints_to_send();
    // и обратно
    void setDataUints_to_receive();
sockets* getConnectedSocketById(int id);
    char* newDataBlock(int index,char buf[]);
    void deleteDataBlock(char *datablock);


    void printToFILE(FILE* f,row* r,sockets * s,char * message);
    void print();
    void USARTWrite( void *object, size_t size);
    ~data();
    std::vector<row *> vec;
    std::vector<char *> message;
    std::vector<sockets*> connected_sockets;
};

#endif // DATA_H
