#include "Particle.h"

//BitHandler_H
#ifndef BitHandler_H
#define BitHandler_H


class BitHandler
{
private:
    //General
    static const int half_period = 1;  //in ms
    static const int inputPin = D2;
    static const int outputPin = D3;

    //Read
    static volatile int clk_flag;
    static volatile int start_flag;
    static volatile int header_flag;
    static volatile int data_flag;
    static volatile int crc_flag;

    static volatile double clk_value_R;

    static volatile unsigned long time_buffer[2];
    static volatile unsigned long time_buffer_crctr;
    static volatile int crctd_flag;
    static volatile int time_buffer_index;
    static volatile byte byte_buffer;
    static volatile int byte_buffer_index;
    static volatile int byte_buffer_overflow; //0=0, 1=1, 2=no overflow
    static volatile int msg_buffer_index;
    static volatile int crc_buffer_index;

    static volatile byte startBuffer;
    static volatile byte headerBuffer;
    static volatile byte msgBuffer[74];
    static volatile byte crcBuffer[2];
    static volatile byte endBuffer;

    //Functions
    static void manchesterDecode(int type); //0 = falling, 1 = rising
    static void readBit(int type);
    static void updateByteBuffer(int bit1, int bit2 = 2); // bit2=2 => aucun bit
    static void sendManch_1();
    static void sendManch_0();

public:
    //Write
    static volatile byte messagePaquet[80];
    static volatile int messageByteLengthW;

    //Read
    static volatile unsigned long clk_values[10];
    static volatile int clk_index;
    static volatile int end_flag;

    BitHandler();
    static void resetHandler();
    static void setMessage(byte message[80], int messageLength);
    static void threadSendMessage();
    static void risingInterrupt();
    static void fallingInterrupt();
    static double calculCLK();
    static void getRawMessageBytes(byte (&messageReturn)[80]);
};

#endif