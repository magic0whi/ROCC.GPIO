/*
Copyright (c) 2013-2014 Ben Croston

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

#include "Python.h"
#include "c_gpio.h"
#include "common.h"

int gpio_mode = MODE_UNKNOWN;
// Board编号1~40
const int pin_to_gpio[41] = { -1, -1, -1, 89, -1, 88, -1, 55, 100, -1, 102, 84, 70, 85, -1, 86, 103, -1, 101, 97, -1, 98, 2, 96, 104, -1, 76, 68, 69, 83, -1, 87, 0, 80, -1, 82, 64, 79, 65, -1, 27 };
// BCM编号0~27
const int pin_to_gpio_bcm[41] = { 69, 89, 88, 55, 83, 87, 76, 104, 98, 97, 96, 0, 80, 100, 102, 64, 84, 70, 82, 65, 27, 86, 103, 101, 2, 79, 85}
int setup_error = 0;

int check_gpio_priv(void)
{
    // check module has been imported cleanly
    if (setup_error)
    {
        PyErr_SetString(PyExc_RuntimeError, "Module not imported correctly!");
        return 1;
    }
    return 0;
}

// 修改后返回devfs的id
int get_gpio_number(int channel, unsigned int *gpio)
{
    // check setmode() has been run
    if (gpio_mode != BOARD && gpio_mode != BCM)
    {
        PyErr_SetString(PyExc_RuntimeError, "Please set pin numbering mode using GPIO.setmode(GPIO.BOARD) or GPIO.setmode(GPIO.BCM)");
        return 3;
    }

    // check channel number is in range
    if ( (gpio_mode == BCM && (channel < 0 || channel > 53))
      || (gpio_mode == BOARD && (channel < 1 || channel > 40))
    {
        PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Raspberry Pi");
        return 4;
    }

    // convert channel to gpio
    if (gpio_mode == BOARD)
    {
        if (pin_to_gpio[channel] == -1)
        {
            PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Raspberry Pi");
            return 5;
        } else {
            *gpio = pin_to_gpio[channel];
        }
    }
    else // gpio_mode == BCM
    {
        *gpio = pin_to_gpio_bcm[channel];
    }

    return 0;
}
