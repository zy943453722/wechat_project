
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dbi_mysql.h"

/**********************************************************/

DBI_mysql::DBI_mysql(void) : _conn(NULL), _stmt(NULL)
{
	mysql_library_init(0, NULL, NULL);
	if (NULL == _conn) {
		_conn = mysql_init(_conn);
        mysql_set_character_set(_conn,"utf8");
	}
}//初始化mysql的API库

DBI_mysql::~DBI_mysql(void)
{
	mysql_library_end();
}

int DBI_mysql::db_connect(const char* host, const char* database, const char* usr, const char* pwd, int port)
{
	int tmp = 3306;
	clearerr();
	int flag = CLIENT_FOUND_ROWS;
	if (0 != port) {
		tmp = port;
	}

	// add by mlz 2014/2/27 for long connection
	char value = 1;
	mysql_options(_conn, MYSQL_OPT_RECONNECT, &value);//重连

	if (!mysql_real_connect(_conn, host, usr, pwd, database, tmp, NULL, flag)) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_error(_conn));
		return DBI_ERROR;
	}

	return DBI_SUCCESS;
}//连接数据库

void DBI_mysql::db_disconnect(void)//断开
{
	clearerr();
	if (NULL != _conn) {
		mysql_close(_conn);
		_conn = NULL;
	}
}

bool DBI_mysql::db_autocommit(bool flag)//批控制自动提交（可设置是否能自动提交）
{
	clearerr();
	if (0 != mysql_autocommit(_conn, flag)) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_error(_conn));
		return false;
	}
	return true;
}

int DBI_mysql::db_begin(void)//设置是否自动提交
{
	clearerr();
	if (!db_autocommit(false)) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_error(_conn));
		return DBI_ERROR;
	}
	return DBI_SUCCESS;
}

int DBI_mysql::db_rollback(void)//回滚
{
	clearerr();
	if (0 != mysql_rollback(_conn)) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_error(_conn));
		return DBI_ERROR;
	}
	return DBI_SUCCESS;
}

int DBI_mysql::db_commit(void)//提交
{
	clearerr();
	if (0 != mysql_commit(_conn)) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_error(_conn));
		return DBI_ERROR;
	}
/*	if (!db_autocommit(true)) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_error(_conn));
		return DBI_ERROR;
	}*/
	return DBI_SUCCESS;
}

int DBI_mysql::db_lock_wait(int secs)//给表上锁
{
	return DBI_SUCCESS;
}

/**********************************************************/

int DBI_mysql::db_execute(const char* sql)//执行sql语句
{
	clearerr();
	int ret = mysql_real_query(_conn, sql, strlen(sql));
	if (0 != ret) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_error(_conn));
		return DBI_ERROR;
	}
	return DBI_SUCCESS;
}

int DBI_mysql::db_fetch(void)//获取结果集
{
	clearerr();
	_res = mysql_store_result(_conn);
	if (!_res) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_error(_conn));
		return DBI_ERROR;
	}
	return DBI_SUCCESS;
}

bool DBI_mysql::db_next(void)//获取一行行取记录集
{
	clearerr();
	_row = mysql_fetch_row(_res);
	if (NULL == _row) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_error(_conn));
		return false;
	}
	return true;
}

void DBI_mysql::db_close_cursor(void)
{
	clearerr();
	mysql_free_result(_res);
}

int DBI_mysql::db_get_field(int pos, char* val, int size)//获取字段
{
	clearerr();
	if (NULL == _row[pos]) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Row NULL");
		return DBI_FLAG_NULL;
	}
	strcpy(val, _row[pos]);
	return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(int pos, int& val)
{
	clearerr();
	if (NULL == _row[pos]) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Row NULL");
		return DBI_FLAG_NULL;
	}
	val = atoi(_row[pos]);
	return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(int pos, long long& val)
{
	clearerr();
	if (NULL == _row[pos]) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Row NULL");
		return DBI_FLAG_NULL;
	}
	val = atol(_row[pos]);
	return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(int pos, float& val)
{
	clearerr();
	if (NULL == _row[pos]) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Row NULL");
		return DBI_FLAG_NULL;
	}
	val = (float)atof(_row[pos]);
	return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(int pos, double& val)
{
	clearerr();
	if (NULL == _row[pos]) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Row NULL");
		return DBI_FLAG_NULL;
	}
	val = atol(_row[pos]);
	return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(int pos, DBI_DATE& val)
{
	clearerr();
	return DBI_SUCCESS;
}

/**********************************************************/

