#include <stdarg.h>
#include <stdio.h>

#include "api_mysql.h"
#include "log.h"

static const int SQL_BUF_SIZ = 4096;

MysqlRowSet::MysqlRowSet(MYSQL_RES* res_set) : _res_set(res_set)
{
}

MysqlRowSet::~MysqlRowSet()
{
	if (_res_set) {
		mysql_free_result(_res_set);
	}
}

int MysqlRowSet::row_num() const
{
	return mysql_num_rows(_res_set);
}

int MysqlRowSet::col_num() const
{
	return mysql_num_fields(_res_set);
}

bool MysqlRowSet::next()
{
	_row = mysql_fetch_row(_res_set);
	return (_row != NULL);
}

std::string MysqlRowSet::get(int field_no) const
{
	return _row[field_no];
}

////////////////////////////////////////////////////////

int MysqlAPI::init(const std::string& host_ip,
				int host_port,
			   	const std::string& user,
				const std::string& passwd,
				const std::string& db
		   	   )
{
	int error = 0;
	do 
	{
		if ((_mysql_conn = mysql_init(NULL)) == NULL) 
		{
			log_error("init mysql failed.");
			error = -1;
			break;
		}

		if (mysql_real_connect(_mysql_conn, 
								host_ip.c_str(),
								user.c_str(), 
								passwd.c_str(), 
								db.c_str(), 
								host_port,
								NULL, 
								0) == NULL) {
            log_error("connect mysql failed. [%s:%d | %s-%s]", 
                    host_ip.c_str(),
                    host_port,
                    user.c_str(),
                    passwd.c_str());
			error = -1;
			break;
		}

	} while(0);

	return error;
}

MysqlRowSetPtr MysqlAPI::query(const char* fmt, ...)
{
    char sql_buf[SQL_BUF_SIZ+1] = {0};
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(sql_buf, SQL_BUF_SIZ, fmt, ap);
    va_end(ap);
    sql_buf[n] = 0;

	log_trace("select-sql: %s", sql_buf);
	int error = mysql_query(_mysql_conn, sql_buf);
	if (error) {
		log_error("mysql error:[%d][%s]", mysql_errno(_mysql_conn), mysql_error(_mysql_conn));
		return MysqlRowSetPtr(NULL);
	}

	return MysqlRowSetPtr(new MysqlRowSet(mysql_store_result(_mysql_conn)));
}

int MysqlAPI::update(const char* fmt, ...)
{
    char sql_buf[SQL_BUF_SIZ+1] = {0};
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(sql_buf, SQL_BUF_SIZ, fmt, ap);
    va_end(ap);
    sql_buf[n] = 0;

	log_trace("update-sql: %s", sql_buf);
	int error = mysql_query(_mysql_conn, sql_buf);
    if (error) {
        log_error("mysql error:[%d][%s]", mysql_errno(_mysql_conn), mysql_error(_mysql_conn));
        return -1;
    }

	return mysql_affected_rows(_mysql_conn);
}
