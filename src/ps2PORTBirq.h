/*******************************************************************************
 * @file    ps2PORTBirq.h
 * @author  Jay Convertino(electrobs@gmail.com)
 * @date    2024.03.12
 * @brief   ISR for PS2 keyboard
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

#ifndef PS2PORTBIRQ_H_
#define PS2PORTBIRQ_H_

#include <avr/interrupt.h>
#include <avr/common.h>
#include <avr/io.h>
#include <util/delay.h>
#include "ps2DataType.h"
#include "ps2defines.h"

struct s_ps2 *p_ps2Portb = NULL;

void setPS2_PORTB_Device(struct s_ps2 *p_device)
{
  p_ps2Portb = p_device;
}

ISR(PCINT0_vect)
{
  if(p_ps2Portb == NULL) return;

  if(p_ps2Portb->recvCallback == NULL) return;

  uint8_t pinState = (*(p_ps2Portb->p_port - 2) & (1 << p_ps2Portb->clkPin)) >> p_ps2Portb->clkPin;

  switch(p_ps2Portb->dataState)
  {
    case(send):
      //falling edge
      if(pinState == 0)
      {
        uint8_t portValue = (p_ps2Portb->buffer >> p_ps2Portb->index) & 0x0001;

        //set data on output pin
        *p_ps2Portb->p_port |= (portValue << p_ps2Portb->dataPin);
        *p_ps2Portb->p_port &= (portValue << p_ps2Portb->dataPin) | ~(1 << p_ps2Portb->dataPin);

        p_ps2Portb->index++;

        if(p_ps2Portb->index > MESSAGE_LENGTH)
        {
          //set port to input.
          *(p_ps2Portb->p_port - 1) &= ~(1 << p_ps2Portb->dataPin);
          p_ps2Portb->index  = 0;
          p_ps2Portb->buffer = 0;
          p_ps2Portb->dataState = ck_ack;
        }
      }
      break;
    case(ck_ack):
      //falling edge
      if(pinState == 0)
      {
        //if its one, set to no ack, 0 ack(done)
        p_ps2Portb->lastAckState = (((*(p_ps2Portb->p_port - 2) >> p_ps2Portb->dataPin) & 0x01) ? noack : ack);

        p_ps2Portb->dataState = idle;
      }
      break;
    case(idle):
      //rising edge is not a correct start of recv from idle
      if(pinState == 1) break;

      p_ps2Portb->dataState = recv;
    case(recv):
      //falling edge
      if(pinState == 0)
      {
        //get data from input pin and buffer
        p_ps2Portb->buffer |= ((*(p_ps2Portb->p_port - 2) >> p_ps2Portb->dataPin) & 0x01) << p_ps2Portb->index;

        p_ps2Portb->index++;

        //one we have all the data, process data and clear
        if(p_ps2Portb->index > MESSAGE_LENGTH)
        {
          p_ps2Portb->recvCallback(p_ps2Portb->buffer);
          p_ps2Portb->buffer = 0;
          p_ps2Portb->index = 0;
          p_ps2Portb->dataState = idle;
        }
      }
      break;
    default:
      p_ps2Portb->dataState = idle;
      break;
  }
}

#endif /* PS2PORTBIRQ_H_ */