int DBI_mysql::db_prepare_sql(dbi_prepare_t* q, const char* sql)
{
	clearerr();
    //printf("file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);

	if (NULL == q->p_query) {
		q->p_query = (mysql_preparement *)malloc(sizeof(mysql_preparement));
		memset(q->p_query, 0, sizeof(mysql_preparement));
	}
    mysql_preparement* t = (mysql_preparement *)q->p_query;
    t->stmt = mysql_stmt_init(_conn);
    if (NULL == t->stmt){
    	snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
        return DBI_ERROR;
    }

    if (0 != mysql_stmt_prepare(t->stmt, sql, strlen(sql))){
        snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
        return DBI_ERROR;
    }

	t->param_count = mysql_stmt_param_count(t->stmt);
	//printf("param_count %d\n", t->param_count);
	t->field_count = mysql_stmt_field_count(t->stmt);
	//printf("field_count %d\n", t->field_count);

	/* 入参 */
	if (t->param_count > 0) {
		t->bind_input = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * t->param_count);
		memset(t->bind_input, 0, sizeof(MYSQL_BIND) * t->param_count);

		for (int i=0; i<t->param_count; i++) {
			t->bind_input[i].buffer = (char *)malloc(1);
			t->bind_input[i].buffer_length = 1;
			t->bind_input[i].is_null = (my_bool *)malloc(sizeof(my_bool));
			t->bind_input[i].length = (unsigned long *)malloc(sizeof(unsigned long));
			*(t->bind_input[i].length) = 0;
			t->bind_input[i].error = (my_bool *)malloc(sizeof(my_bool));
		}

	}

	/* 出参 */
	if (t->field_count > 0) {
		t->bind_output = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * t->field_count);
		memset(t->bind_output, 0, sizeof(MYSQL_BIND) * t->field_count);
		if (0 != mysql_stmt_bind_result(t->stmt, t->bind_output)) {
            snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
            return DBI_ERROR;
        }
    }

    return DBI_SUCCESS;
}

int DBI_mysql::db_free_sql(dbi_prepare_t* q)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}

	//clearmap();

	if (NULL != t->bind_input) {
		for (int i=0; i<t->param_count; i++) {
			free(t->bind_input[i].error);
			free(t->bind_input[i].length);
			free(t->bind_input[i].is_null);
			free(t->bind_input[i].buffer);
		}
		free(t->bind_input);
	}
	if (NULL != t->bind_output) {
		free(t->bind_output);
	}

	if (0 != mysql_stmt_close(t->stmt)){
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
		return DBI_ERROR;
	}

	free(t);

    return DBI_SUCCESS;
}

int DBI_mysql::db_execute(dbi_prepare_t* q)
{
	int ret = DBI_ERROR;
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return ret;
	}

	if (t->param_count > 0) {
		/* Bind the buffers */
		if (0 != mysql_stmt_bind_param(t->stmt, t->bind_input)) {
			snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
			return ret;
		}
	}

	if (0 == mysql_stmt_execute(t->stmt)) {
		const char* p = mysql_info(_conn);
		if (NULL != p) {
			//printf("%s\n", p);
		}
		unsigned int count = mysql_warning_count(_conn);
		if( count>0 )
			printf("Warning count[%d]\n", count);

		if (t->field_count > 0) {
			/* Now buffer all results to client (optional step) */
			if (0 != mysql_stmt_store_result(t->stmt)) {
				snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
				return DBI_ERROR;
			}
		}
		return DBI_SUCCESS;
	} else {
		unsigned int err = mysql_stmt_errno(t->stmt);
		printf("MYSQL ERRCODE[%d]\n", err);
		switch (err) {
		case 1062:
			ret = DBI_PRIMARYKEY_EXIST;
		break;
		default:
		break;
		}
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
	}
	return ret;
}

int DBI_mysql::db_fetch(dbi_prepare_t* q)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}

    int ret = mysql_stmt_fetch(t->stmt);
    if( ret == 0 || ret == MYSQL_DATA_TRUNCATED)
    {
        return DBI_SUCCESS;
    }
    if( ret == 1 )
    {
        snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
        return DBI_ERROR;
    }
    if( ret == MYSQL_NO_DATA )
    {
        return DBI_FETCH_EOF;
    }

    return DBI_SUCCESS;
}

int DBI_mysql::db_close_cursor(dbi_prepare_t* q)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}

	//mysql_free_result(t->result_metadata);

	if (0 != mysql_stmt_free_result(t->stmt)){
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
		return DBI_ERROR;
	}

    return DBI_SUCCESS;
}

/**********************************************************/
/* 设置字符串字段 */
int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, const char* val, bool is_null)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = strlen(val);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_STRING;//不需要区分BLOB,mysql会自动转到BLOB插入
	*(t->bind_input[pos].length) = size;

	strcpy((char *)t->bind_input[pos].buffer, val);
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}
int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, const char* val, int size, bool is_null)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
    if (t->param_count == 0) {
    	snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_STRING;//不需要区分BLOB,mysql会自动转到BLOB插入
	*(t->bind_input[pos].length) = size;

	strncpy((char *)t->bind_input[pos].buffer, val, size);
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}
/* 内置类型 */
int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, short val, bool is_null)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
    if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(short);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_SHORT;
	t->bind_input[pos].is_unsigned = false;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}
