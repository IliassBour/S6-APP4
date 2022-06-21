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

void sendMessage(std::string message, byte (&message_byte)[74]) {
    std::string bin_string;

    for (std::size_t i = 0; i < message.length(); ++i)
    {
        bin_string = std::bitset<8>(message.c_str()[i]).to_string();

        message_byte[i] = std::stoi(bin_string, nullptr, 2);
    }
    message_byte[message.length()] = 255;
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
    trame[2] = message.length();
    
    //Charge utile
    byte message_byte[74];
    int index = 0;
    uint8_t data[74];
    sendMessage(message, message_byte);
    while(message_byte[index] != 255 && index < 74) {
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

void sendWrongTrame(byte (&trame)[80]) {
    int lenght = trame[2];

    if(lenght < 3) {
        trame[3] += 1;
    } else {
        trame[lenght-3] += 1;
    }
}

void receiveTrame(byte trame[]) {
    //verify start
    if(trame[1] != 126){
        Serial.println("Start de la trame non valide!");
    }

    //verify crc16
    int index = 0;
    uint8_t data[74];
    while(trame[index+5] != 126 && index < 74) {
        data[index] = trame[index+3];

        index++;
    }

    uint16_t crc = calculCRC16(data, index);
    
    if((trame[index+3] << 8) + trame[index+4] == crc) {
        Serial.println("CRC16 correcte!");
    } else {
        Serial.printlnf("Trame errone! crc calcule %x != crc trame %x%x", crc, trame[index+3], trame[index+4]);
    } 

    //Extract message
    extractMessage(data, index);

    //verify end
    if(trame[index+5] != 126){
        Serial.println("End de la trame non valide!");
    }
}/**/

std::string extractMessage(uint8_t data[74], int lenght) {
    std::string message = "";

    Serial.print("Le message est : ");

    for(int i = 0; i < lenght; i++){
        message += (char) data[i];
        Serial.printf("%c", data[i]);
    }
    Serial.println("\n\n");
    
    return message;
}

