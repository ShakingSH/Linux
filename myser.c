// myser.c

#include "manage_user.h"
#include "manage_bill.h"

#define PORT 6003  // 端口号
#define MAXSIZE 1024  // 缓冲区大小

int running = 1;

void  stop()
{
	running=0;
}

int main(int argc, char *argv[])
{
	int sockfd, new_fd;  // 定义存放套接口描述符的变量
	struct sockaddr_in server_addr;  // 定义服务器端套接口数据结构server_addr
	struct sockaddr_in client_addr;  // 定义客户端套接口数据结构client_addr
    struct userstruct user;
	struct userstruct ur;
    int server_len, client_len;  // 服务器和客户消息长度
	int nbytes, portnumber;
	int num;
	char name[10];
	char buf[MAXSIZE]; 
    char rebuf[MAXSIZE];
	char pathname[100];
	signal(SIGINT, stop);  // 注册SIGINT信号
	FILE * fpn, *fp;
	fp = fopen("cata.txt", "r+b");
	struct object ob[100];
	for(num=0; num < 100; num++)
	{
		if(fread(&ob[num], sizeof(struct object), 1, fp) < 1)
		{
			break;
		}
	}
	fclose(fp);
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)  // 服务器端开始建立socket描述符
	{
		fprintf(stderr, "Socket error:%s\n\a", strerror(errno));
		exit(1);
	}
	bzero(&server_addr, sizeof(struct sockaddr_in));  // 先将套接口地址数据结构清零
	server_addr.sin_family = AF_INET;  // 设为TCP/IP地址族
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 设置本机地址并从主机字节序转换为网络字节序*/
	server_addr.sin_port = htons(PORT);  // 设置端口号并从主机字节序转换为网络字节序*/
	if(bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)  // 调用bind函数绑定指定的端口号和ip地址到服务器创建的套接口
	{
		fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
		exit(1);
	}
	if(listen(sockfd, 5) == -1)  // 端口绑定成功，监听sockfd描述符，设置同时处理的最大连接请求数为5 */
	{
		fprintf(stderr, "Listen error:%s\n\a", strerror(errno));
		exit(1);
	}
	while(running)
	{
		client_len = sizeof(client_addr);  // 接收客户端连接请求
		new_fd = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t *__restrict) &client_len);
		while(1)
		{
			read(new_fd, rebuf, sizeof(struct userstruct));
			printf("%s\n", rebuf);
			memcpy(&ur, rebuf, sizeof(ur));
			printf("%s,%s\n", ur.name, ur.password);
			struct userstruct user;
			FILE *fp;
			int found = 0;
			if((fp = fopen(ARFILE, "rb")) == NULL)
			{
				return 0;
			}
			while(fread(&user, sizeof(user), 1, fp) == 1)
			{
				if(strcmp(ur.name, user.name) == 0)
				{
					found = 1;
					break;
				}
				memset(&user, 0x00, sizeof(user));
			}
			fclose(fp);
			if(found)
			{
				if(strcmp(ur.password, user.password) == 0)
				{
					if(user.access)
					{
						write(new_fd, "L1", 3);
					}
					else
					{
						write(new_fd, "L2", 3);
					}
					break;
				}
				else
				{
					write(new_fd, "ER", 3);				
				}	
			}
			else
			{
				write(new_fd, "NO", 3);
			}
		}
		while(1)
		{
			memset(rebuf,0x00,sizeof(rebuf));
			read(new_fd,rebuf,sizeof(rebuf));
			printf("收到客户端消息: %s\n",rebuf);  // 输出到终端
			if(rebuf[0] == '1')
			{
				while(1)
				{
					memset(rebuf,0x00,sizeof(rebuf));
					read(new_fd,rebuf,sizeof(rebuf));
					printf("收到客户端消息: %s\n",rebuf);  // 输出到终端
					if(rebuf[0] == '1')
					{
						write(new_fd,"请输入新用户名",MAXSIZE);  // 回复消息
						read(new_fd,rebuf,sizeof(rebuf));
						strcpy(user.name,rebuf);
						printf("收到客户端姓名消息: %s\n",user.name);  // 输出到终端
						if(queryuser(user.name) == 1)
						{
							write(new_fd,"用户已存在!",MAXSIZE);
							continue;
						}
						write(new_fd,"请输入用户密码",MAXSIZE);  // 回复消息
						read(new_fd,rebuf,sizeof(rebuf));
						strcpy(user.password, rebuf);
						printf("收到客户端密码信息: %s\n", user.password);  // 输出到终端
						write(new_fd,"如果该新添用户是管理员请输入y，否则输入n：",MAXSIZE);  // 回复消息         
						read(new_fd,rebuf,sizeof(rebuf));
						printf("收到客户端权限消息: %s\n", rebuf);  // 输出到终端
						if(rebuf[0] == 'y')
						{
							user.access = 1;
						}
						else if(rebuf[0] == 'n')
						{
							user.access = 0;
						}
						if(insertuser(user))
						{
							write(new_fd,"增加新用户成功...\n",MAXSIZE);
						}
						else
						{
							write(new_fd,"添加用户的时候发生了错误...\n",MAXSIZE);
						}
					}
					else if(rebuf[0] == '2')
					{
						write(new_fd,"请输入用户名",MAXSIZE);  // 回复消息
						read(new_fd,rebuf,sizeof(rebuf));
						printf("收到客户端查询用户是否存在消息: %s\n",rebuf);  // 输出到终端
						if(queryuser(rebuf))
						{
							write(new_fd,"该用户存在\n",MAXSIZE);
						}
						else
						{
							write(new_fd,"没有找到该用户的数据...\n",MAXSIZE);
						}
					}
					else if(rebuf[0]== '3')
					{
						memset(&user, 0x00, sizeof(user));
						FILE *fp;
						if((fp = fopen(ARFILE, "rb")) == NULL)
						{
							return 0;
						}
						while(fread(&user, sizeof(user), 1, fp) == 1)
						{
							write(new_fd, &user, sizeof(user));
							read(new_fd, rebuf, sizeof(rebuf));
						}
						fclose(fp);
						write(new_fd, "end", 4);
					}
					else if(rebuf[0] == '4')
					{
						write(new_fd,"请输入要删除的用户姓名:",MAXSIZE);  // 回复消息
						read(new_fd,rebuf,10);
						strcpy(name,rebuf);
						printf("收到客户端删除用户姓名消息: %s\n",name);  // 输出到终端
						int ret = removeuser(name);
						if(ret == 1)
						{
							write(new_fd, "删除用户账号成功...\n",MAXSIZE);
						}
						else if(ret == -1)
						{
							write(new_fd, "不能删除管理员账号...\n",MAXSIZE);
						}
						else
						{
							write(new_fd, "删除用户账号发生了错误...\n",MAXSIZE);                
						}
					}
					else if(rebuf[0] == '5')
					{
						write(new_fd,"请输入用户姓名",MAXSIZE);  // 回复消息
						read(new_fd,rebuf,sizeof(rebuf));
						strcpy(user.name,rebuf);
						printf("收到客户端修改用户名消息: %s\n",rebuf);  // 输出到终端
						write(new_fd, "请输入用户密码", MAXSIZE);  // 回复消息
						read(new_fd, rebuf, sizeof(rebuf));
						strcpy(user.password, rebuf);
						printf("收到客户端修改密码消息: %s\n",rebuf);  // 输出到终端
						int ret = updateuser(user);
						if(ret == 1)
						{
							write(new_fd,"修改用户资料成功...\n",MAXSIZE);
						}
						else if(ret == -1)
						{
							write(new_fd,"没有该用户的数据...\n",MAXSIZE);
						}
						else if(ret == -2)
						{
							write(new_fd,"没有权限修改管理员账号...\n",MAXSIZE);
						}
						else
						{
							write(new_fd,"修改用户账号发生了错误...\n",MAXSIZE);
						}
					}
					else if(rebuf[0] == '0')
					{
						break;
					}
				}
			}
			else if(rebuf[0] == '2')
			{
				while (1)
				{
					memset(rebuf,0x00,sizeof(rebuf));
					read(new_fd, rebuf, sizeof(rebuf));
					printf("收到客户端消息: %s\n",rebuf);  // 输出到终端
					if(rebuf[0] == '1')
					{
						int index = 0;
						for(index; index < num; index++)
						{
							write(new_fd, &ob[index], sizeof(struct object));
							read(new_fd, rebuf, sizeof(rebuf));							
						}
						write(new_fd, "end", 4);
					}
					else if(rebuf[0] == '2')
					{
						write(new_fd, "请输入活动名", MAXSIZE);
						read(new_fd, rebuf, 40);
						printf("收到客户端活动名消息: %s\n",rebuf);  // 输出到终端
						int i = ob_query(ob, num, rebuf);
						if(i == -1)
						{
							write(new_fd, "该活动不存在！", 22);
						}
						else
						{
							strcat(pathname, ob[i].item_name);	
							strcat(pathname, ".txt");
							struct bill b;
							write(new_fd, "加载成功", MAXSIZE);
							while(1)
							{
								fpn = fopen(pathname, "r+b");
								fseek(fp, 0*sizeof(struct bill), SEEK_SET);
								while(fread(&b, sizeof(struct bill), 1, fpn))
								{
									write(new_fd, &b, sizeof(b));
									read(new_fd, rebuf, 3);
								}
								write(new_fd, "end", 4);
								fclose(fpn);
								read(new_fd, rebuf, MAXSIZE);
								printf("收到客户端消息：%s\n", rebuf);
								if(rebuf[0] == '1')
								{
									write(new_fd, "OK", MAXSIZE);
									read(new_fd, rebuf, sizeof(struct bill));
									struct bill *tmp_b = (struct bill*) &rebuf;
									if(bill_query(pathname, tmp_b->ob_name) < 0)
									{
										bill_insert(pathname, tmp_b);
										write(new_fd, "添加成功!", MAXSIZE);
									}
									else
									{
										write(new_fd, "该物品已经存在!", MAXSIZE);
									}
								}
								else if(rebuf[0] == '2')
								{
									write(new_fd,"请输入要删除的物品:",MAXSIZE);
									read(new_fd, rebuf, MAXSIZE);
									int i = bill_query(pathname, rebuf);
									if(i < 0)
									{
										write(new_fd, "该物品不存在!", MAXSIZE);
									}
									else
									{
										bill_remove(pathname, rebuf);
										write(new_fd, "删除物品成功!", MAXSIZE);
									}
								}
								else if(rebuf[0] == '3')
								{
									write(new_fd,"请输入要报销的物品：",MAXSIZE);
									read(new_fd, rebuf, MAXSIZE);
									int i = bill_query(pathname, rebuf);
									if(i < 0)
									{
										write(new_fd, "该物品不存在!", MAXSIZE);
									}
									else
									{
										if(bill_update(pathname, ur.name, i))
										{
											write(new_fd, "报销成功!", MAXSIZE);
										}
										else
										{
											write(new_fd, "报销失败，改物品已被报销!", MAXSIZE);
										}
									}
								}
								else if(rebuf[0] == '0')
								{
									break;
								}
							}
							strcpy(pathname, "./");	
						}
					}
					else if(rebuf[0] == '3')
					{
						write(new_fd, "请输入活动名", MAXSIZE);
						read(new_fd, rebuf, 40);
						printf("收到客户端活动名消息: %s\n",rebuf);  // 输出到终端
						if(ob_query(ob, num, rebuf) == -1)
						{
							strcat(pathname, rebuf);	
							strcat(pathname, ".txt");
							fpn = fopen(pathname, "wb");
							strcpy(pathname, "./");	
							fclose(fpn);
							struct object tmp_ob;
							strcpy(tmp_ob.ad_name, ur.name);
							strcpy(tmp_ob.item_name, rebuf);
							time(&tmp_ob.t);
							ob[num] = tmp_ob;
							num++;
							write(new_fd, "创建成功", MAXSIZE);			
						}
						else
						{
							write(new_fd, "该活动已经存在！", MAXSIZE);
						}
					}
					else if(rebuf[0] == '4')
					{
						write(new_fd, "请输入活动名", MAXSIZE);
						read(new_fd, rebuf, 15);
						printf("收到客户端活动名消息: %s\n",rebuf);  // 输出到终端
						int i = ob_query(ob, num, rebuf);
						printf("%d\n", i);
						if(i == -1)
						{
							write(new_fd, "该活动不存在！", MAXSIZE);
						}
						else
						{
							for(i = i + 1; i < num ; i++)
							{
								ob[i-1] = ob[i];
							}
							num--;
							strcat(pathname, rebuf);	
							strcat(pathname, ".txt");
							remove(pathname);
							strcpy(pathname, "./");							
							write(new_fd, "该活动成功删除！", MAXSIZE);
						}					
					}
					else if(rebuf[0] == '0')
					{
						break;
					}
				}
			}
			else if(rebuf[0] == '0')
			{
				break;
			}
		}
		close(new_fd);
	}
	fp = fopen("cata.txt", "w+b");  // 写入活动信息
	int i = 0;
	for(i; i<num; i++)
	{
		fwrite(&ob[num], sizeof(struct object), 1, fp);
	}
	fclose(fp);
	close(sockfd);
    return 0;
}