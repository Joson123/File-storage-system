#include "usrname.h"


/*
	登入账户
	@ppDd:连接数据库的对象
	@usr_num:登入成功后:返回的账号
	返回值:
	成功返回 1
	失败返回 0
*/
int log_in(sqlite3 *ppDd,unsigned long *usr_num)
{
	sqlite3_stmt *stmt = NULL;  		//后面要销毁这个字节码对象
	int a;
	unsigned long usr = 0;
	char password[100] = {0};
	char buf[100] = {0};	
	printf("请输入账号:"); 
	scanf("%ld",&usr);
	getchar();
	printf("请输入密码:");
	scanf("%s",password);
	fgets(buf,100,stdin);
	char *sql = "select usrname,password from usr;";
	a = sqlite3_prepare_v2(ppDd,sql,strlen(sql),&stmt,NULL);
	if(a != SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_prepare failed:%s\n",sqlite3_errmsg(ppDd));
	}
	while(sqlite3_step(stmt) == SQLITE_ROW)
	{
		if(sqlite3_column_int(stmt,0) == usr && strcmp(sqlite3_column_text(stmt,1),password) == 0)
		{
			*usr_num = usr;
			sqlite3_finalize(stmt);
			return 1;
		}
	}
	sqlite3_finalize(stmt);
	return 0;
}


/*
	注册一个账号
	@ppDd:数据库连接对象
*/

void registered(sqlite3 *ppDd)
{
	while(1)
	{
		sqlite3_stmt *stmt = NULL;
		int a;
		long usr;
		char password[100] = {0};
		printf("请输入账号:");
		scanf("%ld",&usr);
		printf("请输入密码:");
		scanf("%s",password);
		char *sql = "insert into usr(usrname,password) values(?,?);";
		a = sqlite3_prepare_v2(ppDd,sql,strlen(sql),&stmt,NULL);
		if(a != SQLITE_OK)
		{
			fprintf(stderr,"sqlite3_open failed:%s\n",sqlite3_errmsg(ppDd));
		}
		a = sqlite3_bind_int(stmt,1,usr);
		if(a != SQLITE_OK)
		{
			fprintf(stderr,"sqlite3_open failed:%s\n",sqlite3_errmsg(ppDd));
		}
		a = sqlite3_bind_text(stmt,2,password,strlen(password),NULL);
		if(a != SQLITE_OK)
		{
			fprintf(stderr,"sqlite3_open failed:%s\n",sqlite3_errmsg(ppDd));
		}
		a = sqlite3_step(stmt);
		if(a == SQLITE_DONE)
		{
			printf("注册成功!\n");
			sqlite3_finalize(stmt);
			return;
		}
		printf("该账号已经存在，请重新输入\n");
	}
}


/*
	在账户中查看查看一个文件
	@ppDd:数据库连接对象
	@usr_name:账号用户名	
*/

void select_file(sqlite3 *ppDd,unsigned long usr_name)
{
	sqlite3_stmt *stmt = NULL;
	int file_flag = 0;
	char sql[100] = {0};
	sprintf(sql,"select filename from file where file.usrname=%ld;",usr_name);
	int a = sqlite3_prepare_v2(ppDd,sql,strlen(sql),&stmt,NULL);
	if(a != SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_prepare failed:%s\n",sqlite3_errmsg(ppDd));
	}
	
	while(sqlite3_step(stmt) == SQLITE_ROW)
	{	
		file_flag = 1;
		printf("%s\n",sqlite3_column_text(stmt,0));
	}
	if(file_flag == 0)
	{
		printf("该账户没有保存任何文件!\n");
	}
	sqlite3_finalize(stmt);
}

/*
	保存一个文件
	@ppDd:数据库连接对象
	@filename:保存文件的路径名
	@usrname:账号名
	返回值
	成功返回       1
	失败返回   	   0
*/

