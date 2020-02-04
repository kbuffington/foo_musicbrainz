#include "stdafx.h"
#include "sha1.h"

#define SHA1CircularShift(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

void SHA1Input(SHA1Context* context, const uint8_t* message_array, uint32_t length)
{
	while (length--)
	{
		context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);
		context->Length_Low += 8;

		if (context->Message_Block_Index == 64)
		{
			SHA1ProcessMessageBlock(context);
		}

		message_array++;
	}
}

void SHA1PadMessage(SHA1Context* context)
{
	context->Message_Block[context->Message_Block_Index] = 0x80;
	std::fill(context->Message_Block.begin() + context->Message_Block_Index + 1, context->Message_Block.begin() + 56, 0);
	context->Message_Block[56] = context->Length_High >> 24;
	context->Message_Block[57] = context->Length_High >> 16;
	context->Message_Block[58] = context->Length_High >> 8;
	context->Message_Block[59] = context->Length_High;
	context->Message_Block[60] = context->Length_Low >> 24;
	context->Message_Block[61] = context->Length_Low >> 16;
	context->Message_Block[62] = context->Length_Low >> 8;
	context->Message_Block[63] = context->Length_Low;
	SHA1ProcessMessageBlock(context);
}

void SHA1ProcessMessageBlock(SHA1Context* context)
{
	auto [A, B, C, D, E] = context->Intermediate_Hash;
	static constexpr std::array<uint32_t, 4> K = { 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };
	std::array<uint32_t, 80> W;
	uint32_t temp;

	for (int t = 0; t < 80; t++)
	{
		if (t < 16) W[t] = context->Message_Block[t * 4] << 24 | context->Message_Block[t * 4 + 1] << 16 | context->Message_Block[t * 4 + 2] << 8 | context->Message_Block[t * 4 + 3];
		else W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
	}

	for (int t = 0; t < 80; t++)
	{
		if (t < 20) temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
		else if (t < 40) temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
		else if (t < 60 ) temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
		else temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];

		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
		B = A;
		A = temp;
	}

	context->Intermediate_Hash[0] += A;
	context->Intermediate_Hash[1] += B;
	context->Intermediate_Hash[2] += C;
	context->Intermediate_Hash[3] += D;
	context->Intermediate_Hash[4] += E;
	context->Message_Block_Index = 0;
}

void SHA1Result(SHA1Context* context, uint8_t Message_Digest[SHA1HashSize])
{
	SHA1PadMessage(context);
	context->Message_Block.fill(0);
	context->Length_Low = 0;
	context->Length_High = 0;

	for (int i = 0; i < SHA1HashSize; ++i)
	{
		Message_Digest[i] = context->Intermediate_Hash[i >> 2] >> 8 * (3 - (i & 0x03));
	}
}
