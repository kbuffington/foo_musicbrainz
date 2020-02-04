#pragma once
#include <stdint.h>

#define SHA1HashSize 20

struct SHA1Context
{
	int_least16_t Message_Block_Index = 0;
	std::array<uint8_t, 64> Message_Block;
	std::array<uint32_t, 5> Intermediate_Hash = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
	uint32_t Length_High = 0;
	uint32_t Length_Low = 0;
};

void SHA1Input(SHA1Context* context, const uint8_t* message_array, uint32_t length);
void SHA1PadMessage(SHA1Context* context);
void SHA1ProcessMessageBlock(SHA1Context* context);
void SHA1Result(SHA1Context* context, uint8_t Message_Digest[SHA1HashSize]);
