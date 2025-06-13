#include "DB_Manager.h"

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
				std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER,OBJECT>(object.at(ev.id));
				switch (ev.type)
				{
				case DB_LOAD_INFO: {
					// Convert multibyte string to wide character string
					wchar_t wname[MAX_ID_LENGTH];
					size_t converted;
					mbstowcs_s(&converted, wname, MAX_ID_LENGTH, c->_name, MAX_ID_LENGTH - 1);

					// Prepare the stored procedure call
					SQLWCHAR sql_query[] = L"{CALL select_user_info(?)}"; // Stored procedure call syntax
					DBH.retcode = SQLBindParameter(DBH.hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 50, 0, wname, sizeof(wname), nullptr);
					if (!SQL_SUCCEEDED(DBH.retcode)) {
						// Handle binding error
						return;
					}

					// Execute the stored procedure directly
					DBH.retcode = SQLExecDirect(DBH.hstmt, sql_query, SQL_NTS);
					if (SQL_SUCCEEDED(DBH.retcode)) {
						// Bind the result columns
						SQLWCHAR result_user_id[MAX_ID_LENGTH];
						SQLINTEGER user_x, user_y;
						SQLLEN len_user_id = 0, len_user_x = 0, len_user_y = 0;

						SQLBindCol(DBH.hstmt, 1, SQL_C_WCHAR, result_user_id, 50, &len_user_id);
						SQLBindCol(DBH.hstmt, 2, SQL_C_LONG, &user_x, 50, &len_user_x);
						SQLBindCol(DBH.hstmt, 3, SQL_C_LONG, &user_y, 50, &len_user_y);


						// Fetch the results
						if (SQLFetch(DBH.hstmt) == SQL_SUCCESS) {
							// ±‚¡∏
							std::lock_guard<std::mutex> ll{ c->_s_lock };
			
						}
						else {
							// New user creation
							std::lock_guard<std::mutex> ll{ c->_s_lock };
							// Add logic for creating new user
						}

					}
				}
					break;
				case DB_SAVE_INFO: {

				}
				default:
					break;
				}
			}
		}
		
		this_thread::sleep_for(1ms);
	}
}
