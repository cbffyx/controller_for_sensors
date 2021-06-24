#ifndef __COMMON_H
#define __COMMON_H

#include "string.h"
#include "stdint.h"

uint8_t *GetStringMiddle(uint8_t *str, uint16_t strlen,char str1, uint16_t Nostr1,char str2, uint16_t Nostr2,uint16_t *substrlen);

char *StringStr(char *str, char *dest);
uint8_t StrToHex(uint8_t temp);
uint8_t *StringConcat(uint8_t *str, const uint8_t *string);
uint8_t *StringConcat2(uint8_t *str, const uint8_t *string);
uint8_t *StringConcatN(uint8_t *str, const uint8_t *string);
void IntToStr(uint8_t* str, int32_t intnum);
int CopyArray(uint8_t *str, const uint8_t *string);

#endif


