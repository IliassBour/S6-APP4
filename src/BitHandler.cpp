#include "BitHandler.h"
#include "Particle.h"

volatile unsigned long BitHandler::clk_values[10] = {0,0,0,0,0,0,0,0,0,0};
volatile int BitHandler::clk_index = 0;
volatile int BitHandler::clk_flag = 0;
volatile byte BitHandler::messagePaquet[6] = {0x55, 0x7e, 0xf0, 0xff, 0x00, 0x7e};  //55(Clock) - 7e(Start/End) - f0(Entête/1111 0000) - ff(Data) - 00(CRC) ***DEFAULT MESSAGE
volatile int BitHandler::msg_counter = 0;

//Class constructor
BitHandler::BitHandler(){
    //BitHandler::up_counter = 0;
    //BitHandler::down_counter = 0;
    /*for(int i=0;i<10;i++)
    {
        BitHandler::clk_values[i] = 0;
    }
    BitHandler::clk_index = 0;*/
}

//Change message data
void BitHandler::setMessage(byte message){
    BitHandler::messagePaquet[3] = message;
}

//Send message (Only clock signal for now)
void BitHandler::threadSendMessage(){
    if(BitHandler::clk_flag){
        BitHandler::clk_index = 0;
    }

    WITH_LOCK(Serial){
        Serial.println("SENDING MESSAGE");
    }
    
    //Send message
    for(int i = 0; i < 6; i++){
        int bits[8] = {0};

        for(int bit = 7; bit >= 0; --bit){
            bits[bit] = (BitHandler::messagePaquet[i] >> bit) & 1;
            if(bits[bit]%2 == 0){
                sendManch_0();
            }
            else{
                sendManch_1();
            }
        }
    }
    
    //Send clock signal
    /*for(int i = 0; i < 8; i++){
        if(i%2 == 0){
            sendManch_0();
        }
        else{
            sendManch_1();
        }
    }*/

    //BitHandler::clk_flag = 1;
}

double BitHandler::calculCLK(){
    unsigned long time1 = BitHandler::clk_values[2] - BitHandler::clk_values[1];
    unsigned long time2 = BitHandler::clk_values[4] - BitHandler::clk_values[3];
    unsigned long time3 = BitHandler::clk_values[6] - BitHandler::clk_values[5];
    unsigned long time4 = BitHandler::clk_values[8] - BitHandler::clk_values[7];
    
    WITH_LOCK(Serial){
        Serial.printlnf("clk0: %d", BitHandler::clk_values[0]);
        Serial.printlnf("clk1: %d", BitHandler::clk_values[1]);
        Serial.printlnf("clk2: %d", BitHandler::clk_values[2]);
        Serial.printlnf("clk3: %d", BitHandler::clk_values[3]);
        Serial.printlnf("clk4: %d", BitHandler::clk_values[3]);
        Serial.printlnf("clk5: %d", BitHandler::clk_values[5]);
        Serial.printlnf("clk6: %d", BitHandler::clk_values[6]);
        Serial.printlnf("clk7: %d", BitHandler::clk_values[7]);
        Serial.printlnf("clk8: %d", BitHandler::clk_values[8]);
        Serial.printlnf("Time1: %d", time1);
        Serial.printlnf("Time2: %d", time2);
        Serial.printlnf("Time3: %d", time3);
        Serial.printlnf("Time4: %d", time4);
        Serial.printlnf("Message counter: %d", BitHandler::msg_counter);
    }
    
    double moy = ((double)time1+(double)time2+(double)time3+(double)time4)/4.0;
    
    return moy;
}

//Rising edge interrupt
void BitHandler::risingInterrupt(){
    if(BitHandler::clk_index != 9){
        BitHandler::clk_values[BitHandler::clk_index] = millis();
        BitHandler::clk_index++;
    }else{BitHandler::clk_flag = 1;}
    BitHandler::msg_counter++;
    
    attachInterrupt(BitHandler::inputPin, BitHandler::fallingInterrupt, FALLING);
}

//Falling edge interrupt
void BitHandler::fallingInterrupt(){
    if(BitHandler::clk_index != 9){
        BitHandler::clk_values[BitHandler::clk_index] = millis();
        BitHandler::clk_index++;
    }else{BitHandler::clk_flag = 1;}
    BitHandler::msg_counter++;
    
    attachInterrupt(BitHandler::inputPin, BitHandler::risingInterrupt, RISING);
}

//Sends a Manchester coded "1"
void BitHandler::sendManch_1(){
    digitalWrite(D3, HIGH);
    delay(BitHandler::half_period);
    digitalWrite(D3, LOW);
    delay(BitHandler::half_period);
}

//Sends a Manchester coded "0"
void BitHandler::sendManch_0(){
    digitalWrite(D3, LOW);
    delay(BitHandler::half_period);
    digitalWrite(D3, HIGH);
    delay(BitHandler::half_period);
}