#include "DB_Manager.h"
#include "network_manager.h"
DB_Handle DBH;
concurrency::concurrent_queue <DB_event> DBQ;

void DB_init()
{

	//DB connect 
	{
		DBH.retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &DBH.henv);
		if (DBH.retcode == SQL_SUCCESS || DBH.retcode == SQL_SUCCESS_WITH_INFO) {
			DBH.retcode = SQLSetEnvAttr(DBH.henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

			// Allocate connection handle  
			if (DBH.retcode == SQL_SUCCESS || DBH.retcode == SQL_SUCCESS_WITH_INFO) {
				DBH.retcode = SQLAllocHandle(SQL_HANDLE_DBC, DBH.henv, &DBH.hdbc);

				// Set login timeout to 5 seconds  
				if (DBH.retcode == SQL_SUCCESS || DBH.retcode == SQL_SUCCESS_WITH_INFO) {
					SQLSetConnectAttr(DBH.hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

					// Connect to data source  
					DBH.retcode = SQLConnect(DBH.hdbc, (SQLWCHAR*)L"2022180029_termODBC", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);


				}
			}
		}
	}

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
					SQLWCHAR sql_query[] = L"{CALL select_user_info(?)}"; // Stored procedure call syntax
					DBH.retcode = SQLBindParameter(DBH.hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 50, 0, wname, sizeof(wname), nullptr);
					if (!SQL_SUCCEEDED(DBH.retcode)) {return;}
					DBH.retcode = SQLExecDirect(DBH.hstmt, sql_query, SQL_NTS);
					if (SQL_SUCCEEDED(DBH.retcode)) {
						SQLWCHAR result_user_id[MAX_ID_LENGTH];
						SQLINTEGER user_x, user_y,user_level,user_hp;
						SQLLEN len_user_id = 0, len_user_x = 0, len_user_y = 0, len_user_level = 0, len_user_hp = 0;
						SQLBindCol(DBH.hstmt, 1, SQL_C_WCHAR, result_user_id, 50, &len_user_id);
						SQLBindCol(DBH.hstmt, 2, SQL_C_LONG, &user_x, 50, &len_user_x);
						SQLBindCol(DBH.hstmt, 3, SQL_C_LONG, &user_y, 50, &len_user_y);
						SQLBindCol(DBH.hstmt, 4, SQL_C_LONG, &user_level, 50, &len_user_level);
						SQLBindCol(DBH.hstmt, 5, SQL_C_LONG, &user_hp, 50, &len_user_hp);
						if (SQLFetch(DBH.hstmt) == SQL_SUCCESS) {
							//扁粮蜡历老版快
							std::lock_guard<std::mutex> ll{ c->_s_lock };
						}
						else {
							// 货蜡历老版快
							std::lock_guard<std::mutex> ll{ c->_s_lock };
						}

					}
				}
					break;
				case DB_SAVE_XY: {
					SQLWCHAR sql_query[] = L"{CALL update_user_xy(?)(?)(?)}"; // Stored procedure call syntax
					SQLSMALLINT sx = c->x; SQLSMALLINT sy = c->y;
					DBH.retcode = SQLBindParameter(DBH.hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 50, 0, wname, sizeof(wname), nullptr);
					DBH.retcode = SQLBindParameter(DBH.hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &sx, sizeof(SQLINTEGER), nullptr);
					DBH.retcode = SQLBindParameter(DBH.hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &sy, sizeof(SQLINTEGER), nullptr);
					DBH.retcode = SQLExecDirect(DBH.hstmt, sql_query, SQL_NTS);
					if (SQL_SUCCEEDED(DBH.retcode)) {
					
					}
				}
				default:
					break;
				}
			}
		}
		
		this_thread::sleep_for(1ms);
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

