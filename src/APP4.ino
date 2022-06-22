#include "Particle.h"
#include "FrameHandler.h"
#include "BitHandler.h"

// D2 = input
// D3 = output

SYSTEM_THREAD(ENABLED);

void threadInput(void);
void threadOutput(void);


//Thread thread("threadInput", threadInput);
//Thread thread("threadOutput", threadOutput);


//***//
//void threadSendMessage(void);


//Write//
//volatile byte messagePaquet[6];
system_tick_t lastOuputThreadTime = 0;


//Read//
//volatile int up_counter = 0;
//volatile int down_counter = 0;
//volatile unsigned long clk_values[10] = {0};
//volatile int clk_index = 0;
//volatile unsigned long clk_time = 0;

//***//

void setup() {
	Serial.begin(9600);
	pinMode(D2, INPUT);
	pinMode(D3, OUTPUT);
	digitalWrite(D3, HIGH);
	attachInterrupt(D2, BitHandler::fallingInterrupt, FALLING);
}

void loop() {
    delay(8000);

    //**Create message and frame**//
    byte trame[80] = {0};
    int trameLength = sendTrame("Hello world! Am I your friend?", trame);
    sendWrongTrame(trame);
    /*for(int i =0; i <18; i++) {
        Serial.printf("trame %d: %x\n", i, trame[i]);
    }*/

    //**Prepare BitHandler**//
    BitHandler::setMessage(trame, trameLength);

    //**Send message**//
    new Thread("threadMessage", BitHandler::threadSendMessage);



    //**Wait for message to be received**//
    //À remplacer par une condition variable
    while (!BitHandler::end_flag){delay(10);}

    //**Receive raw message**//
    byte received_msg[80] = {0x00};
    BitHandler::getRawMessageBytes(received_msg);  //byte[80]
    WITH_LOCK(Serial){
        Serial.println("FIN DE LA TRANSMISSION");
        for(int i = 0; i < sizeof(received_msg)/sizeof(received_msg[0]); i++){
            Serial.printf("%02x ", received_msg[i]);
        }
        Serial.print("\n");
    }

    //**Receive Frame**//
    receiveTrame(received_msg);
}
