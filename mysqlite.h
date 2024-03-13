/*
 * @Author       : XuanseXS 
 * @Date         : 2024-02-05 09:36:28
 * @LastEditors  : xuanse 
 * @LastEditTime : 2024-03-13 22:26:35
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __MYSQLITE_H
#define __MYSQLITE_H

/* Files includes  -----------------------------------------------------------*/
#include "sqlite3.h"

/* Defines -------------------------------------------------------------------*/
#define SEM_KEY 20240205

typedef int (*query_callback_t)(void *user_data, int col_count, char **col_values, char **col_name);

/* Variables -----------------------------------------------------------------*/


/* Functions ----------------------------------------------------------------*/

/**
 * @description  : 打开数据库(database)
 * @param         {char} *path 数据库路径
 * @return        {sqlite3 *}
 */
sqlite3 *mysqlite_db_open(char *path);

/**
 * @description  : 关闭数据库(database)
 * @param         {sqlite3} *db 数据库Handle
 * @return        {int}
 */
int mysqlite_db_close(sqlite3 *db);

/**
 * @description  : 在数据库中创建一个新表
 * @param         {sqlite3} *db         数据库Handle
 * @param         {char} *table         表名称
 * @param         {char} *table_entry   表所包含的项
 * @return        {int}
 */
int mysqlite_create_table(sqlite3 *db, char *table, char *table_entry);

/**
 * @description  : 向数据库的某个表中添加新的数据行
 * @param         {sqlite3} *db         数据库Handle
 * @param         {char} *table         表名称
 * @param         {char} *val           要添加的新数据行
 * @return        int
 */
int mysqlite_insert(sqlite3 *db, char *table, char *val);

/**
 * @description  : 删除数据库中已有的数据
 * @param         {sqlite3} *db         数据库Handle
 * @param         {char} *table         表名称
 * @param         {char} *content       需要删除的数据
 * @return        {*}
 */
int mysqlite_delete(sqlite3 *db, char *table, char *content);

/**
 * @description  : 删除数据库中已有的表
 * @param         {sqlite3} *db         数据库Handle
 * @param         {char} *table         表名称
 * @return        {*}
 */
int mysqlite_delete_table(sqlite3 *db, char *table);

/**
 * @description  : 向数据库修改更新已有数据
 * @param         {sqlite3} *db         数据库Handle
 * @param         {char} *table         表名称
 * @param         {char} *content       需要修改更新的内容
 * @param         {int} sem_lock        信号量锁 1：开启 0：关闭
 * @return        {*}
 */
int mysqlite_updata(sqlite3 *db, char *table, char *content, int sem_lock);

/**
 * @description  : 向数据库中读取已有的数据
 * @param         {sqlite3} *db         数据库Handle
 * @param         {char} *table         表名称
 * @param         {char} *content       需要读取查询的内容
 * @param         {char} *other         其它的sql语句内容
 * @param         {query_callback_t} cb 查询回调函数
 * @param         {void} *user_data     向回调函数传递的参数
 * @return        {*}
 */
int mysqlite_query(sqlite3 *db, char *table, char *content, char *other, query_callback_t cb, void *user_data);

#endif
