/*
 * Copyright (c) 2019, redocCheng
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-19     redoc        the first version
 */

#ifndef __DRV_MAX7219_H
#define __DRV_MAX7219_H
#include <rtthread.h>
#include "rtdevice.h"
#include "max7219_cfg.h"


/* check chips number definition */
#if !defined(MAX7219_CHIPS_NUMBER)
#error "You must defined MAX7219_CHIPS_NUMBER on 'max7219_cfg.h'"
#endif

#if (MAX7219_CHIPS_NUMBER == 0)
#error "MAX7219_CHIPS_NUMBER can't be 0, You must redefined it on 'max7219_cfg.h'"
#endif

/* check partition table definition */
#if !defined(MAX7219_CHIPS_SCAN_NUMBER_TABLE)
#error "You must defined MAX7219_CHIPS_SCAN_NUMBER_TABLE on 'max7219_cfg.h'"
#endif

/*  单片最大8个 */
#define MAX7219_SIGLE_DIG_NUM_MAX     (8)
#define MAX7219_POSITION_BUF_NUM      (MAX7219_CHIPS_NUMBER*MAX7219_SIGLE_DIG_NUM_MAX)

typedef enum
{
    SHUTDOWN_MODE_SHUTDOWN = 0,
    SHUTDOWN_MODE_NORMAL,

}shutdown_t;

typedef enum
{
    DECODE_MODE_NO_DEC_FOR_8_1                  = 0x00,
    DECODE_MODE_CODE_B_FOR_1_NO_DEC_FOR_8_2     = 0x01,   /*   不支持   */
    DECODE_MODE_CODE_B_FOR_4_1_NO_DEC_FOR_8_5   = 0x0f,   /*   不支持   */
    DECODE_MODE_CODE_B_FOR_8_1                  = 0xff,   /*   不支持   */

}decode_mode_t;

typedef enum
{
    TEST_MODE_NORMAL = 0,
    TEST_MODE_DISPLAY_TEST = 1,

}test_mode_t;

/* 功能寄存器宏定义 */
#define REG_ADDR_NO_OP        (0x00)
#define REG_ADDR_DIGIT_0      (0x01)
#define REG_ADDR_DIGIT_1      (0x02)
#define REG_ADDR_DIGIT_2      (0x03)
#define REG_ADDR_DIGIT_3      (0x04)
#define REG_ADDR_DIGIT_4      (0x05)
#define REG_ADDR_DIGIT_5      (0x06)
#define REG_ADDR_DIGIT_6      (0x07)
#define REG_ADDR_DIGIT_7      (0x08)
#define REG_ADDR_DECODEMODE   (0X09)     /*   译码控制寄存器地址   */
#define REG_ADDR_INTENSITY    (0X0A)     /*   亮度控制寄存器地址   */
#define REG_ADDR_SCANLIMIT    (0X0B)     /*   扫描界限寄存器地址   */
#define REG_ADDR_SHUTDOWN     (0X0C)     /*   关断模式寄存器地址   */
#define REG_ADDR_DISPTEST     (0X0F)     /*   测试控制寄存器地址   */


/*  支持非编码模式和全编码模式，建议只用非编码模式  所以这里不加编码配置选项  */
#define  MAX7219_INFO_DEFAULT          \
{                                      \
    SHUTDOWN_MODE_NORMAL,              \
    DECODE_MODE_NO_DEC_FOR_8_1,        \
    0x0F,                              \
    TEST_MODE_NORMAL,                  \
    MAX7219_CHIPS_SCAN_NUMBER_TABLE,   \
    0,                                 \
}

/*   max7219位置结构体   */
typedef struct
{
	uint16_t chip;		/*  芯片在全局位置          */
	uint8_t  dig;		/*  管脚在芯片的位置        */
    bool flip;	        /*  翻转显示功能开关        */
	
}max7219_position_t;

/*   max7219旋转180度位置结构体   */
typedef union
{
	struct{
        uint8_t bit0_g:1;               
        uint8_t bit1_f:1;                   
        uint8_t bit2_e:1;                   
        uint8_t bit3_d:1;               
        uint8_t bit4_c:1;                   
        uint8_t bit5_b:1;  
        uint8_t bit6_a:1;               
        uint8_t bit7_dp:1;                             
    }b;
    uint8_t w;
	
}max7219_flip_bits;

/*   max7219设置结构体   */
struct rt_device_max7219_info
{
    shutdown_t         shutdown_mode;                            /*  关断模式        */
    decode_mode_t      decode_mode;	                             /*  译码模式        */
    uint8_t            intensity;                                /*  亮度(0-f)       */
    uint8_t            work_mode;                                /*  工作模式        */
    uint8_t            scan_num_buf[MAX7219_CHIPS_NUMBER];       /*  单片扫描个数    */
	max7219_position_t position_buf[MAX7219_POSITION_BUF_NUM];   /*  定位buf         */
    uint16_t           scan_nums;                                /*  扫描总数        */
};

int max7219_clear_all(void);
int max7219_write_dig(uint16_t dig, uint8_t data);
int max7219_write(uint16_t dig, uint8_t data);
int max7219_intensity_set(uint8_t value);
int max7219_position_flip_set(uint16_t dig, uint8_t data);
#endif
