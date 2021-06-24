/**
  ******************************************************************************
  * @file    utils.h
  * @author  cbf
  * @version V1.0.0
  * @date    7-7-2019
  * @brief   
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DATA_MODEL_H_
#define _DATA_MODEL_H_

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

typedef struct STATUS_STR{
    uint8_t prev;
    uint8_t curr;
    uint8_t next;
}status_str;

typedef struct OPERATE_STR{
    uint8_t prev;
    uint8_t curr;
    uint8_t next;
    uint8_t is_new_operate;
}operate_str;

typedef struct TASK_STR{
    uint16_t work_flag;               //工作标志
    uint16_t ready_flag;              //准备好标志
    uint16_t timming_time;            //定时执行时间
    uint16_t timming_time_cnt;        //定时执行时间倒计时
    uint16_t result;                  //任务处理结果
}task_str;

enum SYS_STATUS{
    READY,
    ON,
    OFF,
    AUTO,
//    AUTO_ON_TIMMING_OFF,
//    MANUAL_ON_TIMMING_OFF,
    AUTO_ON_TIMMING_TWO_OFF,
    AUTO_ON_TIMMING_THREE_OFF,
    AUTO_ON_TIMMING_FOUR_OFF,
    AUTO_ON_TIMMING_SIX_OFF,
    MANUAL_ON_TIMMING_TWO_OFF,
    MANUAL_ON_TIMMING_THREE_OFF,
    MANUAL_ON_TIMMING_FOUR_OFF,
    MANUAL_ON_TIMMING_SIX_OFF,
    SYS_STATUS_MAX
};


enum TIMMING_OFF{
    IMMEDIATE,
    TWO_OFF,
    THREE_OFF,
    FOUR_OFF,
    SIX_OFF,
    NEVER,
    TIMMING_OFF_MAX
};


enum OPERATE{
    NONE,
    CHOOSE,
    COMFIRM,
    OEVRTIME,
    TIMEUP
};



extern status_str sys_status_str;

extern operate_str sys_operate_str;

extern task_str light_flash_task_str;

extern task_str off_light_off_task_str;

extern task_str oevrtime_handle_task_str;

extern task_str timming_ps_detect_task_str;

extern task_str timming_off_handle_task_str;

extern task_str overtime_status_store_task_str;

uint8_t get_next_status(uint8_t sys_status_str_curr);

uint8_t get_next_auto_on_timming_off_status(uint8_t sys_status_str_curr);

uint8_t get_next_manual_on_timming_off_status(uint8_t sys_status_str_curr);

extern uint8_t auto_flag;
extern uint8_t off_flag;
extern uint8_t auto_two_off_flag;
extern uint8_t auto_three_off_flag;
extern uint8_t auto_four_off_flag;
extern uint8_t auto_six_off_flag;

#endif /* _DATA_MODEL_H_ */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/