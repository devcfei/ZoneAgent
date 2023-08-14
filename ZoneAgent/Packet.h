#pragma once


#include <stdint.h>

#pragma pack(push,1)
//
// Packet Header
//
typedef struct _PACKET_HEADER
{
	uint32_t Size;
	uint32_t Uid;
	uint8_t Ctrl;
	uint8_t Cmd;
	uint32_t Protocol; // when Size==10, Protocol invalid, never access it
}PACKET_HEADER;


//
// Command ID
//
#define C2ZA_CMD_E2_LOGIN_REQUEST  0xE2


//
// Protocal ID
//
#define C2ZA_PROTO_1106_LOGIN_REQUEST	0x1106



#define ZA2C_PROTO_1105_LOGIN_RESP		0x1105





//
// Login Request, no encode
//	Client -> ZA
//

typedef struct _PACKET_C2ZA_LOGIN_REQUEST
{
	PACKET_HEADER Header;

	char username[21];
	char password[21];
	uint8_t unk0[12];
}PACKET_C2LA_LOGIN_REQUEST;

C_ASSERT(sizeof(PACKET_C2LA_LOGIN_REQUEST) == 0x44);

//
// Login Response , Proto ZA2C_PROTO_1105_LOGIN_RESP
//	ZA -> Client
//
struct ROLE_BRIEF
{

	char name[21];
	unsigned char used;     // slot used
	unsigned char classtype;
	unsigned char town; // 0 - read, 1 - blue
	unsigned int level;
	unsigned char wearlist[0xA0];  // !need to be decoded

};

typedef struct _PACKET_ZA2C_LOGIN_RESP
{
	PACKET_HEADER Header;


	uint8_t unknown[5];
	ROLE_BRIEF rolebrief[5];

}PACKET_ZA2C_LOGIN_RESP;

C_ASSERT(sizeof(PACKET_ZA2C_LOGIN_RESP) == 959);


int encode(unsigned char* buffer, unsigned int len);
int decode(unsigned char* buffer, unsigned int len);

#pragma pack(pop)
