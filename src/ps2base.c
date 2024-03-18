/*******************************************************************************
 * @file    psKeyboard.c
 * @author  Jay Convertino(electrobs@gmail.com)
 * @date    2024.03.12
 * @brief   ps2 keyboard driver
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


#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <avr/common.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ps2base.h"

//helper functions
void sendCommand(struct s_ps2 *p_ps2, uint8_t command)
{
  p_ps2->lastCMD = command;

  sendData(p_ps2, command);
}

void sendCommand_noack(struct s_ps2 *p_ps2, uint8_t command)
{
  p_ps2->lastCMD = command;

  sendData_noack(p_ps2, command);
}

void sendData(struct s_ps2 *p_ps2, uint8_t data)
{
  uint8_t tmpSREG = 0;
  uint16_t tempConv = 0;

  tmpSREG = SREG;

  waitForDataIdle(p_ps2);

  cli();

  tempConv = dataToPacket(data);

  copyPacketToBuffer(p_ps2, tempConv);

  startTransmit(p_ps2);

  SREG = tmpSREG;

  waitingForCallback(p_ps2);
}

void sendData_noack(struct s_ps2 *p_ps2, uint8_t data)
{
  uint8_t tmpSREG = 0;
  uint16_t tempConv = 0;

  tmpSREG = SREG;

  waitForDataIdle(p_ps2);

  cli();

  tempConv = dataToPacket(data);

  copyPacketToBuffer(p_ps2, tempConv);

  startTransmit(p_ps2);

  //no ack
  p_ps2->recvCallback = p_ps2->callUserCallback;
  p_ps2->callbackState = no_cmd;

  SREG = tmpSREG;
}

enum callbackStates waitingForCallback(struct s_ps2 *p_ps2)
{
  while(p_ps2->callbackState == waiting);

  return p_ps2->callbackState;
}

void waitForDataIdle(struct s_ps2 *p_ps2)
{
  while(p_ps2->dataState != idle);
}

void waitForDevReady(struct s_ps2 *p_ps2)
{
  while(p_ps2->callbackState != ready_cmd);
}

void waitForDevID(struct s_ps2 *p_ps2)
{
  while(p_ps2->callbackState != dev_id);
}

//FA is ACK
void waitForCMDack(struct s_ps2 *p_ps2)
{
  while(p_ps2->callbackState != ack_cmd);
}


uint8_t convertToRaw(uint16_t ps2data)
{
  uint8_t tmpParity = 0;

  tmpParity = oddParityGen((uint8_t)((ps2data >> DATA_BIT0_POS) & 0x00FF));

  if(tmpParity != (uint8_t)((ps2data >> PARITY_BIT_POS) & 0x0001)) return 0;

  return (uint8_t)((ps2data >> DATA_BIT0_POS) & 0x00FF);
}

uint8_t oddParityGen(uint16_t data)
{
  //setting to 1 generates odd parity. 0 for even.
  uint8_t tempParity = 1;
  uint8_t index = 0;

  for(index = 0; index < sizeof(uint16_t)*8; index++)
  {
    tempParity ^= (data >> index) & 0x01;
  }

  return tempParity;
}

//convert data to packet
uint16_t dataToPacket(uint8_t data)
{
  uint8_t parity = 0;
  uint16_t tempConv = 0;

  //start value is 0, tempConv set to 0, no need to do again.
  tempConv |= ((uint16_t)data) << DATA_BIT0_POS;

  parity = oddParityGen(data);

  tempConv |= ((uint16_t)parity) << PARITY_BIT_POS;

  tempConv |= ((uint16_t)STOP_BIT_VALUE) << STOP_BIT_POS;

  return tempConv;
}

//copy to send buffer
void copyPacketToBuffer(struct s_ps2 *p_ps2, uint16_t packet)
{
  p_ps2->buffer = packet;
}

//start transmit routine.
void startTransmit(struct s_ps2 *p_ps2)
{
  p_ps2->dataState = send;

  p_ps2->callbackState = waiting;

  p_ps2->recvCallback = p_ps2->responseCallback;

  //set clock pin low for at least 100ms
  *(p_ps2->p_port - 1) |= 1 << p_ps2->clkPin;

  *p_ps2->p_port &= ~(1 << p_ps2->clkPin);

  _delay_ms(110);

  //set data pin to output and set low
  *(p_ps2->p_port - 1) |= 1 << p_ps2->dataPin;

  *p_ps2->p_port &= ~(1 << p_ps2->dataPin);

  //release clock pin by setting to input
  *(p_ps2->p_port - 1) &= ~(1 << p_ps2->clkPin);

  p_ps2->index++;

  _delay_us(10);
}
