#include "FrameHandler.h"
#include <string>
#include <bitset>

#define CRC16 0x8005

std::string toBinary(int n)
{
    std::string r;
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
    return r;
}

void sendMessage(std::string message, byte (&message_byte)[75]) {
    Serial.printlnf("message : %c", message.c_str()[0]);
    std::string bin_string;

    for (std::size_t i = 0; i < message.length(); ++i)
    {
        bin_string = std::bitset<8>(message.c_str()[i]).to_string();

        message_byte[i] = std::stoi(bin_string, nullptr, 2);
    }
    message_byte[message.length()] = 255;
}

void sendTrame(std::string message, byte (&trame)[80]) {
    std::string temp_bin = "";

    //Préambule
    for(int i = 0; i < 8; i++){
        if(i%2 == 0){
            temp_bin += "0";
        }
        else{
            temp_bin += "1";
        }
    } 
    trame[0] = std::stoi(temp_bin, nullptr, 2);

    //Start
    temp_bin = "0";
    for(int i = 0; i < 6; i++){
        temp_bin += "1";
    }
    temp_bin += "0";
    trame[1] = std::stoi(temp_bin, nullptr, 2);

    //Entête (type + flags) -> 11000011
    temp_bin = "11";
    for(int i = 0; i < 4; i++){
        temp_bin += "0";
    }
    temp_bin += "11";
    trame[2] = std::stoi(temp_bin, nullptr, 2);
    
    //Charge utile
    byte message_byte[75];
    int index = 0;
    uint8_t data[75];
    sendMessage(message, message_byte);
    while(message_byte[index] != 255 && index < 75) {
        trame[3+index] = message_byte[index];
        data[index] = (uint8_t) message_byte[index];
        index++;
    }

    //Contrôle (CRC16)
    uint16_t crc = calculCRC16(data, index);
    trame[index+3] = crc >> 8;
    trame[index+4] = crc & 0xFF;

    //End
    temp_bin = "0";
    for(int i = 0; i < 6; i++){
        temp_bin += "1";
    }
    temp_bin += "0";
    trame[index+5] = std::stoi(temp_bin, nullptr, 2);
}

uint16_t calculCRC16(const uint8_t *data, uint16_t size) {
    uint16_t out = 0;
    int bits_read = 0, bit_flag;

    /* Sanity check: */
    if(data == NULL)
        return 0;

    while(size > 0)
    {
        bit_flag = out >> 15;

        /* Get next bit: */
        out <<= 1;
        out |= (*data >> bits_read) & 1; // item a) work from the least significant bits

        /* Increment bit counter: */
        bits_read++;
        if(bits_read > 7)
        {
            bits_read = 0;
            data++;
            size--;
        }

        /* Cycle check: */
        if(bit_flag)
            out ^= CRC16;

    }

    // item b) "push out" the last 16 bits
    int i;
    for (i = 0; i < 16; ++i) {
        bit_flag = out >> 15;
        out <<= 1;
        if(bit_flag)
            out ^= CRC16;
    }

    // item c) reverse the bits
    uint16_t crc = 0;
    i = 0x8000;
    int j = 0x0001;
    for (; i != 0; i >>=1, j <<= 1) {
        if (i & out) crc |= j;
    }

    return crc;
}

void receiveTrame(byte trame[]) {
    //calcul clk

    //verify entete
    //verify flag

    //Extract message
    //verify crc16
    //verify end
}/**/