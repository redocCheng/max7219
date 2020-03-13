/*
 * Copyright (c) 2019, Anke Development Team
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
#include <drv_common.h>

// max7219 spi_bus
#define MAX7219_SPI_BUS_NAME  	 "spi1"

//max7219 spi_device_name
#define MAX7219_SPI_DEVICE_NAME  "spi10"

#define RT_MAX7219_DEFAULT_SPI_CFG                         \
{                                                          \
    .mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB,    \
    .data_width = 8,                                       \
    .max_hz = 500 * 1000,                                  \
}

typedef enum
{
    SHUTDOWN_MODE_SHUTDOWN = 0,
    SHUTDOWN_MODE_NORMAL,
	
}shutdown_t;

typedef enum
{
    DECODE_MODE_NO_DEC_FOR_7_0 = 0,
    DECODE_MODE_CODE_B_FOR_0_NO_DEC_FOR_7_1 = 0x01,
    DECODE_MODE_CODE_B_FOR_3_0_NO_DEC_FOR_7_4 = 0x0f,
    DECODE_MODE_CODE_B_FOR_7_0 = 0xff,
	
}decode_mode_t;

typedef enum
{
    TEST_MODE_NORMAL = 0,
    TEST_MODE_DISPLAY_TEST = 1,
	
}test_mode_t;


//功能寄存器宏定义
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


/* max7219扫描个数  */
#define MAX7219_SCAN_NUM  (0x06) 

#define  MAX7219_INFO_DEFAULT    \
{                                \
    SHUTDOWN_MODE_NORMAL,        \
    DECODE_MODE_NO_DEC_FOR_7_0,  \
    0x0F,                        \
    TEST_MODE_NORMAL,            \
    MAX7219_SCAN_NUM,            \
}

/*   max7219设置结构体   */
struct rt_device_max7219_info
{
    shutdown_t shutdown_mode;   /*  关断模式    */
    decode_mode_t decode_mode;	/*  译码模式    */
    uint8_t intensity;          /*  亮度(0-f)   */
    uint8_t work_mode;          /*  工作模式    */
    uint8_t scan_num;		    /*  扫描个数    */
};                                             

struct drv_max7219_device
{
    struct rt_spi_device *spi_device;
    struct rt_device_max7219_info info;
};

void max7219_init(void);
rt_err_t max7219_clear(uint8_t dig);
rt_err_t max7219_clear_all(void);
rt_err_t max7219_write_num(uint8_t dig, uint8_t data);
rt_err_t max7219_write(uint8_t dig, uint8_t data);
rt_err_t max7219_intensity_set(uint8_t value);

#endif
