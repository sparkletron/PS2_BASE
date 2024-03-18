/*******************************************************************************
 * @file    ps2DataType.h
 * @author  Jay Convertino(electrobs@gmail.com)
 * @date    2024.03.12
 * @brief   Generic data type for ps2 data
 * @version 0.0.0
 *
 * @TODO
 *  - Cleanup interface
 *
 * @license mit
 *
 * Copyright 2024 Johnathan Convertino
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ******************************************************************************/

#ifndef PS2DATATYPE_H_
#define PS2DATATYPE_H_

#include <inttypes.h>
#include "ps2defines.h"

struct s_ps2
{
  volatile uint8_t *p_port;
  uint8_t clkPin;
  uint8_t dataPin;

  volatile enum ackStates lastAckState;
  volatile enum dataStates dataState;
  volatile enum callbackStates callbackState;

  uint8_t lastCMD;

  volatile uint8_t index;

  volatile uint16_t buffer;

  t_PS2userRecvCallback userRecvCallback;
  t_PS2recvCallback recvCallback;

  t_PS2recvCallback responseCallback;
  t_PS2recvCallback callUserCallback;
};

#endif /* PS2DATATYPE_H_ */
