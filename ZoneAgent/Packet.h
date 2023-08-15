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
#define C2ZA_PROTO_1106_SELECT_ROLE			0x1106
#define C2ZA_PROTO_1107_WORLD_LOGIN			0x1107


#define ZA2C_PROTO_1105_LOGIN_RESP			0x1105
#define ZA2C_PROTO_1106_CONFIRM_ROLE		0x1106
#define ZA2C_PROTO_1107_WORLD_LOGIN_RESP	0x1107




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
	uint8_t used;     // slot used
	uint8_t classtype;
	uint8_t town; // 0 - read, 1 - blue
	uint32_t level;
	uint8_t wearlist[0xA0];  // !need to be decoded

};

typedef struct _PACKET_ZA2C_LOGIN_RESP
{
	PACKET_HEADER Header;


	uint8_t unknown[5];
	ROLE_BRIEF rolebrief[5];

}PACKET_ZA2C_LOGIN_RESP;

C_ASSERT(sizeof(PACKET_ZA2C_LOGIN_RESP) == 959);



// Select Role, Proto C2ZA_PROTO_1106_SELECT_ROLE
//	Client -> ZA
typedef struct _PACKET_C2ZA_SELECT_ROLE
{
	PACKET_HEADER Header;
	char data[50];
}PACKET_C2ZA_SELECT_ROLE;
C_ASSERT(sizeof(PACKET_C2ZA_SELECT_ROLE) == 0x40);



// Confirm Role, Proto ZA2C_PROTO_1106_CONFIRM_ROLE
//	ZA -> Client
typedef struct _PACKET_ZA2C_CONFIRM_ROLE
{
	PACKET_HEADER Header;
	uint8_t unknown;
	char name[29];
}PACKET_ZA2C_CONFIRM_ROLE;
C_ASSERT(sizeof(PACKET_ZA2C_CONFIRM_ROLE) == 0x2C);


// World Login, Proto C2ZA_PROTO_1107_WORLD_LOGIN
// Client -> ZA
typedef struct _PACKET_C2ZA_WORLD_LOGIN
{
	PACKET_HEADER Header;
	uint8_t classtype;

	char name[21];

	uint32_t mapindex;
}PACKET_C2ZA_WORLD_LOGIN;
C_ASSERT(sizeof(PACKET_C2ZA_WORLD_LOGIN) == 0x28);


// World Login Resp, Proto ZA2C_PROTO_1107_WORLD_LOGIN_RESP
//  ZA -> Client
typedef struct _PACKET_ZA2C_WORLD_LOGIN_RESP
{
	PACKET_HEADER Header;

	uint16_t unknown1; // no idea
	uint16_t MP;
	uint16_t Str;
	uint16_t Magic;
	uint16_t Dex;
	uint16_t Vit;
	uint16_t Mana;
	uint32_t MaxHPStore;
	uint32_t MaxMPStore;
	uint16_t HP;
	uint16_t Point;
	uint16_t MinAttack;
	uint16_t MinMagicAttack;
	uint16_t Defense;
	uint16_t FileAttack;
	uint16_t FileDefense;
	uint16_t IceAttack;
	uint16_t IceDefense;
	uint16_t LightningAttack;
	uint16_t LightningDefense;
	uint16_t MaxHPBar;
	uint16_t MaxMPBar;
	uint16_t MaxAttack;
	uint16_t MaxMaticAttack;
	uint16_t unknown2;
	uint8_t PetAction[20];
	char CharacterName[21];
	uint8_t Class;
	uint16_t Level;
	uint32_t Exp;
	uint32_t MapIndex;
	uint32_t MapCell;
	uint8_t Skill[28];
	uint32_t Sinfo;
	uint32_t Woonz;
	uint32_t HPStore;
	uint32_t MPStore;
	uint32_t LorePoint;
	uint16_t unknown3;
	uint8_t XX1;
	uint8_t PatInvtory[0x14 * 5];
	uint8_t XX2;
	uint8_t CharacterInvtory[0x14 * 60];
	uint8_t XX3;
	uint8_t WearList[0x14 * 10];
}PACKET_ZA2C_WORLD_LOGIN_RESP;

C_ASSERT(sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP) == 1677);


// encode/decode
int encode(uint8_t* buffer, uint32_t len);
int decode(uint8_t* buffer, uint32_t len);

#pragma pack(pop)
