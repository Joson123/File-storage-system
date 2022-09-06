#include "usrname.h"

int main(int argc, char *argv [])
{
	sqlite3 *ppDd = NULL;
	int a = sqlite3_open("./1.db",&ppDd);
	if(a != SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_open failed:%s\n",sqlite3_errmsg(ppDd));
	}
	int switch_num;
	while(1)
	{
		printf("**************文件存储系统**************\n");
		printf("1、登入\n");
		printf("2、注册\n");
		printf("3、退出系统\n");
		printf("****************************************\n");
		int b;
		scanf("%d",&switch_num);
		if(switch_num == 1)
		{
			unsigned long *usr_num = malloc(sizeof(long));
			b = log_in(ppDd,usr_num);
			if(b == 1)
			{
				printf("登入成功\n");
				while(1)
				{ 
					int file_num;
					int file_flag = 0;
					printf("1、查询文件\n");
					printf("2、存储文件\n");
					printf("3、提取文件\n");
					printf("4、删除文件\n");
					printf("5、退出登入\n");
					scanf("%d",&file_num);
					if(file_num == 1)
					{
						select_file(ppDd,*usr_num);
					}
					else if(file_num == 2)
					{
						printf("输入你要存储的文件名\n");
						char *filename = malloc(sizeof(char));
						scanf("%s",filename);
						b = save_file(ppDd,filename,*usr_num);
						if(b == 1)
						{
							printf("保存成功!\n");
						}
						else
						{
							printf("保存失败，可能不存在这个文件\n");
						}
						free(filename);
					}
					else if(file_num == 3)
					{
						printf("输入你要提取的文件名\n");
						char *filename = malloc(sizeof(char));
						scanf("%s",filename);
						get_file(ppDd,filename,*usr_num);
						free(filename);
					}
					else if(file_num == 4)
					{
						printf("输入你要删除的文件名\n");
						char *filename = malloc(sizeof(char));
						scanf("%s",filename);
						delete_file(ppDd,filename,*usr_num);
						free(filename);
					}
					else if(file_num == 5)
					{
						printf("退出成功\n");
						break;
					}
					else
					{
						printf("你输入的选项错误,请重新输入\n");
					}
				}
			}
			else
			{
				printf("账号或密码错误\n");
			}
			free(usr_num);
		}
		else if(switch_num == 2)
		{
			registered(ppDd);
		}
		else if(switch_num == 3)
		{
			sqlite3_close(ppDd);
			break;
		}
		else
		{
			printf("你输入的选项错误,请重新输入\n");
		}
	}
	return 0;
}