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
#define C2ZA_PROTO_1200_ROLE_MOVE			0x1200
#define C2ZA_PROTO_1202_ROLE_MOVE_DONE		0x1202
#define C2ZA_PROTO_1400_ATTACK_MONSTER		0x1400
#define C2ZA_PROTO_1461_EXIT_GAME			0x1461
#define C2ZA_PROTO_1800_MESSAGE				0x1800


#define ZA2C_PROTO_1105_LOGIN_RESP			0x1105
#define ZA2C_PROTO_1106_CONFIRM_ROLE		0x1106
#define ZA2C_PROTO_1107_WORLD_LOGIN_RESP	0x1107
#define ZA2C_PROTO_1108_WORLD_EXIT			0x1108
#define ZA2C_PROTO_1200_ROLE_MOVE_RESP		0x1200
#define ZA2C_PROTO_1300_MOSNTER_NEW			0x1300
#define ZA2C_PROTO_1400_ATTACK_MONSTER_RESP	0x1400
#define ZA2C_PROTO_1800_MESSAGE				0x1800



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
	uint8_t wearlist[160];  // !need to be decoded

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
	char unknown[5];
	char name[45];
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
	uint16_t FireAttack;
	uint16_t FireDefense;
	uint16_t IceAttack;
	uint16_t IceDefense;
	uint16_t LightningAttack;
	uint16_t LightningDefense;
	uint16_t MaxHPBar;
	uint16_t MaxMPBar;
	uint16_t MaxAttack;
	uint16_t MaxMagicAttack;
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
	uint8_t PatInventory[0x14 * 5];
	uint8_t XX2; 
	uint8_t CharacterInventory[0x14 * 60];
	uint8_t XX3;
	uint8_t WearList[0x14 * 10];
}PACKET_ZA2C_WORLD_LOGIN_RESP;

C_ASSERT(sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP) == 1677);



// World Exit, Proto ZA2C_PROTO_1108_WORLD_EXIT
//  ZA -> Client
typedef struct _PACKET_ZA2C_WORLD_EXIT
{
	PACKET_HEADER Header;
	uint8_t fix0;

}PACKET_ZA2C_WORLD_EXIT;

C_ASSERT(sizeof(PACKET_ZA2C_WORLD_EXIT) == 0xF);


// Role Move , Proto C2ZA_PROTO_1200_ROLE_MOVE
// Client -> ZA
typedef struct _PACKET_C2ZA_ROLE_MOVE
{
	PACKET_HEADER Header;
	uint8_t unknown1;
	uint8_t unknown2;
	uint8_t unknown3;
	uint8_t destX;
	uint8_t destY;
	uint8_t tail[6];
}PACKET_C2ZA_ROLE_MOVE;

C_ASSERT(sizeof(PACKET_C2ZA_ROLE_MOVE) == 25);


// Role Move Resp, Proto ZA2C_PROTO_1200_ROLE_MOVE_RESP
//  ZA -> Client
typedef struct _PACKET_ZA2C_ROLE_MOVE_RESP
{
	PACKET_HEADER Header;
	uint8_t unknown1; // 
	uint8_t unknown2; // 
	uint8_t unknown3; // 
	uint8_t unknown4; // 
	uint8_t destX;
	uint8_t destY;
	uint8_t tail[3];
}PACKET_ZA2C_ROLE_MOVE_RESP;

C_ASSERT(sizeof(PACKET_ZA2C_ROLE_MOVE_RESP) == 23);

// Monster new , Proto ZA2C_PROTO_1300_MOSNTER_NEW
// ZA -> Client
typedef struct _PACKET_ZA2C_MOSNTER_NEW
{
	PACKET_HEADER Header;
	uint32_t Fix1[2];
	uint8_t FixF[3];
	uint8_t Fix0[5];
	uint32_t ID;	// Monster ID
	uint8_t Unk[30];
	uint16_t Level; // Monster Level
	uint16_t Unk1;
	uint8_t Unk2[3];
	uint16_t sn;
	uint16_t id;
	uint32_t Fix0000[2];
	uint8_t X;
	uint8_t Y;
	uint16_t Fix00;

}PACKET_ZA2C_MOSNTER_NEW;

C_ASSERT(sizeof(PACKET_ZA2C_MOSNTER_NEW) == 0x57);



// Attack Monster , Proto C2ZA_PROTO_1400_ATTACK_MONSTER
// Client -> ZA

typedef struct _PACKET_C2ZA_ATTACK_MONSTER
{
	PACKET_HEADER Header;
	uint8_t unknown[2];
	uint16_t sn;	//
	uint16_t id;
	uint8_t tail[20];
}PACKET_C2ZA_ATTACK_MONSTER;

C_ASSERT(sizeof(PACKET_C2ZA_ATTACK_MONSTER) == 0x28);


// Attack Monster Resp , Proto ZA2C_PROTO_1400_ATTACK_MONSTER_RESP
// ZA -> Client
#define MOSNTER_STATUS_ALIVE 1
#define MOSNTER_STATUS_DEAD	 2
#define MOSNTER_STATUS_PK	 3
typedef struct  _PACKET_ZA2C_ATTACK_MONSTER_RESP
{
	PACKET_HEADER Header;
	uint8_t cx;		// Character position X
	uint8_t cy;		// Character position Y
	uint16_t fix0; //  fix to 0, and must set
	uint8_t unused1[2]; // seems unused
	uint8_t mx;		// Monster postion X
	uint8_t my;		// Monster postion Y
	uint8_t unused2[2]; // seems unused
	uint32_t fix2;  // fixed to 2 , and must set
	uint16_t sn;
	uint16_t id;
	uint16_t status;//  MOSNTER_STATUS_
	uint16_t count;	// Attacked count
	uint16_t cmp;	// Character MP
	uint8_t unk0[5];
	
	uint16_t damage;  // Damage value
	uint8_t unk1;
	uint16_t chp;	// Character HP
	uint8_t unk2;
	uint8_t mhpp;   // Monster HP percent, 255-full, 0-empty


}PACKET_ZA2C_ATTACK_MONSTER_RESP;

C_ASSERT(sizeof(PACKET_ZA2C_ATTACK_MONSTER_RESP) == 0x32);



// Message , Proto C2ZA_PROTO_1800_MESSAGE
// Client -> ZA
typedef struct _PACKET_C2ZA_MESSAGE
{
	PACKET_HEADER Header;
	uint16_t Type;
	uint8_t Unk[23];
	char message[0x79];
}PACKET_C2ZA_MESSAGE;

C_ASSERT(sizeof(_PACKET_C2ZA_MESSAGE) == 0xA0);


// Message , Proto ZA2C_PROTO_1800_MESSAGE
// ZA -> Client
typedef struct _PACKET_ZA2C_MESSAGE
{
	PACKET_HEADER Header;
	uint32_t tag0;
	uint16_t tag1;
	char message[88];
}PACKET_ZA2C_MESSAGE;
C_ASSERT(sizeof(PACKET_ZA2C_MESSAGE) == 108);







// encode/decode
int encode(uint8_t* buffer, uint32_t len);
int decode(uint8_t* buffer, uint32_t len);

#pragma pack(pop)
