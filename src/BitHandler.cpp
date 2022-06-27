#include "BitHandler.h"
#include "Particle.h"

//***Define default values***//
//General
#define MANCH_RISING 1
#define MANCH_FALLING 0

//Read
volatile unsigned long BitHandler::clk_values[10] = {0};
volatile int BitHandler::clk_index = 0;

volatile int BitHandler::clk_flag = 0;
volatile int BitHandler::start_flag = 0;
volatile int BitHandler::header_flag = 0;
volatile int BitHandler::data_flag = 0;
volatile int BitHandler::crc_flag = 0;
volatile int BitHandler::end_flag = 0;

volatile double BitHandler::clk_value_R = 0;

volatile unsigned long BitHandler::time_buffer[2] = {0};
volatile unsigned long BitHandler::time_buffer_crctr = 0;
volatile int BitHandler::crctd_flag = 0;
volatile int BitHandler::time_buffer_index = 0;
volatile byte BitHandler::byte_buffer = 0x00;
volatile int BitHandler::byte_buffer_index = 0;
volatile int BitHandler::byte_buffer_overflow = 2;
volatile int BitHandler::msg_buffer_index = 0;
volatile int BitHandler::crc_buffer_index = 0;

volatile byte BitHandler::startBuffer = 0x00;
volatile byte BitHandler::headerBuffer = 0x00;
volatile byte BitHandler::msgBuffer[74] = {0x00};
volatile byte BitHandler::crcBuffer[2] = {0x00};
volatile byte BitHandler::endBuffer = 0x00;

//Write
volatile byte BitHandler::messagePaquet[80] = {0x00};
volatile int BitHandler::messageByteLengthW = 0;
//*** ***//

//Class constructor
BitHandler::BitHandler(){}

