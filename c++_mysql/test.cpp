#include"dbi_mysql.h"
#include <iostream>
using namespace std;
int main()
{
    DBI_mysql dbi;
    dbi_prepare_t q;
    char val[1024];
    char sql[] = "select * from info";
    int i = 0;
    int columns = 0;
    if(dbi.db_connect("127.0.0.1","Student","root","826451379",0) != DBI_SUCCESS)
    {
        dbi.db_get_error();
        exit(-1);
    }
    if(dbi.db_begin() != DBI_SUCCESS)
    {
        dbi.db_get_error();
        exit(-1);
    }
    if(dbi.db_execute(sql) != DBI_SUCCESS)
    {
        dbi.db_get_error();
        exit(-1);
    }
    if(dbi.db_fetch() != DBI_SUCCESS)
    {
        dbi.db_get_error();
        exit(-1);
    }
    //cout << "a" <<endl;
    if(dbi.db_prepare_sql(&q,sql) != DBI_SUCCESS)
    {
        dbi.db_get_error();
        exit(-1);
    }
    //cout << "b" <<endl;
    if((columns = dbi.db_get_Outcolumns(&q)) == DBI_ERROR)
    {
        dbi.db_get_error();
        exit(-1);
    }
    cout << columns << endl;
    //while(DBI_FETCH_EOF != 1)
    while(dbi.db_next() == true)
    {
        //dbi.db_next();
        for(i = 0; i < columns; i++)
        {
            if(dbi.db_get_field(i,val,0) != DBI_SUCCESS)
            {
                   dbi.db_get_error();
                   exit(-1);
            }
            else
            {
                cout << val << "\t";
            }
        }
        cout << endl;
    }
    dbi.db_close_cursor();
    dbi.db_disconnect();
	return 0;
}

