//CACP04.h
#ifndef CACP04_H_INCLUDED
#define CACP04_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

enum CACP04_status {
	CACP04_status_ok,
	CACP04_status_no_i2c_acknowledge,
	CACP04_status_i2c_transfer_error,
	CACP04_status_crc_error
};

void CACP04_init(void);


double CACP04_Result_Double(uint8_t *buf);
float CACP04_Result_Float(uint8_t *buf);
float GetCap();
float GetMoisture(float temp,float humi);

enum status_code CACP04_Read_Result(uint8_t addr,double *Result);



#endif /* CACP04_H_INCLUDED */
