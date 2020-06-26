#include <data/data.h>


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>

#include <arpa/inet.h>

#include <algorithm>


#include <stdint.h>

#define CHAR_BITS  8  // size of character

#define INT_BITS  ( sizeof(int) * CHAR_BITS) //bits in integer

#define SHORT_BITS  ( sizeof(short) * CHAR_BITS) //bits in integer

#define MAX_MSG_LEN 999

data::data( )
{

}

int data::load(std::string path)
{
    int count = 0;
    std::string line;

    /* Creating input filestream */
    std::ifstream file(path);
    while (getline(file, line))
        count++;
    file.close();
    std::cout << "Numbers of lines in the file : " << count << std::endl;
    FILE* f2;

    if ((  f2 = fopen(path.c_str(), "r")   )==NULL)
    {
        printf ("Cannot open file.\n");
        exit(1);
    }







    int k=0;
    for (k=0;k<count;++k)
    {

        if(isNotEmpty_str(f2) )
        {
            row   *r=new row;
            char *mess=new char;

            parse_row( r,  f2, k ,mess);

            message.push_back((char*)mess);
            vec.push_back(r);
        }
    }

    fclose(f2);
}




sockets *data::getConnectedSocketById(int id)
{

    std::vector<int> vec;

    for (int i=0;i<connected_sockets.size();++i)
    {
        vec.push_back(connected_sockets[i]->id);
    }


    std::vector<int>::iterator it;
    it = std::find( vec.begin(), vec.end(), id);

    if (it !=  vec.end())
    {

        auto index = std::distance(vec.begin(), it);
        std::cout << "Element " << id <<" found at position : "<<index <<std::endl ;
        return this->connected_sockets.at(index);
    }
    else {
        std::cout << "Element " << id <<"not found in sockets *data::getConnectedSocketById" ;
        return this->connected_sockets.at(0);
    }
}




bool data::isNotEmpty_str(FILE* f2)
{

    char temp;


    fread(&temp, 1, 1, f2);
    if(temp==' '||temp=='\n' || temp=='\0')
    {

        return false;// пустая строка, выйти из парсинга cтроки
    }
    else {
        fseek(f2,-1,SEEK_CUR);
        return true;
    }
}


// htonl comp->internet 32 bit [4 bytes]
// ntohl comp<-internet 32 bit [4 bytes]

// htons comp->internet 16 bit [2 bytes]
// ntohs comp<-internet 16 bit [2 bytes]

void data::setDataUints_to_send()
{

    for (unsigned int i=0;i<vec.size();++i)
    {
        vec[i]->year=htons(vec[i]->year);
        vec[i]->year2=htons(vec[i]->year2);
        //32 bit uint
        vec[i]->row_number=htonl(vec[i]->row_number);
    }
}

void data::setDataUints_to_receive()
{
    for (unsigned int i=0;i<vec.size();++i)
    {
        vec[i]->year=ntohs(vec[i]->year);
        vec[i]->year2=ntohs(vec[i]->year2);
        //32 bit uint
        vec[i]->row_number=ntohl(vec[i]->row_number);
    }
}





char * data::newDataBlock(int messageIndex, char buffer[])
{

    unsigned int row_size=15;
    char * message=this->message[messageIndex];//указатель на одно сообщение
    unsigned int message_size=strlen(message);
    row   *r=this->vec[messageIndex]; //указатель на одну структуру
    // char *buffer=(char *) malloc(row_size+message_size);


    std::cout << "------newDataBlock------"<<std::endl;

    std::cout << "row_size="<<15<<std::endl;;
    std::cout << "message_size="<<strlen(message)<<std::endl;
    std::cout << "message_size="<<strlen(message)+15<<std::endl;
    std::cout << "------------"<<std::endl;





    memset(buffer,0x00,row_size+message_size);
    //memcpy( &received_message[0], &buffer[row_size_serv], blockSIze-row_size_serv +1);
    // memcpy( temp_row, &buffer[0], row_size_serv );
    //copying....






    memcpy(buffer,r,row_size);
    memcpy( &buffer[row_size], &message[0], message_size );






    std::cout<< "end prepare struct+ message in one byteblock \n";
    return buffer;
}

void data::deleteDataBlock(char *t)
{
    delete [] t;
}

