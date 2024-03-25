/*******************************************************************************
 * @file    ps2PORTDirq.h
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

#ifndef PS2PORTDIRQ_H_
#define PS2PORTDIRQ_H_

#include <avr/interrupt.h>
#include <avr/common.h>
#include <avr/io.h>
#include <util/delay.h>
#include "ps2DataType.h"
#include "ps2defines.h"

volatile uint8_t prevPortPins_portd = 0;
volatile uint8_t portMask_portd = 0;
volatile uint8_t numOfdevs_portd = 0;

struct s_ps2 *p_ps2Portd[MAX_NUMBER_OF_DEVS] = {NULL};

void setPS2_PORTD_Device(struct s_ps2 *p_device)
{
  if(numOfdevs_portd < MAX_NUMBER_OF_DEVS)
  {
    p_ps2Portd[numOfdevs_portd] = p_device;
    portMask_portd |= (1 << p_ps2Portd[numOfdevs_portd]->clkPin);
    prevPortPins_portd = *(p_ps2Portd[numOfdevs_portd]->p_port - 2) & portMask_portd;

    numOfdevs_portd++;
  }
}

ISR(PCINT2_vect)
{
  uint8_t index = 0;

  if(p_ps2Portd[index] == NULL) return;

  if(p_ps2Portd[index]->recvCallback == NULL) return;

  if(prevPortPins_portd == (*(p_ps2Portd[0]->p_port - 2) & portMask_portd)) return;

//   check all of the devices, and if there is a difference on the pin interrupt in question break the loop and use that index
//   this is lowest number is highest priority.
  for(index = 0; index < numOfdevs_portd; index++)
  {
    if((*(p_ps2Portd[0]->p_port - 2) & (1 << p_ps2Portd[index]->clkPin)) != (prevPortPins_portd &  (1 << p_ps2Portd[index]->clkPin))) break;
  }

  prevPortPins_portd = (*(p_ps2Portd[0]->p_port - 2) & portMask_portd);

  if(index >= numOfdevs_portd) return;

  uint8_t pinState = (*(p_ps2Portd[index]->p_port - 2) & (1 << p_ps2Portd[index]->clkPin)) >> p_ps2Portd[index]->clkPin;

  switch(p_ps2Portd[index]->dataState)
  {
    case(send):
      //falling edge
      if(pinState == 0)
      {
        uint8_t portValue = (p_ps2Portd[index]->buffer >> p_ps2Portd[index]->index) & 0x0001;

        //set data on output pin
        *p_ps2Portd[index]->p_port |= (portValue << p_ps2Portd[index]->dataPin);
        *p_ps2Portd[index]->p_port &= (portValue << p_ps2Portd[index]->dataPin) | ~(1 << p_ps2Portd[index]->dataPin);

        p_ps2Portd[index]->index++;

        if(p_ps2Portd[index]->index > MESSAGE_LENGTH)
        {
          //set port to input.
          *(p_ps2Portd[index]->p_port - 1) &= ~(1 << p_ps2Portd[index]->dataPin);
          p_ps2Portd[index]->index = 0;
          p_ps2Portd[index]->buffer = 0;
          p_ps2Portd[index]->dataState = ck_ack;
        }
      }
      break;
    case(ck_ack):
      //falling edge
      if(pinState == 0)
      {
        //if its one, set to no ack, 0 ack(done)
        p_ps2Portd[index]->lastAckState = (((*(p_ps2Portd[index]->p_port - 2) >> p_ps2Portd[index]->dataPin) & 0x01) ? noack : ack);

        p_ps2Portd[index]->dataState = idle;
      }
      break;
    case(idle):
      //rising edge is not a correct start of recv from idle
      if(pinState == 1) break;

      p_ps2Portd[index]->dataState = recv;
    case(recv):
      //falling edge
      if(pinState == 0)
      {
        //get data from input pin and buffer
        p_ps2Portd[index]->buffer |= ((*(p_ps2Portd[index]->p_port - 2) >> p_ps2Portd[index]->dataPin) & 0x01) << p_ps2Portd[index]->index;

        p_ps2Portd[index]->index++;

        //one we have all the data, process data and clear
        if(p_ps2Portd[index]->index > MESSAGE_LENGTH)
        {
          p_ps2Portd[index]->recvCallback(p_ps2Portd[index]->buffer);
          p_ps2Portd[index]->buffer = 0;
          p_ps2Portd[index]->index = 0;
          p_ps2Portd[index]->dataState = idle;
        }
      }
      break;
    default:
      p_ps2Portd[index]->dataState = idle;
      break;
  }
}

#endif /* PS2PORTDIRQ_H_ */
