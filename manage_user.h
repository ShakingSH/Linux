// manager_user.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <netinet/in.h>	 
#include <arpa/inet.h>	 
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>

#define ARFILE "./user.ar"  // 指定档案文件的路径名称

// 用户的账号、密码、权限
struct userstruct
{
    char name[15];
    char password[15];
    int access;
};

// 删除用户函数
int removeuser(char *name)
{
    struct userstruct user;
    FILE *fp;
    FILE *fpn;
    int ret = 1;
    if((fpn = fopen("./tmpfile", "w")) == NULL)
    {
        return 0;
    }
    if((fp = fopen(ARFILE, "r")) == NULL)
    {
        return 0;
    }
    while(fread(&user, sizeof(user), 1, fp) == 1)  // 循环复制，与输入姓名相匹配的不复制
    {
        if(strcmp(name, user.name) != 0)
        {
            fwrite(&user, sizeof(user), 1, fpn);  // 不相同，则复制
        }
        else if(user.access == 1)
        {

            fwrite(&user, sizeof(user), 1, fpn);  // 管理员，则复制  
            ret = -1;           
        }
        memset(&user, 0x00, sizeof(user));
    }
    fclose(fp);
    fclose(fpn);
    remove(ARFILE);  // 删除原档案文件
    rename("./tmpfile", ARFILE);  // 复制好的新文件重命名为档案文件
    return ret;
}

// 查询员工函数
int queryuser(char *name)
{
    int found = 0;
    struct userstruct user;
    FILE *fp;
    if((fp = fopen(ARFILE, "r")) == NULL)
    {
        return 0;
    }
    while(fread(&user, sizeof(user), 1, fp) == 1)
    {
        if(strcmp(name, user.name) == 0)
        {
            found = 1;
            break;
        }
        memset(&user, 0x00, sizeof(user));
    }
    fclose(fp);
    return found;
}

// 增加用户函数
int insertuser(struct userstruct ur)
{
    FILE *fp;
    if((fp = fopen(ARFILE, "a")) == NULL)
    {
        return 0;
    }
    if(fwrite(&ur, sizeof(ur), 1, fp) < 0)
    {
        perror("fwrite");
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}

// 修改用户函数
int updateuser(struct userstruct new_user)
{
    struct userstruct old_user;
    FILE *fp;
    fpos_t pos;
    int found = -1;
    int cnt = 0;
    if((fp = fopen(ARFILE, "r+")) == NULL)
    {
        return 0;
    }
    while(fread(&old_user, sizeof(old_user), 1, fp) == 1)  // 循环复制，与输入姓名相匹配的不复制
    {
        if(strcmp(old_user.name, new_user.name) != 0)
        {
            cnt++;
        }
        else if(strcmp(old_user.name, new_user.name) == 0)
        {
            if(old_user.access)
            {
                return -2;
            }
            else
            {
                found = 1;
                pos.__pos = (long)(cnt * sizeof(struct userstruct));  // 设置移动量为一个结构体
                fsetpos(fp, &pos);  // 另外一种移动文件指针位置的方法
                fwrite(&new_user, sizeof(new_user), 1, fp);                
            }

        }
        memset(&old_user, 0x00, sizeof(old_user));
    }
    return found;
}