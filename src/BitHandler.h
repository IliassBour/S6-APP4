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
    //volatile byte[6] messagePaquet;
    //system_tick_t lastOuputThreadTime = 0;

    //Read
    static volatile int up_counter; //temp
    static volatile int down_counter; //temp
    static volatile unsigned long clk_values[10];
    static volatile int clk_index;
    static volatile int clk_flag;

    //Functions
    void sendManch_1();
    void sendManch_0();
    static void risingInterrupt();
    static void fallingInterrupt();


public:
    BitHandler();
    void threadSendMessage();
    double calculCLK();
};

#endif