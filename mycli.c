// mycli.c

#include "manage_user.h"
#include "manage_bill.h"

#define PORT 6003  // 端口号
#define MAXSIZE 1024  // 缓冲区大小

int main(int argc, char *argv[])
{
    
	int new_fd;
	char buf[MAXSIZE];
    char rebuf[MAXSIZE];
    char c;
	struct sockaddr_in server_addr; // 定义服务器端套接口数据结构server_addr 
    struct hostent *host;  // 定义一个hostent结构的指针
    struct userstruct user, me;
    host = gethostbyname("localhost");
	if((new_fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		fprintf(stderr, "Socket Error:%s\a\n", strerror(errno));
		exit(1); 
	} 
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	if(connect(new_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr)) == -1)
	{
		fprintf(stderr, "Connect Error:%s\a\n", strerror(errno));
		exit(1);
	}
    else 
    {
        fprintf(stdout,"连接成功\n");
    }
    while (1)
    {
        printf("请输入用户账号：");
        scanf("%s", me.name);
        printf("请输入用户密码：");
        scanf("%s", me.password);
        write(new_fd, &me, sizeof(me));
        read(new_fd, rebuf, 3);
        if(strcmp(rebuf, "ER") == 0)
        {
            printf("密码错误！\n");
        }
        else if(strcmp(rebuf, "NO") == 0)
        {
            printf("账号不存在！\n");
        }
        else if(strcmp(rebuf, "L1") == 0)
        {
            me.access = 1;
            break;
        }
        else if(strcmp(rebuf, "L2") == 0)
        {
            me.access = 0;
            break;
        }
    }
    if(me.access)
    {
        printf("管理员%s您好，欢迎您使用报账系统!\n", me.name);
    }
    else
    {
        printf("用户%s您好，欢迎您使用报账系统!\n", me.name);
    }
    while(1)
    {
        printf("     *活动报账管理系统*\n");
        printf("-------------------------\n");
        printf("     1.进入用户管理       \n");
        printf("     2.进入报账系统       \n");
        printf("     0.退出系统           \n");
        printf("-------------------------\n");        
        printf("请输入：");
    	scanf("%s", buf);
    	write(new_fd, buf,sizeof(buf));  // 发送消息    
        if(buf[0] == '1')
        {
            while(1)
            {  
                printf("       *用户管理平台*       \n");
                printf("---------------------------\n");
                printf("     1.添加新用户账号       \n");
                printf("     2.查找用户是否存在     \n");
                printf("     3.查看全部账号         \n");
                printf("     4.删除用户账号         \n");
                printf("     5.修改用户账号         \n");
                printf("     0.返回                \n");
                printf("---------------------------\n");
                printf("请输入：");
                scanf("%s", buf);
                if(buf[0] == '1' && me.access != 1)
                {
                    printf("对不起，您的权限不允许您添加用户\n");
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);  
                    continue;  
                }
                else if(buf[0] == '4' && me.access != 1)
                {
                    printf("对不起，您的权限不允许您删除用户\n");
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);    
                    continue; 
                }
                else if(buf[0] == '5' && me.access != 1)
                {
                    printf("对不起，您的权限不允许您修改用户\n");
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);   
                    continue;  
                }
                write(new_fd, buf, sizeof(buf));  // 发送消息
                if(buf[0] == '0')
                {
                    break;
                }
                else if(buf[0] == '1')
                {
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n请输入用户名：", rebuf);  // 输出到终端
                    scanf("%s", buf);
                    write(new_fd, buf, sizeof(buf));  // 发送消息
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                    if(strcmp(rebuf, "用户已存在!") == 0)
                    {
                        printf("输入任意键继续...\n");
                        scanf("%s", buf);  
                        continue;
                    }
                    printf("请输入用户密码：");  // 输出到终端
                    scanf("%s", buf);
                    write(new_fd, buf, sizeof(buf));  // 发送消息
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n请输入y/n：", rebuf);  // 输出到终端
                    scanf("%s", buf);
                    write(new_fd, buf, sizeof(buf));  // 发送消息
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端  
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);       
                }
                else if(buf[0] == '2')
                {
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                    scanf("%s", buf);
                    write(new_fd, buf, sizeof(buf));  // 发送消息
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端     
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);      
                }
                else if(buf[0] == '3')
                {
                    printf("----------------用 户 列 表----------------\n");
                    read(new_fd, rebuf, sizeof(struct userstruct));  // 接收新消息
                    while(strcmp(rebuf, "end") != 0)
                    {
                        struct userstruct *ur = (struct userstruct*) &rebuf;
                        if(ur->access)
                        {
                            printf("用户名：%s  身份权限：管理员\n", ur->name);
                        }
                        else
                        {
                            printf("用户名：%s  身份权限：用户\n", ur->name);
                        }
                        write(new_fd, "OK", 3);
                        read(new_fd, rebuf, sizeof(struct userstruct));
                    }
                    printf("------------------到 底 啦------------------\n");            
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);      
                }
                else if(buf[0] == '4')
                {
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                    scanf("%s", buf);
                    write(new_fd, buf, 10);  // 发送消息
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端 
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);          
                }
                else if(buf[0] == '5')
                {
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                    scanf("%s", buf);
                    write(new_fd, buf, sizeof(buf));  // 发送消息
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端     
                    scanf("%s", buf);
                    write(new_fd, buf, sizeof(buf));  // 发送消息    
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);                               
                }
            }            
        }   
        else if(buf[0] == '2')
        {
            while (1)
            {
                printf("      *报账管理平台*        \n");
                printf("---------------------------\n");
                printf("     1.查看所有活动目录     \n");
                printf("     2.选择活动            \n");
                printf("     3.添加活动            \n");
                printf("     4.删除活动            \n");
                printf("     0.返回                \n");
                printf("---------------------------\n");
                printf("请输入：");
                scanf("%s", buf);
                if(buf[0] == '3' && me.access != 1)
                {
                    printf("对不起，您的权限不允许您添加活动\n");
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);    
                    continue; 
                }
                else if(buf[0] == '4' && me.access != 1)
                {
                    printf("对不起，您的权限不允许您删除活动\n");
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);    
                    continue;             
                }
                write(new_fd, buf, sizeof(buf));  // 发送消息
                if(buf[0] == '1')
                {
                    printf("----------------活 动 列 表----------------\n");
                    read(new_fd, rebuf, sizeof(struct object));  // 接收新消息
                    while(strcmp(rebuf, "end") != 0)
                    {
                        struct object *ob = (struct object*) &rebuf;
                        printf("活动项目：%s,创建人：%s,创建时间：%s", ob->item_name, ob->ad_name, asctime(localtime(&ob->t)));
                        write(new_fd, "OK", 3);
                        read(new_fd, rebuf, sizeof(struct object));
                    }
                    printf("------------------到 底 啦------------------\n");            
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);  
                }
                else if(buf[0] == '2')
                {
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                    scanf("%s", buf);   
                    write(new_fd, buf, sizeof(buf));  // 发送消息
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端 
                    if(strcmp(rebuf, "该活动不存在！") == 0)
                    {
                        printf("输入任意键继续...\n");
                        scanf("%s", buf); 
                    }
                    else
                    {
                        while (1)
                        {
                            printf("----------------账 目 列 表----------------\n");
                            read(new_fd, rebuf, sizeof(struct bill));  // 接收新消息
                            while(strcmp(rebuf, "end") != 0)
                            {
                                struct bill *b = (struct bill*) &rebuf;
                                if(b->solve)
                                {
                                    printf("物品：%s,数量：%d,单位：%s,单价：%.2lf,总价：%.2lf\n申请时间：%s经办人：%s,是否已经报销：是\n报销时间：%s负责人：%s,备注：%s\n", b->ob_name, b->num, b->unit, b->unit_pri, b->total, asctime(localtime(&b->t1)), b->ur_name, asctime(localtime(&b->t2)), b->ad_name, b->comments);    
                                }
                                else
                                {
                                    printf("物品：%s,数量：%d,单位：%s,单价：%.2lf,总价：%.2lf\n申请时间：%s经办人：%s,是否已经报销：否\n报销时间：无\n负责人：无,备注：%s\n", b->ob_name, b->num, b->unit, b->unit_pri, b->total, asctime(localtime(&b->t1)), b->ur_name, b->comments);
                                }
                                printf("******************************************************************************************\n");
                                write(new_fd, "OK", 3);
                                read(new_fd, rebuf, sizeof(struct bill));
                            }
                            printf("------------------到 底 啦------------------\n"); 
                            printf("若要新增账目请输入1，删除账目请输入2，报销账目请输入3，返回请输入0\n请输入：");
                            scanf("%s", buf);
                            if(buf[0] == '2' && me.access != 1)
                            {
                                printf("对不起，您的权限不允许您删除账目\n");
                                printf("输入任意键继续...\n");
                                scanf("%s", buf);    
                                continue; 
                            }
                            else if(buf[0] == '3' && me.access != 1)
                            {
                                printf("对不起，您的权限不允许您报销账目\n");
                                printf("输入任意键继续...\n");
                                scanf("%s", buf);    
                                continue;                                 
                            }
                            write(new_fd, buf, MAXSIZE);  // 发送消息
                            if(buf[0] == '1')
                            {
                                struct bill tmp_b;
                                read(new_fd, rebuf, MAXSIZE);
                                printf("请输入物品名称：");
                                scanf("%s", tmp_b.ob_name);
                                printf("请输入物品单位：");
                                scanf("%s", tmp_b.unit);
                                printf("请输入物品数量：");
                                scanf("%d", &tmp_b.num);
                                printf("请输入物品单价：");
                                scanf("%lf", &tmp_b.unit_pri);
                                printf("请输入备注：");
                                scanf("%s", tmp_b.comments);
                                tmp_b.total = tmp_b.unit_pri * tmp_b.num;
                                tmp_b.solve = 0;
                                strcpy(tmp_b.ur_name, me.name);
                                strcpy(tmp_b.ad_name, "NULL");
                                time(&tmp_b.t1);
                                time(&tmp_b.t2);
                                write(new_fd, &tmp_b, sizeof(tmp_b));
                                read(new_fd, rebuf, MAXSIZE);  // 接收新消息
                                printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                                printf("输入任意键继续...\n");
                                scanf("%s", buf);       
                            }
                            else if(buf[0] == '2')
                            {
                                read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                                printf("%s", rebuf);  // 输出到终端
                                scanf("%s", buf);
                                write(new_fd, buf, MAXSIZE);
                                read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                                printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                                printf("输入任意键继续...\n");
                                scanf("%s", buf);                               
                            }
                            else if(buf[0] == '3')
                            {
                                read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                                printf("%s", rebuf);  // 输出到终端
                                scanf("%s", buf);
                                write(new_fd, buf, MAXSIZE);
                                read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                                printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                                printf("输入任意键继续...\n");
                                scanf("%s", buf);  
                            }
                            else if(buf[0] == '0')
                            {
                                break;
                            }                                                        
                        }                        
                    }
                }
                else if(buf[0] == '3')
                {
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                    scanf("%s", buf);   
                    write(new_fd, buf, sizeof(buf));  // 发送消息
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端     
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);  
                    continue;              
                }
                else if(buf[0] == '4')
                {
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端
                    scanf("%s",buf);   
                    write(new_fd, buf, sizeof(buf));  // 发送消息
                    read(new_fd, rebuf, sizeof(rebuf));  // 接收新消息
                    printf("收到服务器消息:%s\n", rebuf);  // 输出到终端     
                    printf("输入任意键继续...\n");
                    scanf("%s", buf);     
                    continue;                    
                }
                else if(buf[0] == '0')
                {
                    break;
                }
            }
        } 
        else if(buf[0] == '0')
        {
            break;
        }
        else
        {
            printf("请输入正确的命令...\n");
        }
    }
    printf("结束通信\n");
    close(new_fd);   
    return 0;
}