int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, int val, bool is_null)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
    if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(int);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_LONG;
	t->bind_input[pos].is_unsigned = false;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}
int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, long val, bool is_null)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(long);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_LONGLONG;
	t->bind_input[pos].is_unsigned = false;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

	return DBI_SUCCESS;
}
int DBI_mysql::db_set_param(dbi_prepare_t *q, int pos, long long val, bool is_null)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(long long);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_LONGLONG;
	t->bind_input[pos].is_unsigned = false;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}
/* 无符号基本类型 */
int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, unsigned short val, bool is_null)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(unsigned short);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_SHORT;
	t->bind_input[pos].is_unsigned = true;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}
int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, unsigned int val, bool is_null)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(unsigned int);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_LONG;
	t->bind_input[pos].is_unsigned = true;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}
int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, unsigned long val, bool is_null)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(unsigned long);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_LONGLONG;
	t->bind_input[pos].is_unsigned = true;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

	return DBI_SUCCESS;
}
int DBI_mysql::db_set_param(dbi_prepare_t *q, int pos, unsigned long long val, bool is_null)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(unsigned long long);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_LONGLONG;
	t->bind_input[pos].is_unsigned = true;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}
/**/
int DBI_mysql::db_set_param(dbi_prepare_t *q, int pos, float val, bool is_null)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(float);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_FLOAT;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}
int DBI_mysql::db_set_param(dbi_prepare_t *q, int pos, double val, bool is_null)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(double);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_DOUBLE;
	//*(t->bind_input[pos].length) = 0;

	memcpy(t->bind_input[pos].buffer, &val, sizeof(val));
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}

int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, const DBI_DATE& val, bool is_null)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	int size = sizeof(MYSQL_TIME);
	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}
	
	t->bind_input[pos].buffer_type = MYSQL_TYPE_TIMESTAMP;
	//*(t->bind_input[pos].length) = 0;

	MYSQL_TIME ts;
	ts.year = val.year;
	ts.month = val.month;
	ts.day = val.day;
	ts.hour = val.hour;
	ts.minute = val.minute;
	ts.second = val.second;
	ts.second_part = 0;

	memcpy(t->bind_input[pos].buffer, &ts, sizeof(ts));
	*(t->bind_input[pos].is_null) = is_null;

	return DBI_SUCCESS;
}

int DBI_mysql::db_set_param(dbi_prepare_t* q, int pos, void* val, int size, bool is_null)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->param_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	if (t->bind_input[pos].buffer_length < size) {
		t->bind_input[pos].buffer = (char *)realloc(t->bind_input[pos].buffer, size);
		t->bind_input[pos].buffer_length = size;
	}

	t->bind_input[pos].buffer_type = MYSQL_TYPE_BLOB;
	*(t->bind_input[pos].length) = size;

	memcpy(t->bind_input[pos].buffer, val, size);
	*(t->bind_input[pos].is_null) = is_null;

    return DBI_SUCCESS;
}

/**********************************************************/

int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, char* val, int size, dbi_flag_t* flag)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_STRING;
	t->bind_output[pos].buffer = val;
	t->bind_output[pos].buffer_length = size;

	t->bind_output[pos].is_unsigned = false;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}

int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, short& val, dbi_flag_t* flag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

    unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_SHORT;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(short);

	t->bind_output[pos].is_unsigned = false;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, int& val, dbi_flag_t* flag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_LONG;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(int);

	t->bind_output[pos].is_unsigned = false;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, long& val, dbi_flag_t* flag)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

    unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_LONGLONG;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(long);

	t->bind_output[pos].is_unsigned = false;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

	return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, long long& val, dbi_flag_t *flag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_LONGLONG;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(long long);

	t->bind_output[pos].is_unsigned = false;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}

int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, unsigned short& val, dbi_flag_t* flag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_SHORT;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(unsigned short);

	t->bind_output[pos].is_unsigned = true;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, unsigned int& val, dbi_flag_t* flag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

		unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_LONG;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(unsigned int);

	t->bind_output[pos].is_unsigned = true;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, unsigned long& val, dbi_flag_t* flag)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_LONGLONG;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(unsigned long);

	t->bind_output[pos].is_unsigned = true;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

	return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, unsigned long long& val, dbi_flag_t *flag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_LONGLONG;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(unsigned long long);

	t->bind_output[pos].is_unsigned = true;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}

int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, float& val, dbi_flag_t *flag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_FLOAT;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(float);

	t->bind_output[pos].is_unsigned = false;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}
