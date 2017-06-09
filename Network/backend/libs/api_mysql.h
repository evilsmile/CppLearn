#ifndef __API_MYSQL_H__
#define __API_MYSQL_H__

#include <string>
#include <memory>

#include <mysql/mysql.h>

class MysqlRowSet {
	public:
		MysqlRowSet(MYSQL_RES* res_set);
		~MysqlRowSet();

		int row_num() const;
		int col_num() const;
		bool next();
		std::string get(int field_no) const;

	private:
		MYSQL_RES* _res_set;
		MYSQL_ROW _row;
};

typedef std::auto_ptr<MysqlRowSet> MysqlRowSetPtr;

////////////////////////////////////////////////////////

class MysqlAPI {
	public:
		int init(const std::string& host_ip,
						int host_port,
						const std::string& user,
						const std::string& passwd,
						const std::string& db);
		MysqlRowSetPtr query(const char* fmt, ...);
		int update(const char* fmt, ...);
	private:
		MYSQL* _mysql_conn;

};

#endif
