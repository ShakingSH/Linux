// manager_bill.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <netinet/in.h>	 
#include <arpa/inet.h>	 
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>

struct object
{
    char item_name[15];
    char ad_name[15];
    time_t t;
};

struct bill
{
    char ob_name[15];  // 物品名称
    int num;  // 数量
    char unit[5];  // 单位
    double unit_pri;  // 单价
    double total;  // 总价
    time_t t1;  // 申请时间
    char ur_name[15];  // 经办人
    int solve;  // 是否已经报销
    time_t t2;  // 报销时间
    char ad_name[15];  // 负责人
    char comments[30];  // 备注
};

// 活动查询
int ob_query(struct object ob[], int num, char *name)
{
    int i;
    for(i = 0; i < num; i++)
    {
        if(strcmp(ob[i].item_name, name) == 0)
        {
            break;
        }
    }
    if(i == num)
    {
        return -1;
    }
    else
    {
        return i;
    }
}

// 物品查询
long int bill_query(char *pathname, char *name)
{
    FILE *fp = fopen(pathname, "rb");
    struct bill tmp_b;
    int found = 0;
    int cnt = 0;
    while(fread(&tmp_b, sizeof(struct bill), 1, fp))
    {
        if(strcmp(tmp_b.ob_name, name) == 0)
        {
            found = 1;
            break;
        }
        cnt++;
    }
    fclose(fp);
    if(found)
    {
        return cnt;
    }
    else
    {
        return -1;
    }
}

// 新增物品
int bill_insert(char *pathname, struct bill *b)
{
    FILE *fp = fopen(pathname, "a+b");
    if(fwrite(b, sizeof(struct bill), 1, fp))
    {
        fclose(fp);
        return 1;
    }
    else
    {
        fclose(fp);
        return 0;
    }
}

// 删除物品
int bill_remove(char *pathname, char *name)
{
    struct bill b;
    FILE *fp;
    FILE *fpn;
    if((fpn = fopen("./tmpfile", "w")) == NULL)
    {
        return 0;
    }
    if((fp = fopen(pathname, "r")) == NULL)
    {
        return 0;
    }
    while(fread(&b, sizeof(b), 1, fp) == 1)  // 循环复制，与输入相匹配的不复制
    {
        if(strcmp(name, b.ob_name) != 0)
        {
            fwrite(&b, sizeof(b), 1, fpn);  // 不相同，则复制
        }
        memset(&b, 0x00, sizeof(b));
    }
    fclose(fp);
    fclose(fpn);
    remove(pathname);  // 删除原档案文件
    rename("./tmpfile", pathname);  // 复制好的新文件重命名为档案文件
    return 1;
}

int bill_update(char *pathname, char * ad_name, int i)
{
    FILE *fp;
    struct bill tmp_b;
    fp = fopen(pathname, "r+b");
    fseek(fp,i*sizeof(struct bill),SEEK_SET);
    fread(&tmp_b, sizeof(tmp_b), 1, fp);
    fseek(fp,i*sizeof(struct bill),SEEK_SET);
    if(tmp_b.solve)
    {
        fclose(fp);
        return 0;
    }
    else
    {
        tmp_b.solve = 1;
        strcpy(tmp_b.ad_name, ad_name);
        time(&tmp_b.t2);
        fwrite(&tmp_b, sizeof(tmp_b), 1, fp);
        fclose(fp);
        return 1;
    }
}