int DBI_mysql::db_get_field(dbi_prepare_t *q, int pos, double& val, dbi_flag_t *flag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_DOUBLE;
	t->bind_output[pos].buffer = &val;
	t->bind_output[pos].buffer_length = sizeof(double);

	t->bind_output[pos].is_unsigned = false;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}

int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, DBI_DATE& val, dbi_flag_t* flag)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	MYSQL_TIME ts;
	unsigned long length = 0;
	my_bool b_err = false;
	my_bool b_null = false;

	t->bind_output[pos].buffer_type = MYSQL_TYPE_TIMESTAMP;
	t->bind_output[pos].buffer = &ts;
	t->bind_output[pos].buffer_length = sizeof(MYSQL_TIME);

	//t->bind_output[pos].is_unsigned = false;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &length;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	val.year = ts.year;
	val.month = ts.month;
	val.day = ts.day;
	val.hour = ts.hour;
	val.minute = ts.minute;
	val.second = ts.second;

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

	return DBI_SUCCESS;
}

int DBI_mysql::db_get_field(dbi_prepare_t* q, int pos, void* buf, unsigned long bufsize, unsigned long& readed, dbi_flag_t* flag)
{
	clearerr();
	mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
	if (t->field_count == 0) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "This sql has no input param");
		return DBI_ERROR;
	}

	my_bool b_err = false;
	my_bool b_null = false;
	t->bind_output[pos].buffer_type = MYSQL_TYPE_BLOB;
	t->bind_output[pos].buffer = buf;
	t->bind_output[pos].buffer_length = bufsize;

	//t->bind_output[pos].is_unsigned = false;
	t->bind_output[pos].is_null = &b_null;
	t->bind_output[pos].length = &readed;
	t->bind_output[pos].error = &b_err;

	mysql_stmt_fetch_column(t->stmt, &t->bind_output[pos], pos, 0);

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (b_err) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (b_null) {
			*flag = DBI_FLAG_NULL;
			return DBI_SUCCESS;
		}
	}

    return DBI_SUCCESS;
}

/**********************************************************/

int DBI_mysql::db_bind_param(dbi_prepare_t* q, int pos, char*& val)
{
	return DBI_SUCCESS;
}
int DBI_mysql::db_bind_param(dbi_prepare_t* q, int pos, int*& val)
{
	return DBI_SUCCESS;
}
int DBI_mysql::db_bind_param(dbi_prepare_t* q, int pos, long int*& val)
{
	return DBI_SUCCESS;
}
int DBI_mysql::db_bind_param(dbi_prepare_t* q, int pos, long long*& val)
{
	return DBI_SUCCESS;
}
int DBI_mysql::db_bind_param(dbi_prepare_t* q, int pos, float*& val)
{
	return DBI_SUCCESS;
}
int DBI_mysql::db_bind_param(dbi_prepare_t* q, int pos, double*& val)
{
	return DBI_SUCCESS;
}
int DBI_mysql::db_bind_param(dbi_prepare_t* q, int pos, DBI_DATE*& val)
{
	return DBI_SUCCESS;
}

/**********************************************************/

int DBI_mysql::db_set_null_flag(dbi_prepare_t* q, int pos, dbi_flag_t iflag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}

    if(iflag == DBI_FLAG_NULL)
        *(t->bind_output[pos].is_null) = true;
    else
        *(t->bind_output[pos].is_null) = false;

    return DBI_SUCCESS;
}

int DBI_mysql::db_get_null_flag(dbi_prepare_t* q, int pos, dbi_flag_t *flag)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}

	if (NULL != flag) {
		*flag = DBI_FLAG_NOTNULL;
		if (*(t->bind_output[pos].error) != 0) {
			*flag = DBI_FLAG_TRUNCATE;
		}
		if (*(t->bind_output[pos].is_null) != 0) {
			*flag = DBI_FLAG_NULL;
		}
	}

    return DBI_SUCCESS;
}

int DBI_mysql::db_get_incolumns(dbi_prepare_t* q)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
    return t->param_count;
}

int DBI_mysql::db_get_outcolumns(dbi_prepare_t* q)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}
    return t->field_count;
}

int DBI_mysql::db_get_affected_rows(dbi_prepare_t* q)
{
	clearerr();
    mysql_preparement* t = (mysql_preparement *)q->p_query;
	if (NULL == t) {
		snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, "Param NULL");
		return DBI_ERROR;
	}

    int ret = mysql_stmt_affected_rows(t->stmt);
    if( ret < 0 )
    {
        snprintf(err_msg, sizeof(err_msg)-1, "[%s,%s,%d]%s", __FILE__, __func__, __LINE__, mysql_stmt_error(t->stmt));
        return DBI_ERROR;
    }

    return ret;
}

/**********************************************************/

inline void DBI_mysql::clearerr(void)
{
	err_msg[0] = '\0';
}

void DBI_mysql::db_get_error()
{
    printf("error:%s\n",err_msg);
}

