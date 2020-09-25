#ifndef DATAPACK_H
#define DATAPACK_H

#define LENGTH_BYTE 4
#define OPERATION_BYTE 1
#define DATAPACK_LENGTH 100
#define BUFF_LENGTH 150

class DataPack {
public:
    int len;
    char op;
    char data[DATAPACK_LENGTH];
};


#endif