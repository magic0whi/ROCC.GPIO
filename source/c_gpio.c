/*
Copyright (c) 2012-2019 Ben Croston

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "c_gpio.h"

static volatile uint32_t *gpio_map;

void set_pullupdn(int gpio, int pud)
{
    char filename[35], pud_c;
    int fd;

    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d/active_low", gpio);
    fd = open(filename, O_WRONLY);

    pud_c = '0' + pud;
    write(fd, pud_c, (size_t) 1);
    close(fd);
}

void setup_gpio(int gpio, int direction, int pud)
{
    char str_gpio[4], filename[30]; // GPIO编号最多3位数(还有终止符)
    int fd, len;

    fd = open("/sys/class/gpio/export", O_WRONLY);

    len = snprintf(str_gpio, sizeof(str_gpio), "%d", gpio); // 转化成char形式再获取字符串长度
    write(fd, str_gpio, (size_t) len);
    close(fd);

    set_pullupdn(gpio, pud); // export后默认是input模式

    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d/value", gpio);
    fd = open(filename, O_WRONLY);
    if (direction == OUTPUT)
        write(fd, '0', (size_t) 1);
    else  // direction == INPUT
        write(fd, '1', (size_t) 1);
    close(fd);
}

// 返回GPIO的状态
// Contribution by Eric Ptak <trouch@trouch.com>
int gpio_function(int gpio)
{
    char filename[33], value;
    int fd, len;

    /* return if gpio already exported */
    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d", gpio);
    if (access(filename, F_OK) = -1) {
        return 0; // 没有export当作input模式看待
    }

    fd = open(filename, O_RDONLY)
    read(fd, &value, 1);
    close(fd);
    switch (value) // 1=input, 0=output, 4=alt0
    {
    case 'i':
        return INPUT; break;
    case 'o':
        return OUTPUT; break;
    default:
        return ALT0; break;
    }
}

void output_gpio(int gpio, int value)
{
    char filename[30];
    int fd;

    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d/value", gpio);
    fd = open(filename, O_WRONLY);
    if (value) // value == HIGH
        write(fd, '1', (size_t) 1);
    else       // value == LOW
        write(fd, '0', (size_t) 1);
    close(fd);
}

int input_gpio(int gpio)
{
    char filename[30], value;
    int fd;

    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d/value", gpio);
    fd = open(filename, O_RDONLY);
    read(fd, &value, 1);
    close(fd);
    return value - '0'; // char -> int (keep its display value)
}