int save_file(sqlite3 *ppDd,char *filename,unsigned long usrname)
{
	int fd = open(filename,O_RDWR);
	if(fd == -1)
	{
		perror("open failed\n");
		return 0;
	}
	sqlite3_stmt *stmt = NULL;
	struct stat st;
	int b = stat(filename,&st);
	if(b == -1)
	{
		perror("stat is error\n");
	}
	if(S_ISREG(st.st_mode))
	{
		char file[512] = {0};      //保存文件的语句字符串
		char buf[1000] = {0};
		int ret = read(fd,buf,100);
		if(ret == 0)
		{
			printf("该文件没有任何内容!\n");
			return 0;
		}
		sprintf(file,"insert into file(usrname,filename,contain) values(?,?,?);");
		int a = sqlite3_prepare_v2(ppDd,file,strlen(file),&stmt,NULL);
		if(a != SQLITE_OK)
		{	
			fprintf(stderr,"sqlite3 is prepare2 error:%s\n",sqlite3_errmsg(ppDd));
		}
		sqlite3_bind_int(stmt,1,usrname);
		sqlite3_bind_text(stmt,2,filename,strlen(filename),NULL);
		sqlite3_bind_text(stmt,3,buf,strlen(buf),NULL);
		sqlite3_step(stmt);
	}
	else
	{
		printf("不是符合的文件\n");
		return 0;
	}
	unlink(filename);
	sqlite3_finalize(stmt);
	return 1;
}


/*
	获取一个文件
	@ppDd:数据库连接对象
	@filename:要获取的那个文件
	@usrname:用户名
*/

void get_file(sqlite3 *ppDd,char *filename,unsigned long usrname)
{
	int a;
	int file_flag = 0;
	int fd;
	char select_file[512] = {0};
	sqlite3_stmt *stmt = NULL;
	sprintf(select_file,"select filename from file where file.usrname=%ld;",usrname);
	a = sqlite3_prepare_v2(ppDd,select_file,strlen(select_file),&stmt,NULL);
	if(a != SQLITE_OK)
	{
		fprintf(stderr,"sqlite3 is prepare error:%s\n",sqlite3_errmsg(ppDd));
		return;
	}
	while(sqlite3_step(stmt) == SQLITE_ROW)
	{
		if(strcmp(filename,sqlite3_column_text(stmt,0)) == 0)
		{
			file_flag = 1;
			memset(select_file,0,512);
			sprintf(select_file,"select contain from file where file.filename='%s';",filename);
			a = sqlite3_prepare_v2(ppDd,select_file,strlen(select_file),&stmt,NULL);
			if(a != SQLITE_OK)
			{
				fprintf(stderr,"sqlite3 is prepare error:%s\n",sqlite3_errmsg(ppDd));
			}
			while(sqlite3_step(stmt) == SQLITE_ROW)
			{
				int ret;
				fd = open(filename,O_RDWR | O_CREAT | O_EXCL,0660);
				if(fd == -1)
				{
					printf("该文件已经存在，无法正常提取\n");
					return;
				}
				char contain[1000] = {0};
				strcpy(contain,sqlite3_column_text(stmt,0));
				ret = write(fd,contain,strlen(contain));
				if(ret == -1)
				{
					perror("write failed\n");
				}
				if(ret == 0)
				{
					close(fd);
					sqlite3_finalize(stmt);
					printf("文件写入失败\n");
					return;
				}
			}
		}
	}
	if(file_flag == 0)
	{
		printf("该账号不存在这个文件!\n");
		sqlite3_finalize(stmt);
		return;
	}
	printf("提取成功!\n");
	close(fd);
	sqlite3_finalize(stmt);
}

/*
	删除一个账号下的文件
	@ppDd:数据库连接对象
	@filename:要删除的文件名
	@usrname:你登入的用户名
*/

void delete_file(sqlite3 *ppDd,char *filename,unsigned long usrname)
{
	int a;
	int file_flag = 0;
	char sql[100] = {0};
	sqlite3_stmt *stmt = NULL;
	sprintf(sql,"select filename from file where file.usrname=%ld;",usrname);
	a = sqlite3_prepare_v2(ppDd,sql,strlen(sql),&stmt,NULL);
	if(a != SQLITE_OK)
	{
		fprintf(stderr,"sqlite3 is prepare error:%s\n",sqlite3_errmsg(ppDd));
		return;
	}
	while(sqlite3_step(stmt) == SQLITE_ROW)
	{
		if(strcmp(filename,sqlite3_column_text(stmt,0)) == 0)
		{
			file_flag = 1;
			char sqlbuf[100] = {0};
			sprintf(sqlbuf,"delete from file where filename='%s';",filename);
			a = sqlite3_prepare_v2(ppDd,sqlbuf,strlen(sqlbuf),&stmt,NULL);
			if(a != SQLITE_OK)
			{
				fprintf(stderr,"sqlite3 is prepare error:%s\n",sqlite3_errmsg(ppDd));
				return;
			}
			sqlite3_step(stmt);
		}
	}
	if(file_flag == 0)
	{
		printf("找不到这个文件无法删除\n");
		return;
	}
	printf("删除成功!\n");
}

