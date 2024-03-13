/*
 * @Author: XuanseXS 
 * @Date: 2024-02-05 09:35:54
 * @LastEditors: XuanseXS 
 * @LastEditTime: 2024-02-05 13:44:42
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */

/* Files includes  -----------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "mysqlite.h"
#include "sqlite_semaphore.h"

/* Defines -------------------------------------------------------------------*/

/* Static Variable  ----------------------------------------------------------*/
static int sem_id = 0;

/* Variable  -----------------------------------------------------------------*/
union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
};

/* Static Fanction  ----------------------------------------------------------*/

/* Fanction  -----------------------------------------------------------------*/
sqlite3 *mysqlite_db_open(char *path)
{
	int ret;
	sqlite3 *db;
	union semun sem_union;

	sem_id = semget((key_t)SEM_KEY, 4, 0666 | IPC_CREAT | IPC_EXCL);
	if (sem_id == -1)
	{
		printf("semaphore get failed!\n");
	}
	else
	{
		printf("Semaphore:Set value 1 sem_id:%d\n", sem_id);
		sem_union.val = 1;
		semctl(sem_id, 0, SETVAL, sem_union);
	}

	ret = sqlite3_open(path, &db);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_open : %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	else
	{
		return db;
	}
}

int mysqlite_db_close(sqlite3 *db)
{
	sem_id = 0;
	sqlite3_close(db);
	return 0;
}

int mysqlite_create_table(sqlite3 *db, char *table, char *table_entry)
{
	char *sql_template = "CREATE TABEL IF NOT EXISTS %s (%s);";
	char *sql = NULL;
	int ret = 0;

	sql = calloc(1, strlen(sql_template) + strlen(table) + strlen(table_entry) + 1); // 为sql动态分配内存空间
	ret = sprintf(sql, sql_template, table, table_entry);
	if (ret == -1)
	{
		printf("%s sprintf error!\n", __func__);
		goto err;
	}

	SQL_semP(SEM_KEY, 0);
	ret = sqlite3_exec(db, sql, 0, 0, NULL);
	SQL_semV(SEM_KEY, 0);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_exec_create_tabel : %s\n", sqlite3_errmsg(db));
		goto err;
	}

	free(sql);
	return 0;
err:
	free(sql);
	return -1;
}

int mysqlite_insert(sqlite3 *db, char *table, char *val)
{
	char *sql_template = "INSERT INTO %s VALUES(%s);";
	char *sql = NULL;
	int ret = 0;

	sql = calloc(1, strlen(sql_template) + strlen(table) + strlen(val) + 1); // 为sql动态分配内存空间
	ret = sprintf(sql, sql_template, table, val);
	if (ret == -1)
	{
		printf("%s sprintf error!\n", __func__);
		goto err;
	}

	SQL_semP(SEM_KEY, 0);
	ret = sqlite3_exec(db, sql, 0, 0, NULL);
	SQL_semV(SEM_KEY, 0);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_exec_insert : %s\n", sqlite3_errmsg(db));
		goto err;
	}

	free(sql);
	return 0;
err:
	free(sql);
	return -1;
}

int mysqlite_delete(sqlite3 *db, char *table, char *content)
{
	char *sql_template = "DELETE FROM %s %s;";
	char *sql = NULL;
	int ret = 0;

	sql = calloc(1, strlen(sql_template) + strlen(table) + strlen(content) + 1); // 为sql动态分配内存空间
	ret = sprintf(sql, sql_template, table, content);
	if (ret == -1)
	{
		printf("%s sprintf error!\n", __func__);
		goto err;
	}

	SQL_semP(SEM_KEY, 0);
	ret = sqlite3_exec(db, sql, 0, 0, NULL);
	SQL_semV(SEM_KEY, 0);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_exec_delete : %s\n", sqlite3_errmsg(db));
		goto err;
	}

	free(sql);
	return 0;
err:
	free(sql);
	return -1;
}

int mysqlite_delete_table(sqlite3 *db, char *table)
{
	char *sql_template = "DROP TABLE %s %s;";
	char *sql = NULL;
	int ret = 0;

	sql = calloc(1, strlen(sql_template) + strlen(table) + 1); // 为sql动态分配内存空间
	ret = sprintf(sql, sql_template, table);
	if (ret == -1)
	{
		printf("%s sprintf error!\n", __func__);
		goto err;
	}

	SQL_semP(SEM_KEY, 0);
	ret = sqlite3_exec(db, sql, 0, 0, NULL);
	SQL_semV(SEM_KEY, 0);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_exec_delete_table : %s\n", sqlite3_errmsg(db));
		goto err;
	}

	free(sql);
	return 0;
err:
	free(sql);
	return -1;
}

int mysqlite_updata(sqlite3 *db, char *table, char *content, int sem_lock)
{
	char *sql_template = "UPDATE %s SET %s;";
	char *sql = NULL;
	int ret = 0;

	sql = calloc(1, strlen(sql_template) + strlen(table) + strlen(content) + 1); // 为sql动态分配内存空间
	ret = sprintf(sql, sql_template, table, content);
	if (ret == -1)
	{
		printf("%s sprintf error!\n", __func__);
		goto err;
	}

	if (sem_lock)
		SQL_semP(SEM_KEY, 0);
	ret = sqlite3_exec(db, sql, 0, 0, NULL);
	if (sem_lock)
		SQL_semV(SEM_KEY, 0);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_exec_updata : %s\n", sqlite3_errmsg(db));
		goto err;
	}

	free(sql);
	return 0;
err:
	free(sql);
	return -1;
}

int mysqlite_query(sqlite3 *db, char *table, char *content, char *other, query_callback_t cb, void *user_data)
{
	char *sql_template = "SELECT %s FROM %s %s;";
	char *sql = NULL;
	int ret = 0;

	sql = calloc(1, strlen(sql_template) + strlen(table) + strlen(content) + strlen(other) + 1); // 为sql动态分配内存空间
	ret = sprintf(sql, sql_template, content, table, other);
	if (ret == -1)
	{
		printf("%s sprintf error!\n", __func__);
		goto err;
	}

	SQL_semP(SEM_KEY, 0);
	ret = sqlite3_exec(db, sql, cb, user_data, NULL);
	SQL_semV(SEM_KEY, 0);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_exec_select : %s, ret = %d\n", sqlite3_errmsg(db), ret);
		goto err;
	}

	free(sql);
	return 0;
err:
	free(sql);
	return -1;
}
