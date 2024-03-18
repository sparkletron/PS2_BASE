/*******************************************************************************
 * @file    psbase.h
 * @author  Jay Convertino(electrobs@gmail.com)
 * @date    2024.03.18
 * @brief   ps2 base functions
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

#ifndef _ps2base
#define _ps2base

#include <inttypes.h>
#include "ps2DataType.h"

/**
 * \brief Convert PS2 keyboard define representation
 * to a character.
 *
 * \param ps2data PS2 keyboard data in a the form of a define from the scan code lookup table.
 *
 * \return Return a ASCII character
 */


//helper functions
//waits for callback to return, returns state of the callback for error
//sends commands
void sendCommand(struct s_ps2 *p_ps2, uint8_t command);
//sends data, (does not update LAST_CMD);
void sendData(struct s_ps2 *p_ps2, uint8_t data);
//sends commands that have no ack
void sendCommand_noack(struct s_ps2 *p_ps2, uint8_t command);
//sends data that has no ack, (does not update LAST_CMD);
void sendData_noack(struct s_ps2 *p_ps2, uint8_t data);
//handling (none implimented at this time).
enum callbackStates waitingForCallback(struct s_ps2 *p_ps2);
//wait for idle state in rx/tx irq
void waitForDataIdle(struct s_ps2 *p_ps2);
//wait for device ready AA, 00
void waitForDevReady(struct s_ps2 *p_ps2);
//wait for command ack, eventually add timeouts to these methods
void waitForCMDack(struct s_ps2 *p_ps2);
//wait for device id state from callback
void waitForDevID(struct s_ps2 *p_ps2);
//Converts PS2 data to raw data, this performs checks on the data.
//If it returns 0 then the data is invalid.
uint8_t convertToRaw(uint16_t ps2data);
//generate odd parity
uint8_t oddParityGen(uint16_t data);
//convert 8 bit data into a 11 bit packet
uint16_t dataToPacket(uint8_t data);
//copys data passed to it to the internal send buffer.
void copyPacketToBuffer(struct s_ps2 *p_ps2, uint16_t packet);
//start trasmission of data to the keyboard.
void startTransmit(struct s_ps2 *p_ps2);

#endif
