#pragma once
#include "stdafx.h"
#include "global_data.h"


enum DB_event_type { DB_LOAD_INFO, DB_SAVE_XY, DB_SAVE_LEVEL, DB_SAVE_HP, DB_CREATE_USER};
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

void PrintSQLError(SQLHANDLE handle, SQLSMALLINT handleType, const std::string& context);

void DB_init(); //DB핸들 초기화, DB연결

void DB_thread(); //DB접근 스레드



