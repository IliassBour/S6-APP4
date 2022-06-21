#include "Particle.h"

//BitHandler_H
#ifndef BitHandler_H
#define BitHandler_H


class BitHandler
{
private:
    //General
    static const int half_period = 10;  //in ms
    static const int inputPin = D2;
    static const int outputPin = D3;

    //Write
    //system_tick_t lastOuputThreadTime = 0;

    //Read
    static volatile int msg_counter;
    static volatile int msg_byte_length_R;

    static volatile int clk_flag;
    static volatile int start_flag;

    static volatile double clk_value_R;

    static volatile unsigned long time_buffer[2];
    static volatile int time_buffer_index;
    static volatile byte byte_buffer;
    static volatile int byte_buffer_index;

    static volatile byte clockBufferTemp;
    static volatile byte startBufferTemp;
    static volatile byte headerkBufferTemp;
    static volatile byte msgBufferTemp[74];
    static volatile byte crcBufferTemp[2];
    static volatile byte endBufferTemp;

    //Functions
    static void manchesterDecode(int type); //0 = falling, 1 = rising
    static void sendManch_1();
    static void sendManch_0();

public:
    //Write
    static volatile byte messagePaquet[80];
    static volatile int messageByteLengthW;

    //Read
    static volatile int read_end_flag;
    static volatile unsigned long clk_values[10];
    static volatile int clk_index;

    BitHandler();
    static void setMessage(byte message);
    static void threadSendMessage();
    static void risingInterrupt();
    static void fallingInterrupt();
    static double calculCLK();
};

#endif