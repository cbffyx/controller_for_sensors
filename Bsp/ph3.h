#ifndef PH3_H
#define PH3_H

#include <stdint.h>
#include <stdbool.h>



typedef struct 
{
	uint8_t type;
	uint8_t addr;
	uint8_t len;
	uint8_t cmd;
	uint8_t regH;
	uint8_t regL;
	uint8_t dataLen;
	uint8_t state;
	uint8_t val[3];
	uint8_t digits;
	uint8_t name;
	uint8_t unit;
	uint8_t crc;
	uint8_t eof;
}Ph32kFrame;

typedef struct
{
	uint8_t sof;
	uint8_t cmd;
	uint8_t addr;
	uint8_t sign;
	uint8_t dec[2];
	uint8_t point;
	uint8_t crcH;
	uint8_t crcL;
	uint8_t eof;
}Ph35Frame;


float Ph32KppmSample(void);
float GetPH32Kppm(void);
float Ph35ppmSample(void);
float Ph35ppmSampleMAX(void);
float GetPH35ppm(void);

void DisablePH3();
void EnablePH3();


#endif
