#ifndef IAP_H
#define IAP_H

#include <stdint.h>
#include <stdbool.h>


#define RECORD_VALID_FLAG	0xaa

#pragma pack(1)

typedef struct
{
	uint8_t ckSum;
	uint8_t isValid;
}RecordCkDef;

typedef struct
{
	uint32_t size;
	uint32_t crc32;
}AppInfoDef;

typedef struct
{
	AppInfoDef appInfo;
	uint8_t isIaped;
	uint8_t rsv[256-1-sizeof(AppInfoDef)];
}IapHeaderDef;

typedef struct
{
	IapHeaderDef data;
	RecordCkDef ck;
}IapHeaderCrcDef;

#pragma pack()











bool CheckIap(void);
void WriteIapHeadCrc1(void);
unsigned long Crc32(uint32_t *crc_table, unsigned long crc,unsigned char *buffer, unsigned int size);
void init_crc_table(uint32_t *table);


#endif