void data::printToFILE(FILE *f,row* r,sockets * s,char * mess)
{

    std::cout<<s->ip<<":"<<s->port<<
        " "<<(int)r->day<<"."<<(int)r->month<<"."<<r->year<<" "<<
        (int)r->day2<<"."<<(int)r->month2<<"."<<r->year2 <<
        " "<<(int)r->hours<<":"<<(int)r->min<<":"<<(int)r->sec<< " "<<
        mess <<std::endl;

    fprintf(f,"%s:%i %i.%i.%u %i.%i.%u %i:%i:%i %s", s->ip.c_str(), s->port, (int)r->day,(int)r->month,(unsigned int)r->year,
            (int)r->day2,(int)r->month2,(unsigned int)r->year2,(int)r->hours, (int)r->min, (int)r->sec ,mess );
}

void data::print()
{
    for (int k=0;k<vec.size();++k)
    {

        std::cout<<vec[k]->row_number<<"||"<<vec[k]->year<<"|"<<vec[k]->year2<<"|"<< "message:"<<message[k]<<std::endl;


    }
}

data::~data()
{
    // new : just declare std::shared_ptr< obj-type > obj(params);
    // or auto t = std::make_shared<obj-type>obj-type(params);
    // and no need to calling delete or free in destructors

    //old cpp style


    //проверка (частая причина segfault в деструкторе)
    if(!vec.empty())
    {
        //        for (unsigned int i=0;i<vec.size();++i)
        //        {
        //            delete vec[i];
        //        }
        vec.clear();
    }

    if(!message.empty())
    {

        // old c style
        for (unsigned int i=0;i<message.size();++i)
        {
            free(message[i]);
        }
        message.clear();

    }
}


int data::parse_row(row * r,FILE* f2, uint32_t row,char * mess)
{
    //printf("указатель в файле на позиции: %ld \n", ftell(f2));
    r->row_number=row;
    unsigned char separator ;// временная переменная, нужна для мусора вроде пробелов

    char temp[4]={'\0','\0','\0','\0'};

    fread(&temp, 1, 2, f2);
    unsigned int t2=atoi(temp);
    r->day=(char)t2;


    fread(&separator, 1, 1, f2); //точки
    fread(&temp, 1, 2, f2);
    t2=atoi(temp);
    r->month=(char)t2;
    fread(&separator, 1, 1, f2); //точки



    char temp_year0[5]={'\0','\0','\0','\0','\0'};
    fread(&temp_year0,1, 4, f2);

    int year0 =atoi(temp_year0);
    //  PrintInBinary(year);


    uint16_t p_out0=year0;
    r->year=p_out0;







    fread(&separator, 1, 1, f2); //пробел

    char temp2[4]={'\0','\0'};
    fread(&temp2, 1, 2, f2);
    t2=atoi(temp2);
    r->day2=(char)t2;

    fread(&separator, 1, 1, f2); //точки
    fread(&temp2, 1, 2, f2);
    t2=atoi(temp2);
    r->month2=(char)t2;
    fread(&separator, 1, 1, f2); //точки



    char temp_year[5]={'\0','\0','\0','\0','\0'};
    fread(&temp_year,1, 4, f2);

    int year =atoi(temp_year);
    //  PrintInBinary(year);


    uint16_t p_out=year;
    r->year2=p_out;








    fread(&separator, 1, 1, f2);
    char temp3[4]={'\0','\0'};
    fread(&temp3, 1, 2, f2);
    t2=atoi(temp3);
    r->hours=(char)t2;



    fread(&separator, 1, 1, f2); //точки
    fread(&temp3, 1, 2, f2);
    t2=atoi(temp3);
    r->min=(char)t2;


    fread(&separator, 1, 1, f2); //точки
    fread(&temp3, 1, 2, f2);
    t2=atoi(temp3);
    r->sec=(char)t2;

    fread(&separator, 1, 1, f2); //пробел

    char temp_msg[MAX_MSG_LEN];


    fgets( temp_msg, MAX_MSG_LEN, f2 ); // читываем из потока ввода строку





    unsigned int i;
    for ( i = 0; i < MAX_MSG_LEN; i++ )
    {


        if ( temp_msg[i] == '\n' || temp_msg[i] == '\0')
        {
            // printf("позиции   ++%i++  \n",i);
            // printf("строка   ++%s++  \n",temp_msg);

            //r->message=(char*)malloc(i);
            //strcpy(r->message,temp_msg);

            //mess=(char*) malloc(i+1);


            strcpy(mess,temp_msg);

            //printf("file pos: %ld \n", ftell(f2));
            break;
        }

    }


    return 1; // all ok
}




//main func - convert from struct data to char*

void data::USARTWrite( void *object, size_t size)
{
    unsigned char *byte;
    for ( byte = (unsigned char *)object; size--; ++byte )
    {
        printf("%02X", *byte);
    }
    putchar('\n');
}


















