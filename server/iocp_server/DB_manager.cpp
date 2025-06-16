#include "DB_Manager.h"
#include "network_manager.h"
DB_Handle DBH;
concurrency::concurrent_queue <DB_event> DBQ;

void PrintSQLError(SQLHANDLE handle, SQLSMALLINT handleType, const std::string& context)
{
	SQLWCHAR sqlState[6];
	SQLINTEGER nativeError;
	SQLWCHAR messageText[1024];
	SQLSMALLINT messageLength;

	SQLRETURN ret = SQLGetDiagRec(handleType, handle, 1, sqlState, &nativeError,
		messageText, sizeof(messageText) / sizeof(SQLWCHAR), &messageLength);
	if (SQL_SUCCEEDED(ret)) {
		std::wcerr << L"Error in " << std::wstring(context.begin(), context.end())
			<< L": SQLSTATE=" << sqlState
			<< L", NativeError=" << nativeError
			<< L", Message=" << messageText << std::endl;
	}
	else {
		std::cerr << "Error in " << context << ": Failed to retrieve diagnostics" << std::endl;
	}
}

void DB_init()
{
	DBH.retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &DBH.henv);
	if (!SQL_SUCCEEDED(DBH.retcode)) {
		PrintSQLError(DBH.henv, SQL_HANDLE_ENV, "SQLAllocHandle (ENV)");
	}

	// 2. ODBC 버전 설정
	DBH.retcode = SQLSetEnvAttr(DBH.henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	if (!SQL_SUCCEEDED(DBH.retcode)) {
		PrintSQLError(DBH.henv, SQL_HANDLE_ENV, "SQLSetEnvAttr");
		SQLFreeHandle(SQL_HANDLE_ENV, DBH.henv);
		DBH.henv = nullptr;
	}

	// 3. 연결 핸들 할당
	DBH.retcode = SQLAllocHandle(SQL_HANDLE_DBC, DBH.henv, &DBH.hdbc);
	if (!SQL_SUCCEEDED(DBH.retcode)) {
		PrintSQLError(DBH.henv, SQL_HANDLE_ENV, "SQLAllocHandle (DBC)");
		SQLFreeHandle(SQL_HANDLE_ENV, DBH.henv);
		DBH.henv = nullptr;
	}

	// 4. 로그인 타임아웃 설정
	DBH.retcode = SQLSetConnectAttr(DBH.hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
	if (!SQL_SUCCEEDED(DBH.retcode)) {
		PrintSQLError(DBH.hdbc, SQL_HANDLE_DBC, "SQLSetConnectAttr");
		SQLFreeHandle(SQL_HANDLE_DBC, DBH.hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, DBH.henv);
		DBH.hdbc = nullptr;
		DBH.henv = nullptr;
	}

	// 5. 데이터베이스 연결
	DBH.retcode = SQLConnect(DBH.hdbc, (SQLWCHAR*)L"2022180029_termODBC", SQL_NTS,
		(SQLWCHAR*)nullptr, 0, nullptr, 0);
	if (!SQL_SUCCEEDED(DBH.retcode)) {
		PrintSQLError(DBH.hdbc, SQL_HANDLE_DBC, "SQLConnect");
		SQLFreeHandle(SQL_HANDLE_DBC, DBH.hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, DBH.henv);
		DBH.hdbc = nullptr;
		DBH.henv = nullptr;
	}

	// 6. 문 핸들 할당
	DBH.retcode = SQLAllocHandle(SQL_HANDLE_STMT, DBH.hdbc, &DBH.hstmt);
	if (!SQL_SUCCEEDED(DBH.retcode)) {
		PrintSQLError(DBH.hdbc, SQL_HANDLE_DBC, "SQLAllocHandle (STMT)");
		SQLDisconnect(DBH.hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, DBH.hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, DBH.henv);
		DBH.hdbc = nullptr;
		DBH.henv = nullptr;
	}

	std::cout << "ODBC initialization and connection successful" << std::endl;


}

void DB_thread()
{
	while (true)
	{
		
		if (!DBQ.empty()) {
			DB_event ev;
			if (DBQ.try_pop(ev)) {
				auto it = object.find(ev.id);
				if (it == object.end()) break;
				std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
				wchar_t wname[MAX_ID_LENGTH]; size_t converted;
				mbstowcs_s(&converted, wname, MAX_ID_LENGTH, c->_name, MAX_ID_LENGTH - 1);

				switch (ev.type)
				{
				case DB_LOAD_INFO: {
				
					SQLWCHAR sql_query[] = L"{CALL select_user_info(?)}";

					// 파라미터 바인딩
					SQLRETURN retcode = SQLBindParameter(DBH.hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
						50, 0, (SQLPOINTER)wname, sizeof(wname), nullptr);
					if (!SQL_SUCCEEDED(retcode)) {
						PrintSQLError(DBH.hstmt, SQL_HANDLE_STMT, "SQLBindParameter");
						return;
					}

					// 저장 프로시저 실행
					retcode = SQLExecDirect(DBH.hstmt, sql_query, SQL_NTS);
					if (!SQL_SUCCEEDED(retcode)) {
						PrintSQLError(DBH.hstmt, SQL_HANDLE_STMT, "SQLExecDirect");
						return;
					}

					// 결과 바인딩
					SQLWCHAR result_user_id[MAX_ID_LENGTH];
					SQLINTEGER user_x, user_y, user_level, user_hp ,user_exp;
					SQLLEN len_user_id = 0, len_user_x = 0, len_user_y = 0,
						len_user_level = 0, len_user_hp = 0, len_user_exp =0;

					retcode = SQLBindCol(DBH.hstmt, 1, SQL_C_WCHAR, result_user_id, sizeof(result_user_id), &len_user_id);
					retcode = SQLBindCol(DBH.hstmt, 2, SQL_C_LONG, &user_x, sizeof(SQLINTEGER), &len_user_x);
					retcode = SQLBindCol(DBH.hstmt, 3, SQL_C_LONG, &user_y, sizeof(SQLINTEGER), &len_user_y);
					retcode = SQLBindCol(DBH.hstmt, 4, SQL_C_LONG, &user_level, sizeof(SQLINTEGER), &len_user_level);
					retcode = SQLBindCol(DBH.hstmt, 5, SQL_C_LONG, &user_hp, sizeof(SQLINTEGER), &len_user_hp);
					retcode = SQLBindCol(DBH.hstmt, 6, SQL_C_LONG, &user_exp, sizeof(SQLINTEGER), &len_user_exp);

					// 결과 페치
					retcode = SQLFetch(DBH.hstmt);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						//워커한테 보내줘야댐
				        OVER_EXP* exover = new OVER_EXP;
						exover->_comp_type = OP_DB_LOAD_USER;
						exover->result_info.x = (int)user_x;
						exover->result_info.y= (int)user_y;
						exover->result_info.level = (int)user_level;
						exover->result_info.hp = (int)user_hp;
						exover->result_info.exp = (int)user_exp;
						PostQueuedCompletionStatus(h_iocp, 1 ,c->_id, &exover->_over);
					}
					else if (retcode == SQL_NO_DATA) {
						OVER_EXP* exover = new OVER_EXP;
						exover->_comp_type = OP_DB_NEW_USER;
						PostQueuedCompletionStatus(h_iocp, 1, c->_id, &exover->_over);
					}
					else {
						PrintSQLError(DBH.hstmt, SQL_HANDLE_STMT, "SQLFetch");

					}
					SQLCloseCursor(DBH.hstmt);
				}
					break;
				case DB_SAVE_XY: {
					SQLWCHAR sql_query[] = L"{CALL update_user_xy(?, ?, ?)}"; 
					SQLINTEGER sx = c->x; 
					SQLINTEGER sy = c->y;
					SQLRETURN retcode = SQLBindParameter(DBH.hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 50, 0, wname, sizeof(wname), nullptr);
					retcode = SQLBindParameter(DBH.hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &sx, sizeof(SQLINTEGER), nullptr);
					retcode = SQLBindParameter(DBH.hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &sy, sizeof(SQLINTEGER), nullptr);
					retcode = SQLExecDirect(DBH.hstmt, sql_query, SQL_NTS);
					SQLCloseCursor(DBH.hstmt);
					break;
				}
				case DB_CREATE_USER: {
					SQLWCHAR sql_query[] = L"{CALL create_user(?, ?, ?)}"; 
					SQLINTEGER sx = c->x; 
					SQLINTEGER sy = c->y;
					SQLRETURN retcode = SQLBindParameter(DBH.hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 50, 0, wname, sizeof(wname), nullptr);
					retcode = SQLBindParameter(DBH.hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &sx, sizeof(SQLINTEGER), nullptr);
					retcode = SQLBindParameter(DBH.hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &sy, sizeof(SQLINTEGER), nullptr);
					retcode = SQLExecDirect(DBH.hstmt, sql_query, SQL_NTS);
					SQLCloseCursor(DBH.hstmt);
					break;
				}
				case DB_SAVE_LEVEL: {
					SQLWCHAR sql_query[] = L"{CALL update_user_level(?, ?, ?)}"; 
					SQLINTEGER slevel = c->_level; 
					SQLINTEGER sexp = c->_exp; 
					SQLRETURN retcode = SQLBindParameter(DBH.hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 50, 0, wname, sizeof(wname), nullptr);
					retcode = SQLBindParameter(DBH.hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &slevel, sizeof(SQLINTEGER), nullptr);
					retcode = SQLBindParameter(DBH.hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &sexp, sizeof(SQLINTEGER), nullptr);
					retcode = SQLExecDirect(DBH.hstmt, sql_query, SQL_NTS);
					SQLCloseCursor(DBH.hstmt);
					break;
				}
				case DB_SAVE_HP: {
					SQLWCHAR sql_query[] = L"{CALL update_user_hp(?, ?)}";
					SQLINTEGER shp = c->_hp;
					SQLRETURN retcode = SQLBindParameter(DBH.hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 50, 0, wname, sizeof(wname), nullptr);
					retcode = SQLBindParameter(DBH.hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &shp, sizeof(SQLINTEGER), nullptr);
					retcode = SQLExecDirect(DBH.hstmt, sql_query, SQL_NTS);
					SQLCloseCursor(DBH.hstmt);
					break;
				}
				default:
					break;
				}
			}
		}
		
	}
}


void USER::do_recv()
{
	DWORD recv_flag = 0;
	memset(&_recv_over._over, 0, sizeof(_recv_over._over));
	_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
	_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
	WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
		&_recv_over._over, 0);
}

