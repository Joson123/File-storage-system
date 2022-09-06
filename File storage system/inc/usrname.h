#ifndef __USRNAME_H__
#define __USRNAME_H__


#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int log_in(sqlite3 *ppDd,unsigned long *usr_num);   	//用户登入
void registered(sqlite3 *ppDd);							//用户注册
void select_file(sqlite3 *ppDd,unsigned long usr_num);	   //查询文件
int save_file(sqlite3 *ppDd,char *filename,unsigned long usrname);		//保存文件
void get_file(sqlite3 *ppDd,char *filename,unsigned long usrname);		//提取文件
void delete_file(sqlite3 *ppDd,char *filename,unsigned long usrname);		//删除文件



#endif