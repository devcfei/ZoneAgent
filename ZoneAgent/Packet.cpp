#include "framework.h"

int encode(unsigned char* buffer, unsigned int len)
{
	unsigned int m_ConstKey1 = 0x241AE7;
	unsigned int m_ConstKey2 = 0x15DCB2;
	unsigned int m_DynamicKey = 0x4C478BD;
	const int p_Type562 = 0x0C;
	const int p_Type578 = 0x0A;
	int p_DynamicKey1 = 0x02;
	int p_DynamicKey2 = 0x01;
	unsigned int p_ConstKey_En = 0xA7F0753B;
	unsigned int p_ConstKey_De = 0xAAF29BF3;

	for (unsigned int i = p_Type578; i + 4 <= len; i += 4)
	{
		if (i == p_Type578)
		{
			unsigned int wProtocol = *(unsigned int*)(buffer + i);
			wProtocol <<= 16;

			wProtocol |= ((p_DynamicKey1 << 8) + p_DynamicKey2) & 0xFFFF;
			
			wProtocol*= p_ConstKey_En;

			*(unsigned int*)(buffer + i) = wProtocol;

			p_DynamicKey2 = p_DynamicKey1 + p_DynamicKey2;
			p_DynamicKey1 = p_DynamicKey1 + 0xB3;


			continue;
		}
		int DynamicKey = m_DynamicKey;
		for (unsigned int j = i; j < i + 4; j++)
		{
			buffer[j] = (uint8_t)(buffer[j] ^ (DynamicKey >> 8));
			DynamicKey = (buffer[j] + DynamicKey) * m_ConstKey1 + m_ConstKey2;
		}
	}
	return 0;
}

int decode(unsigned char* buffer, unsigned int len)
{
	unsigned int m_ConstKey1 = 0x241AE7;
	unsigned int m_ConstKey2 = 0x15DCB2;
	unsigned int m_DynamicKey = 0x4C478BD;
	const int p_Type562 = 0x0C;
	const int p_Type578 = 0x0A;
	int p_DynamicKey1 = 0x02;
	int p_DynamicKey2 = 0x01;
	unsigned int p_ConstKey_En = 0xA7F0753B;
	unsigned int p_ConstKey_De = 0xAAF29BF3;

	//[0]-[11]: Packet Header
	for (unsigned int i = p_Type578; i + 4 <= len; i += 4)
	{
		if (i == p_Type578)
		{

			unsigned int wProtocol = *(unsigned int*)(buffer + i);
			wProtocol *= p_ConstKey_De;
			wProtocol >>= 16;

			*(unsigned int*)(buffer + i) = wProtocol;

			continue;
		}
		int DynamicKey = m_DynamicKey;
		for (unsigned int j = i; j < i + 4; j++)
		{
			unsigned char pSrc = buffer[j];
			buffer[j] = (uint8_t)(buffer[j] ^ (DynamicKey >> 8));
			DynamicKey = (pSrc + DynamicKey) * m_ConstKey1 + m_ConstKey2;
		}
	}

	return 0;
}