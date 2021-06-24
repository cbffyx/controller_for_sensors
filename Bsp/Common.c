#include "string.h"
#include "stdint.h"
#include "Common.h"

/*
函数功能：在数组长度strlen范围中查找匹配指定开始字符和指定结束字符之间的数组返回给
函数入口：
uint8_t *str ：要查找的数组
uint16_t strlen：查找范围 指定长度范围
char str1：开始字符
uint16_t Nostr1：第几个开始字符
char str2：结束字符
uint16_t Nostr1：第几个结束字符
uint16_t *substrlen：传回查找结果的长度
返回：如果找到，则返回查找到位置的起始指针，未找到，返回NULL
*/
uint8_t *GetStringMiddle(uint8_t *str, uint16_t strlen,
                         char str1, uint16_t Nostr1,
                         char str2, uint16_t Nostr2,
                         uint16_t *substrlen)
{
    uint16_t No1p;
    uint16_t No2p;
    uint16_t i;
    uint8_t *Firststr = NULL;
    uint8_t *Secstr = NULL;
    uint8_t Str1Ok;
    uint8_t Str2Ok;

    if(strlen==0)
    {
        return 0;
    }

    No1p=0;
    No2p=0;
    Str1Ok=0;
    Str2Ok=0;

    for (i=0; i<strlen; i++)
    {
        if (*(str+i)==str1)
        {
            No1p++;
            if (No1p==Nostr1)
            {
                Firststr=str+i;
                Str1Ok=1;
            }
        }

        if (*(str+i)==str2)
        {
            No2p++;
            if (No2p==Nostr2)
            {
                Secstr=str+i;
                Str2Ok=1;
            }
        }
        if (Str1Ok && Str2Ok)
        {
            break;  //锟斤拷尾锟街凤拷锟斤拷锟揭碉拷
        }
    }

    if (Str1Ok&&Str2Ok)
    {
        if ((Secstr-Firststr)>=0)
        {
            *substrlen=Secstr-Firststr-1;
            return Firststr+1;
        }
        else
        {
            *substrlen=Firststr-Secstr-1;
            return Secstr+1;
        }
    }
    else
    {
        return 0;
    }
}

//--------------------------------公共函数集-------------------------------//
/**
  * 简介：判断一个字符串中是否包含另一个字符串。              
  * 参数：   
  * 返回值：NULL 不包含
  */
char *StringStr(char *str, char *dest)
{
#define STR_BUFF_LEN    0x100
    int i = STR_BUFF_LEN;
    char *cp = str;
    char *s1, *s2;
    
    if (*dest == 0)
    {
        return str;
    }
    
    while(i--)
    {        
        s1 = cp;
        s2 = dest;
        
        while((*s1 == *s2) && *s1 && *s2)
        {
            s1++;
            s2++;
        }
        if(!*s2)
            return cp;
        cp++;
    }
    
    return NULL;
}



/**
  * 简介：字符串转换为16进制。              
  * 参数：   
  * 返回值：无
  */
uint8_t StrToHex(uint8_t temp)
{
    uint8_t ret=0;
    
    if(temp>=48 && temp<=57)
    {
        ret = temp - 48;
        return ret;
    }
    
    if(temp>=65 && temp<=70)
    {
        ret = temp - 55;
        return ret;
    }
    
    if(temp>=97 && temp<=102)
    {
        ret = temp - 87;
        return ret;
    }
    
    return ret; 
}

/**
  * 简介：连接两个字符串并在最后加上回车。              
  * 参数：   
  * 返回值：无
  */
uint8_t *StringConcat(uint8_t *str, const uint8_t *string)
{
    uint8_t *s = str;
    
    while(*s)
    {
        s++;
    }
    
    while(*string)
    {
        *s++ = *string++;
    }
    
    *s++ = '\r';
    *s++ = '\n';
    *s = '\0';
    
    return str;     
}

/**
  * 简介：连接两个字符串 最后不加回车。              
  * 参数：   
  * 返回值：无
  */
uint8_t *StringConcatN(uint8_t *str, const uint8_t *string)
{
    uint8_t *s = str;
    
    while(*s)
    {
        s++;
    }
    
    while(*string)
    {
        *s++ = *string++;
    }
    
    *s = '\0';
    
    return str;     
}

/**
  * 简介：连接两个字符串。              
  * 参数：   
  * 返回值：无
  */
uint8_t *StringConcat2(uint8_t *str, const uint8_t *string)
{
    uint8_t *s = str;
    
    while(*s)
    {
        s++;
    }
    
    while(*string)
    {
        *s++ = *string++;
    }
    
    return str;        
}

/**
  * 简介：整型转字符串。              
  * 参数：   
  * 返回值：无
  */
void IntToStr(uint8_t* str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;
    
    if(intnum < 0)
    {
        intnum = intnum*(-1);
        str[j++] = '-';
    }
    
    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48; /* '0' */
        
        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/**
  * 简介：数组复制。              
  * 参数：   
  * 返回值：无
  */
int CopyArray(uint8_t *str, const uint8_t *string)
{
    if(str == NULL | *string == NULL)
        return -1;    
    while(*string)
    {
        *str++ = *string++;
    }
    return 0;
}
