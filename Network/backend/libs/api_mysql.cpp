#include "api_mysql.h"
#include "log.h"

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
			log_error("connect mysql failed.");
			error = -1;
			break;
		}

	} while(0);

	return error;
}

MysqlRowSetPtr MysqlAPI::query(const std::string& sql)
{
	log_trace("select-sql: %s", sql.c_str());
	int error = mysql_query(_mysql_conn, sql.c_str());
	if (error) {
		log_error("query failed");
		return MysqlRowSetPtr(NULL);
	}

	return MysqlRowSetPtr(new MysqlRowSet(mysql_store_result(_mysql_conn)));
}

int MysqlAPI::update(const std::string& sql)
{
	log_trace("update-sql: %s", sql.c_str());
	mysql_query(_mysql_conn, sql.c_str());
	return mysql_affected_rows(_mysql_conn);
}
