#pragma once
constexpr short GAME_PORT = 3000;

constexpr short BUF_SIZE = 200;
constexpr short MAX_CHAT_LENGTH = 100;

constexpr int  MAX_USER = 10000;
constexpr int  NUM_MONSTER = 200000;

constexpr char S2C_P_AVATAR_INFO = 1;
constexpr char S2C_P_MOVE = 2;
constexpr char S2C_P_ENTER = 3;
constexpr char S2C_P_LEAVE = 4;
constexpr char C2S_P_LOGIN = 5;
constexpr char C2S_P_MOVE = 6;
constexpr char S2C_P_CHAT = 7;
constexpr char S2C_P_STAT_CHANGE = 8;
constexpr char S2C_P_LOGIN_FAIL = 9;
constexpr char C2S_P_ATTACK = 10;
constexpr char C2S_P_CHAT = 11;
constexpr char C2S_P_TELEPORT = 12;		// ���� �׽�Ʈ �� ��

constexpr char S2C_P_STATE = 13;
constexpr char C2S_P_STATE = 14;

// ���۸����� HOTSPOT�� �����ϱ� ���� 
// RANDOM TELEPORT�� �� ���

constexpr char MAX_ID_LENGTH = 20;

constexpr char MOVE_UP = 1;
constexpr char MOVE_DOWN = 2;
constexpr char MOVE_LEFT = 3;
constexpr char MOVE_RIGHT = 4;

constexpr char ACTION_ATTACK = 0;
constexpr char ACTION_ATTACK_SKILL = 1;
constexpr char ACTION_HEAL_SKILL = 2;

constexpr unsigned short MAP_HEIGHT = 2000;
constexpr unsigned short MAP_WIDTH = 2000;

constexpr unsigned short init_x = 1050;
constexpr unsigned short init_y = 1038;

//type, state
constexpr int PLAYER = 0;
constexpr int ORC_NPC = 1;
constexpr int HUMAN = 2;
constexpr int S_HUMAN = 3;

constexpr int IDLE = 0;
constexpr int WALK = 1;
constexpr int HURT = 2;
constexpr int ATTACK = 3;
constexpr int DEATH = 4;

//npc
inline int MaxHP(int level) {
	if (level < 1) return 80; // �߰� ����
	return 80 + (level * 20);
}
inline int Dammage(int level) {
	return 7 + (level * 3);
}
inline int need_next_level_exp(int level) {
	int need_exps[10] = { 0,100,200,400,800,1200,1500,2000,3000,5000 };
	int my_level = (level - 1 > 9) ? 9 : level - 1;
	return need_exps[my_level];
}
constexpr short NPC_MAX_HP[2] = { 10,35 };
constexpr short NPC_EXP[2] = { 50,80 };
constexpr short NPC_DAMMAGE[2] = { 5,10 };
constexpr short npc_state_dir[4] = { MOVE_DOWN,MOVE_RIGHT,MOVE_LEFT,MOVE_UP };

#pragma pack (push, 1)

struct sc_packet_avatar_info {
	unsigned char size;
	char type;
	char name[MAX_ID_LENGTH];
	long long  id;
	short x, y;
	short max_hp;
	short hp;
	short level;
	int   exp;
};

struct sc_packet_move {
	unsigned char size;
	char type;
	long long id;
	short x, y;
};

struct sc_packet_enter {
	unsigned char size;
	char type;
	long long  id;
	char name[MAX_ID_LENGTH];
	char o_type;
	short x, y;
	short hp; //���������� �׸������� ����
};

struct sc_packet_leave {
	unsigned char size;
	char type;
	long long  id;
};

struct sc_packet_chat {
	unsigned char size;
	char type;
	long long  id;						// �޼����� ���� Object�� ID
	// -1 => SYSTEM MESSAGE
	//       ���� �޼��� ������ ���
	char message[MAX_CHAT_LENGTH];		// NULL terminated ���ڿ�
};

struct sc_packet_stat_change {
	unsigned char size;
	char type;
	long long  id;
	short hp;
	short level;
	int   exp;
};

struct sc_packet_login_fail {
	unsigned char size;
	char type;
	long long  id;
	char reason;			// 0 : �˼� ���� ����
	// 1 : �ٸ� Ŭ���̾�Ʈ���� �����
	// 2 : �������� ID (Ư������, 20�� �̻�)
	// 3 : ������ ������ �ʹ� ����
};
//�߰� ��Ŷ

//�ִϸ��̼� ����ȭ�� ��Ŷ, ���� ������ �������� �÷��̾������ �Ѹ���.
struct sc_packet_state { 
	unsigned char size;
	char type;
	long long id;
	int state;
	char  direction;
};



//====================================
struct cs_packet_login {
	unsigned char  size;
	char  type;
	char  name[MAX_ID_LENGTH];
};

struct cs_packet_move {
	unsigned char  size;
	char  type;
	char  direction;
};

struct cs_packet_attack {
	unsigned char  size;
	char  type;
	char  direction;
};

struct cs_packet_chat {
	unsigned char  size;
	char  type;
	char  message[MAX_CHAT_LENGTH];		// NULL terminated ���ڿ�
};

struct cs_packet_teleport {
	unsigned char  size;
	char  type;
};

//�߰���Ŷ
struct cs_packet_state {
	unsigned char  size;
	char  type;
	int state;
	char  direction;
};

#pragma pack (pop)

