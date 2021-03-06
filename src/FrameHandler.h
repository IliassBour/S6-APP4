#include "Particle.h"
#include <string>

std::string toBinary(int n);
void sendMessage(std::string message, byte (&message_byte)[74]);
uint16_t calculCRC16(const uint8_t *data, uint16_t size);
int sendTrame(std::string message, byte (&trame)[80]);
void sendWrongTrame(byte (&trame)[80]);
void receiveTrame(byte trame[]);
std::string extractMessage(uint8_t data[74], int lenght);
