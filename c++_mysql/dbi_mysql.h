
#ifndef __DBIMYSQL_H__
#define __DBIMYSQL_H__



#include <mysql.h>

// DBI接口返回码定义
typedef enum dbi_ret_code
{
	DBI_ERROR = -1,				// 操作错误，可以调用 dbi_error方法获取错误信息
	DBI_SUCCESS = 0,			// 操作成功
	DBI_FETCH_EOF = 1,			// 游标到达结果集尾部，遍历完毕
	DBI_PRIMARYKEY_EXIST = 2,	// 主键记录已存在
}DBI_RET_CODE;

//数据库操作句柄
typedef struct dbi_prepare_t
{
	void* p_query;

	dbi_prepare_t() { p_query = 0;}
} dbi_prepare_t;

//flag取值字典
typedef enum DBI_FLAG_TYPE
{
    DBI_FLAG_UNDEF = -2,
    DBI_FLAG_NULL = -1,            /* 标识一个值为null 值*/
    DBI_FLAG_NOTNULL = 0,          /* 标识一个数据为非null值 */
    DBI_FLAG_TRUNCATE = 1          /* 表示数据被数据库截断*/
} dbi_flag_t;

/*
* DBI日期时间结构
*/
typedef struct _dbi_date
{
	unsigned short year;
	unsigned short month;
	unsigned short day;

	unsigned short hour;
	unsigned short minute;
	unsigned short second;
	unsigned short frac;
}DBI_DATE;


/**********************************************************/

struct mysql_preparement
{
	MYSQL_STMT*		stmt;
//	MYSQL_RES*	result_metadata;
//	MYSQL_FIELD*	result_fields;	/* 字段信息 */

	MYSQL_BIND*		bind_input;
	MYSQL_BIND*		bind_output;

	int param_count;
	int field_count;
};

/**********************************************************/

class DBI_mysql
{
public:
	DBI_mysql(void);
	~DBI_mysql(void);

	//连接
	int db_connect(const char* host, const char* database, const char* usr, const char* pwd, int port=0);
	void db_disconnect(void);

	//事务
	 bool db_autocommit(bool flag);
	 int db_begin(void);
	 int db_rollback(void);
	 int db_commit(void);
	 int db_lock_wait(int secs);

	 int db_execute(const char* sql);
	 int db_fetch(void);
	 bool db_next(void);
	 void db_close_cursor(void);

	 int db_get_field(int pos, char* val, int size);
	 int db_get_field(int pos, int& val);
	 int db_get_field(int pos, long long& val);
	 int db_get_field(int pos, float& val);
	 int db_get_field(int pos, double& val);
	 int db_get_field(int pos, DBI_DATE& val);

	 int db_prepare_sql(dbi_prepare_t* q, const char* sql);
	 int db_free_sql(dbi_prepare_t* q);

	 int db_execute(dbi_prepare_t* q);
	 int db_fetch(dbi_prepare_t* q);
	 int db_close_cursor(dbi_prepare_t* q);

	 int db_set_param(dbi_prepare_t* q, int pos, const char* val, bool is_null);
	 int db_set_param(dbi_prepare_t* q, int pos, const char* val, int size, bool is_null);

	 int db_set_param(dbi_prepare_t* q, int pos, short val, bool is_null);
	 int db_set_param(dbi_prepare_t* q, int pos, int val, bool is_null);
	 int db_set_param(dbi_prepare_t* q, int pos, long val, bool is_null);
	 int db_set_param(dbi_prepare_t* q, int pos, long long val, bool is_null);

	 int db_set_param(dbi_prepare_t* q, int pos, unsigned short val, bool is_null);
	 int db_set_param(dbi_prepare_t* q, int pos, unsigned int val, bool is_null);
	 int db_set_param(dbi_prepare_t* q, int pos, unsigned long val, bool is_null);
	 int db_set_param(dbi_prepare_t* q, int pos, unsigned long long val, bool is_null);

	 int db_set_param(dbi_prepare_t* q, int pos, float val, bool is_null);
	 int db_set_param(dbi_prepare_t* q, int pos, double val, bool is_null);

	 int db_set_param(dbi_prepare_t* q, int pos, const DBI_DATE& val, bool is_null);

	 int db_set_param(dbi_prepare_t* q, int pos, void* val, int size, bool is_null);

	 int db_get_field(dbi_prepare_t* q, int pos, char* val, int size, dbi_flag_t* flag);

	 int db_get_field(dbi_prepare_t* q, int pos, short& val, dbi_flag_t* flag);
	 int db_get_field(dbi_prepare_t* q, int pos, int& val, dbi_flag_t* flag);
	 int db_get_field(dbi_prepare_t* q, int pos, long& val, dbi_flag_t* flag);
	 int db_get_field(dbi_prepare_t* q, int pos, long long& val, dbi_flag_t* flag);

	 int db_get_field(dbi_prepare_t* q, int pos, unsigned short& val, dbi_flag_t* flag);
	 int db_get_field(dbi_prepare_t* q, int pos, unsigned int& val, dbi_flag_t* flag);
	 int db_get_field(dbi_prepare_t* q, int pos, unsigned long& val, dbi_flag_t* flag);
	 int db_get_field(dbi_prepare_t* q, int pos, unsigned long long& val, dbi_flag_t* flag);

	 int db_get_field(dbi_prepare_t* q, int pos, float& val, dbi_flag_t* flag);
	 int db_get_field(dbi_prepare_t* q, int pos, double& val, dbi_flag_t* flag);

	 int db_get_field(dbi_prepare_t* q, int pos, DBI_DATE& val, dbi_flag_t* flag);

	 int db_get_field(dbi_prepare_t* q, int pos, void* buf, unsigned long bufsize, unsigned long& readed, dbi_flag_t* flag);

	 int db_bind_param(dbi_prepare_t* q, int pos, char*& val);
	 int db_bind_param(dbi_prepare_t* q, int pos, int*& val);
	 int db_bind_param(dbi_prepare_t* q, int pos, long int*& val);
	 int db_bind_param(dbi_prepare_t* q, int pos, long long*& val);
	 int db_bind_param(dbi_prepare_t* q, int pos, float*& val);
	 int db_bind_param(dbi_prepare_t* q, int pos, double*& val);
	 int db_bind_param(dbi_prepare_t* q, int pos, DBI_DATE*& val);

	 int db_set_null_flag(dbi_prepare_t* q, int  pos, dbi_flag_t iflag);
	 int db_get_null_flag(dbi_prepare_t* q, int  pos, dbi_flag_t *flag);

	 int db_get_incolumns(dbi_prepare_t* q);
	 int db_get_outcolumns(dbi_prepare_t* q);
	 int db_get_affected_rows(dbi_prepare_t* q);

    /*打印错误信息*/
    void db_get_error(void);

private:
	/* 清空错误信息缓冲 */
	void clearerr(void);
	/* 清空 MYSQL_TIME map */
	//void clearmap(void);

private:
	MYSQL*		_conn;
	MYSQL_STMT* _stmt;

	MYSQL_RES*	_res;
	MYSQL_ROW	_row;
	char err_msg[1024];
};

#endif	// __DBIMYSQL_H__
