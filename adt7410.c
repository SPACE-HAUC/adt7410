/**
 * @file adt7410.c
 * @author Sunip K. Mukherjee (sunipkmukherjee@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "adt7410.h"
#include <stdio.h>

#define eprintf(str, ...) \
    fprintf(stderr, "%s, %d: " str "\n", __func__, __LINE__, ##__VA_ARGS__); \
    fflush(stderr)



int adt7410_init(adt7410 *dev, int bus, int addr)
{
    if (i2cbus_open(dev, bus, addr) < 0)
    {
        eprintf("Could not open bus %d: device 0x%02x", bus, addr);
        return -1;
    }
    uint8_t val;
    if (adt7410_read8(dev, ADT7410_REG_IDENT, &val) < 0)
    {
        eprintf("Could not read identity");
        goto err_dev_close;
    }
    if ((val & 0xc0) != 0xc0)
    {
        eprintf("Identity not matching: 0x%02x", val);
        goto err_dev_close;
    }
    if (adt7410_write16(dev, ADT7410_REG_THIGH, ADT7410_TMP_HIGH) < 0)
    {
        eprintf("Could not set high temp");
    }
    if (adt7410_write16(dev, ADT7410_REG_TLOW, ADT7410_TMP_LOW) < 0)
    {
        eprintf("Could not set high temp");
    }
    if (adt7410_write16(dev, ADT7410_REG_TCRIT, ADT7410_TMP_CRIT) < 0)
    {
        eprintf("Could not set high temp");
    }
    return 1;
err_dev_close:
    i2cbus_close(dev);
    return -1;
}

int adt7410_read8(adt7410 *dev, uint8_t addr, uint8_t *val)
{
#ifdef ADT7410_DEBUG
    eprintf("Address: 0x%x", addr);
#endif
    char o_buf[1] = {addr};
    return i2cbus_xfer(dev, o_buf, 1, val, 1, 0);
}

int adt7410_read16(adt7410 *dev, uint8_t addr, int16_t *val)
{
#ifdef ADT7410_DEBUG
    eprintf("Address: 0x%x", addr);
#endif
    char o_buf[1] = {addr};
    *val = 0;
    char *i_buf = val; // reinterpret pointer
    if (i2cbus_xfer(dev, o_buf, 1, i_buf + 1, 1, 0) < 0)
    {
        eprintf("Could not receive high byte");
        return -1;
    }
    o_buf[0]++; // next addr
    return i2cbus_xfer(dev, o_buf, 1, i_buf, 1, 0);
}

int adt7410_write8(adt7410 *dev, uint8_t addr, uint8_t val)
{
#ifdef ADT7410_DEBUG
    eprintf("Address: 0x%x", addr);
#endif
    char buf[2] = {addr, val};
    return i2cbus_write(dev, buf, 2);
}

int adt7410_write16(adt7410 *dev, uint8_t addr, int16_t val)
{
#ifdef ADT7410_DEBUG
    eprintf("Address: 0x%x", addr);
#endif
    char buf[4] = {addr, (val & 0xff00) >> 8, addr + 1, val & 0xff};
    return i2cbus_write(dev, buf, 4);
}

int adt7410_read_temp(adt7410 *dev, float *temp)
{
    short val = 0;
    if (adt7410_read16(dev, ADT7410_REG_TMP, &val) < 0)
    {
        eprintf("Error reading temperature");
        return -1;
    }
    *temp = val * 0.0625;
    return 1;
}

int adt7410_read_status(adt7410 *dev, uint8_t *status)
{
    if (adt7410_read8(dev, ADT7410_REG_STAT, status) < 0)
    {
        eprintf("Could not read status");
        return -1;
    }
    uint8_t stat = *status;
    stat >>= 4; // bottom 4 bits are useless
    if (stat & 0x1)
    {
        eprintf("Temperature too low");
    }
    if (stat & 0x2)
    {
        eprintf("Temperature too high");
    }
    if (stat & 0x4)
    {
        eprintf("Temperature critial");
    }
    return 1;
}

void adt7410_destroy(adt7410 *dev)
{
    i2cbus_close(dev);
}

#ifdef ADT7410_UNIT_TEST
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t done = 0;
void sighandler(int sig)
{
    done = 1;
}

int main(int argc, char *argv[])
{
    if (!((argc == 2) || (argc == 3)))
    {
        printf("Invocation: %s <Bus ID> [<Optional I2C address (hex), default 0x4b>]\n\n", argv[0]);
        return 0;
    }
    signal(SIGINT, &sighandler);
    int bus = atoi(argv[1]);
    int addr = 0x4b;
    if (argc == 3)
        addr = strtol(argv[2], NULL, 16);
    adt7410 dev[1];
    if (adt7410_init(dev, bus, addr) < 0)
    {
        eprintf("Could not initialize device");
        return 0;
    }
    int out_char = 0;
    float temp = 0;
    while(!done)
    {
        while(out_char--)
            printf(" ");
        printf("\r");
        adt7410_read_temp(dev, &temp);
        out_char = printf("Temperature: %.2f C\r", temp);
        fflush(stdout);
        sleep(1);
    }
    adt7410_destroy(dev);
    return 0;
}
#endif