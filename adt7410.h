/**
 * @file adt7410.h
 * @author Sunip K. Mukherjee (sunipkmukherjee@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _ADT7410_H
#define _ADT7410_H
#ifdef __cplusplus
// extern "C" {
#endif
typedef enum
{
    ADT7410_REG_TMP,
    ADT7410_REG_STAT = 0x2,
    ADT7410_REG_CONFIG = 0x3,
    ADT7410_REG_THIGH,
    ADT7410_REG_TLOW = 0x6,
    ADT7410_REG_TCRIT = 0x8,
    ADT7410_REG_THYST = 0xa,
    ADT7410_REG_IDENT = 0xb,
    ADT7410_REG_RESET = 0x2f
} adt7410_regs;

const unsigned char ADT7410_IDENT = 0xcb;

const short ADT7410_TMP_HIGH = 70 / 0.0625;
const short ADT7410_TMP_LOW = -35 / 0.0625;
const short ADT7410_TMP_CRIT = 80 / 0.0625;

#include "i2cbus/i2cbus.h"
#include <stdint.h>

typedef i2cbus adt7410;

int adt7410_init(adt7410 *dev, int bus, int addr);

int adt7410_read_temp(adt7410 *dev, float *temp);

int adt7410_read_status(adt7410 *dev, uint8_t *status);

int adt7410_read8(adt7410 *dev, uint8_t addr, uint8_t *val);

int adt7410_read16(adt7410 *dev, uint8_t addr, int16_t *val);

int adt7410_write8(adt7410 *dev, uint8_t addr, uint8_t val);

int adt7410_write16(adt7410 *dev, uint8_t addr, int16_t val);

void adt7410_destroy(adt7410 *dev);

#ifdef __cplusplus
// }
#endif
#endif