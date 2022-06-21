#include "Particle.h"
#include <string>

std::string toBinary(int n);
void sendMessage(std::string message, byte (&message_byte)[75]);
uint16_t calculCRC16(const uint8_t *data, uint16_t size);
void sendTrame(std::string message, byte (&trame)[80]);
void receiveTrame(byte trame[]);