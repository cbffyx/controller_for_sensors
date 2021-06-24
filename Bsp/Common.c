#include "string.h"
#include "stdint.h"
#include "Common.h"

/*
�������ܣ������鳤��strlen��Χ�в���ƥ��ָ����ʼ�ַ���ָ�������ַ�֮������鷵�ظ�
������ڣ�
uint8_t *str ��Ҫ���ҵ�����
uint16_t strlen�����ҷ�Χ ָ�����ȷ�Χ
char str1����ʼ�ַ�
uint16_t Nostr1���ڼ�����ʼ�ַ�
char str2�������ַ�
uint16_t Nostr1���ڼ��������ַ�
uint16_t *substrlen�����ز��ҽ���ĳ���
���أ�����ҵ����򷵻ز��ҵ�λ�õ���ʼָ�룬δ�ҵ�������NULL
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
            break;  //��β�ַ����ҵ�
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

//--------------------------------����������-------------------------------//
/**
  * ��飺�ж�һ���ַ������Ƿ������һ���ַ�����              
  * ������   
  * ����ֵ��NULL ������
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
  * ��飺�ַ���ת��Ϊ16���ơ�              
  * ������   
  * ����ֵ����
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
  * ��飺���������ַ������������ϻس���              
  * ������   
  * ����ֵ����
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
  * ��飺���������ַ��� ��󲻼ӻس���              
  * ������   
  * ����ֵ����
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
  * ��飺���������ַ�����              
  * ������   
  * ����ֵ����
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
  * ��飺����ת�ַ�����              
  * ������   
  * ����ֵ����
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
  * ��飺���鸴�ơ�              
  * ������   
  * ����ֵ����
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
