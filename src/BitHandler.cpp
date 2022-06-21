#include "BitHandler.h"
#include "Particle.h"

//Class constructor
BitHandler::BitHandler(){
    BitHandler::up_counter = 0;
    BitHandler::down_counter = 0;
    for(int i=0;i<10;i++)
    {
        BitHandler::clk_values[i] = 0;
    }
    BitHandler::clk_index = 0;
}

//Send message (Only clock signal for now)
void BitHandler::threadSendMessage(){
    WITH_LOCK(Serial){
        Serial.println("SENDING MESSAGE");
    }
    //Send clock signal
    for(int i = 0; i < 8; i++){
        if(i%2 == 0){
            sendManch_1();
        }
        else{
            sendManch_0();
        }
    }
}

double BitHandler::calculCLK(){
    /*unsigned long time1 = BitHandler::clk_values[2] - BitHandler::clk_values[1];
    unsigned long time2 = BitHandler::clk_values[4] - BitHandler::clk_values[3];
    unsigned long time3 = BitHandler::clk_values[6] - BitHandler::clk_values[5];
    unsigned long time4 = BitHandler::clk_values[8] - BitHandler::clk_values[7];*/
    unsigned long time1 = 100;
    unsigned long time2 = 100;
    unsigned long time3 = 100;
    unsigned long time4 = 100;
    
    WITH_LOCK(Serial){
        /*Serial.printlnf("clk0: %d", BitHandler::clk_values[0]);
        Serial.printlnf("clk1: %d", BitHandler::clk_values[1]);
        Serial.printlnf("clk2: %d", BitHandler::clk_values[2]);
        Serial.printlnf("clk3: %d", BitHandler::clk_values[3]);
        Serial.printlnf("clk4: %d", BitHandler::clk_values[3]);
        Serial.printlnf("clk5: %d", BitHandler::clk_values[5]);
        Serial.printlnf("clk6: %d", BitHandler::clk_values[6]);
        Serial.printlnf("clk7: %d", BitHandler::clk_values[7]);
        Serial.printlnf("clk8: %d", BitHandler::clk_values[8]);*/
        Serial.printlnf("Time1: %d", time1);
        Serial.printlnf("Time2: %d", time2);
        Serial.printlnf("Time3: %d", time3);
        Serial.printlnf("Time4: %d", time4);
    }
    
    double moy = ((double)time1+(double)time2+(double)time3+(double)time4)/4.0;
    
    return moy;
}

//Rising edge interrupt
void BitHandler::risingInterrupt(){
    BitHandler::up_counter++;
    BitHandler::clk_values[BitHandler::clk_index] = millis();
    /*WITH_LOCK(Serial){
        Serial.printlnf("Temps: %d", millis());
    }*/
    BitHandler::clk_index++;
    
    attachInterrupt(BitHandler::inputPin, BitHandler::fallingInterrupt, FALLING);
}

//Falling edge interrupt
void BitHandler::fallingInterrupt(){
    BitHandler::down_counter++;
    BitHandler::clk_values[BitHandler::clk_index] = millis();
    /*WITH_LOCK(Serial){
        Serial.printlnf("Temps: %d", millis());
    }*/
    BitHandler::clk_index++;
    
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