//Change message data
void BitHandler::setMessage(byte message[80], int messageLength){
    for(int i = 0; i < messageLength; i++){
        BitHandler::messagePaquet[i] = message[i];
    }
    BitHandler::messageByteLengthW = messageLength;
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
    for(int i = 0; i < BitHandler::messageByteLengthW; i++){
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
}

double BitHandler::calculCLK(){
    unsigned long time1 = BitHandler::clk_values[2] - BitHandler::clk_values[1];
    unsigned long time2 = BitHandler::clk_values[4] - BitHandler::clk_values[3];
    unsigned long time3 = BitHandler::clk_values[6] - BitHandler::clk_values[5];
    unsigned long time4 = BitHandler::clk_values[8] - BitHandler::clk_values[7];
    
    double moy = ((double)time1+(double)time2+(double)time3+(double)time4)/4.0;
    
    return moy;
}

//Rising edge interrupt
void BitHandler::risingInterrupt(){
    BitHandler::manchesterDecode(MANCH_RISING);
    
    attachInterrupt(BitHandler::inputPin, BitHandler::fallingInterrupt, FALLING);
}

//Falling edge interrupt
void BitHandler::fallingInterrupt(){
    BitHandler::manchesterDecode(MANCH_FALLING);
    
    attachInterrupt(BitHandler::inputPin, BitHandler::risingInterrupt, RISING);
}

void BitHandler::manchesterDecode(int type){
    //Pourrait etre fait avec un Switch Case
    //CLOCK
    if(!BitHandler::clk_flag){
        BitHandler::clk_values[BitHandler::clk_index] = millis();
        BitHandler::clk_index++;
        if(clk_index >= 9){
            BitHandler::clk_flag = 1;
            BitHandler::clk_value_R = BitHandler::calculCLK();
        }
    }
    //START
    else if(!BitHandler::start_flag){
        BitHandler::readBit(type);
        
        //Fin lecture Byte
        if(BitHandler::byte_buffer_index >= 8){
            BitHandler::startBuffer = BitHandler::byte_buffer;
            BitHandler::byte_buffer_index = 0;
            BitHandler::start_flag = 1;

            WITH_LOCK(Serial){
                Serial.printlnf("Start byte: %02x", BitHandler::startBuffer);
            }
        }
    }
    //HEADER
    else if (!BitHandler::header_flag){
        BitHandler::readBit(type);
        
        //Fin lecture Byte
        if(BitHandler::byte_buffer_index >= 8){
            BitHandler::headerBuffer = BitHandler::byte_buffer;
            BitHandler::header_flag = 1;

            WITH_LOCK(Serial){
                Serial.printlnf("Header byte: %02x", BitHandler::headerBuffer);
            }
            BitHandler::byte_buffer_index = 0;
        }
    }
    //DATA
    else if (!BitHandler::data_flag){
        BitHandler::readBit(type);

        //Fin lecture Byte
        if(BitHandler::byte_buffer_index >= 8) {
            BitHandler::msgBuffer[BitHandler::msg_buffer_index] = BitHandler::byte_buffer;

            BitHandler::msg_buffer_index++;
            BitHandler::byte_buffer_index = 0;

            if (BitHandler::msg_buffer_index == BitHandler::headerBuffer){
                BitHandler::data_flag = 1;
            }
        }
        
    }
    //CRC16
    else if(!BitHandler::crc_flag){
        BitHandler::readBit(type);

        if(BitHandler::byte_buffer_index >= 8) {
            BitHandler::crcBuffer[BitHandler::crc_buffer_index] = BitHandler::byte_buffer;

            BitHandler::crc_buffer_index++;
            BitHandler::byte_buffer_index = 0;

            if (BitHandler::crc_buffer_index == 2){
                BitHandler::crc_flag = 1;
            }
        }
    }
    //END
    else if(!BitHandler::end_flag){
        BitHandler::readBit(type);

        if(BitHandler::byte_buffer_index >= 7) {
            BitHandler::endBuffer = (BitHandler::byte_buffer << 1);

            WITH_LOCK(Serial){
                Serial.printlnf("End byte: %02x", BitHandler::endBuffer);
            }

            BitHandler::byte_buffer_index = 0;
            BitHandler::end_flag = 1;
        }
    }
}

void BitHandler::readBit(int type) {
    //Calcul temps
    BitHandler::time_buffer[BitHandler::time_buffer_index] = millis();

    //Calcul bit disponible
    if(BitHandler::time_buffer_index){
        unsigned long time_diff = BitHandler::time_buffer[1] - BitHandler::time_buffer[0];
        if(time_diff >= BitHandler::clk_value_R){ //*Incertitude sur le range
            if(type == MANCH_RISING){
                //10
                BitHandler::updateByteBuffer(1, 0);
            } else{ //Falling
                //01
                BitHandler::updateByteBuffer(0, 1);
            }
            BitHandler::crctd_flag = 0;
        } else{
            if(((BitHandler::time_buffer[0] - BitHandler::time_buffer_crctr) >= BitHandler::clk_value_R && BitHandler::time_buffer_crctr != 0) || BitHandler::crctd_flag){ //*Incertitude sur le range
                if(type == MANCH_RISING){
                    //1
                    BitHandler::updateByteBuffer(1);
                } else{ //Falling
                    //0
                    BitHandler::updateByteBuffer(0);
                }
                
                //Gestion dephasage
                if((BitHandler::time_buffer[0] - BitHandler::time_buffer_crctr) >= BitHandler::clk_value_R && BitHandler::time_buffer_crctr != 0){
                    BitHandler::crctd_flag = !BitHandler::crctd_flag;
                }
            }
            else { //*Incertitude sur le range
                if(type == MANCH_RISING){
                    //0
                    BitHandler::updateByteBuffer(0);
                } else{ //Falling
                    //1
                    BitHandler::updateByteBuffer(1);
                }
            }
        }
        BitHandler::time_buffer_crctr = BitHandler::time_buffer[1];
    }

    BitHandler::time_buffer_index = !BitHandler::time_buffer_index;
}

void BitHandler::updateByteBuffer(int bit1, int bit2){   //bit2=2 => no bit2
    //Ajout overflow
    if(BitHandler::byte_buffer_overflow != 2){
        BitHandler::byte_buffer = (BitHandler::byte_buffer << 1) | (BitHandler::byte_buffer_overflow);
        BitHandler::byte_buffer_index++;
        BitHandler::byte_buffer_overflow = 2; //Reset overflow
    }
    //Update buffer
    if(bit2 != 2){ //Ajouter 2 bit
        //Possibilite d'overflow
        if(BitHandler::byte_buffer_index == 7){ //Overflow
            BitHandler::byte_buffer = (BitHandler::byte_buffer << 1) | (bit1);
            BitHandler::byte_buffer_overflow = bit2;
            BitHandler::byte_buffer_index += 1;
        }
        else {
            BitHandler::byte_buffer = (BitHandler::byte_buffer << 1) | (bit1); //MSB
            BitHandler::byte_buffer = (BitHandler::byte_buffer << 1) | (bit2); //LSB
            BitHandler::byte_buffer_index += 2;
        }
        
    } 
    else { //Ajouter 1 bits
        BitHandler::byte_buffer = (BitHandler::byte_buffer << 1) | (bit1);
        BitHandler::byte_buffer_index += 1;
    }
}

//Sends a Manchester coded "1"
void BitHandler::sendManch_1(){
    digitalWrite(BitHandler::outputPin, HIGH);
    delay(BitHandler::half_period);
    digitalWrite(BitHandler::outputPin, LOW);
    delay(BitHandler::half_period);
}

//Sends a Manchester coded "0"
void BitHandler::sendManch_0(){
    digitalWrite(BitHandler::outputPin, LOW);
    delay(BitHandler::half_period);
    digitalWrite(BitHandler::outputPin, HIGH);
    delay(BitHandler::half_period);
}

void BitHandler::getRawMessageBytes(byte (&msgReturn)[80]) {
    int finData = 0;

    msgReturn[0] = BitHandler::startBuffer;
    msgReturn[1] = BitHandler::headerBuffer;
    for(finData = 0; finData < (BitHandler::headerBuffer); finData++){
        msgReturn[finData+2] = BitHandler::msgBuffer[finData];
    }
    msgReturn[finData+2] = BitHandler::crcBuffer[0];
    msgReturn[finData+3] = BitHandler::crcBuffer[1];
    msgReturn[finData+4] = BitHandler::endBuffer;
}

void BitHandler::resetHandler() {
    delay(10);
}