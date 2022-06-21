#include "Particle.h"

//BitHandler_H
#ifndef BitHandler_H
#define BitHandler_H


class BitHandler
{
private:
    //General
    static const int half_period = 10;
    static const int inputPin = D2;
    static const int outputPin = D3;

    //Write
    //system_tick_t lastOuputThreadTime = 0;

    //Read
    //static volatile int up_counter; //temp
    //static volatile int down_counter; //temp
    static volatile int clk_flag;
    static volatile int msg_counter;

    //Functions
    static void sendManch_1();
    static void sendManch_0();

public:
    //Write
    static volatile byte messagePaquet[6];

    //Read
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