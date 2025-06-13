#pragma once
#include "stdafx.h"



enum DB_event_type { DB_LOAD_INFO, DB_SAVE_INFO};
struct DB_event {
	DB_event_type type;
	int id;
};

struct DB_Handle
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

};

extern DB_Handle DBH;

extern concurrency::concurrent_queue <DB_event> DBQ;



void DB_init(); //DB핸들 초기화, DB연결

void DB_thread(); //DB접근 스레드

