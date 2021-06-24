/**
  ******************************************************************************
  * @file    data_model.c 
  * @author  cbf
  * @version V0.0.0
  * @date    7-7-2019
  * @brief   common data
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "data_model.h"

status_str sys_status_str;

operate_str sys_operate_str;

task_str light_flash_task_str;

task_str off_light_off_task_str;

task_str oevrtime_handle_task_str;

task_str timming_ps_detect_task_str;

task_str timming_off_handle_task_str;

task_str overtime_status_store_task_str;

uint8_t get_next_status(uint8_t sys_status_str_curr)
{
    uint8_t status_next;
/*
    if(sys_status_str_curr < MANUAL_ON_TIMMING_OFF)
    {
       status_next = sys_status_str_curr + 1; 
    }else if((sys_status_str_curr > MANUAL_ON_TIMMING_OFF)&&(sys_status_str_curr <= AUTO_ON_TIMMING_SIX_OFF))
    {
       status_next = MANUAL_ON_TIMMING_OFF;
    }else
    {
       status_next = ON;
    }
*/
    //状态切换改成单层循环，即开、关、自动、自动开两小时关、自动开三小时关、自动开四小时关、自动开六小时关、
    //手动开两小时关、手动开三小时关、手动开四小时关、手动开六小时关
    if((sys_status_str_curr >= ON) 
       && (sys_status_str_curr < MANUAL_ON_TIMMING_SIX_OFF))
    {
       status_next = sys_status_str_curr + 1; 
    }else
    {
       status_next = ON;
    }
    return status_next;
}

uint8_t get_next_auto_on_timming_off_status(uint8_t sys_status_str_curr)
{
    uint8_t status_next;
    if(sys_status_str_curr < AUTO_ON_TIMMING_TWO_OFF)
    {
        status_next = AUTO_ON_TIMMING_TWO_OFF;
    }else if(sys_status_str_curr < AUTO_ON_TIMMING_SIX_OFF)
    {
       status_next = sys_status_str_curr + 1; 
    }else
    {
       status_next = AUTO_ON_TIMMING_TWO_OFF;
    }
    return status_next;
}

uint8_t get_next_manual_on_timming_off_status(uint8_t sys_status_str_curr)
{
    uint8_t status_next;
    if(sys_status_str_curr < MANUAL_ON_TIMMING_TWO_OFF)
    {
        status_next = MANUAL_ON_TIMMING_TWO_OFF;
    }else if(sys_status_str_curr < MANUAL_ON_TIMMING_SIX_OFF)
    {
       status_next = sys_status_str_curr + 1; 
    }else
    {
       status_next = MANUAL_ON_TIMMING_TWO_OFF;
    }
    return status_next;
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
