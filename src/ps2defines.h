/*******************************************************************************
 * @file    ps2defines.h
 * @author  Jay Convertino(electrobs@gmail.com)
 * @date    2024.03.12
 * @brief   defines for PS2
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


#ifndef PS2_DEFINES
#define PS2_DEFINES

typedef void (*t_PS2recvCallback)(uint16_t recvBuffer);
typedef void (*t_PS2userRecvCallback)(uint8_t recvBuffer);
enum ackStates {ack, noack};
enum dataStates {send, recv, ck_ack, idle};
enum callbackStates {waiting, no_cmd, resend_cmd, ack_cmd, ready_cmd, dev_id};

#define MAX_NUMBER_OF_DEVS 4

//bit stuff
#define PARITY_BIT_POS  9
#define DATA_BIT0_POS   1
#define STOP_BIT_POS    10
#define STOP_BIT_VALUE  1

//common commands/responses
#define CMD_DEV_RDY       0xAA
#define CMD_RESET         0xFF
#define CMD_ACK           0xFA
#define CMD_RESEND        0xFE
#define CMD_DEFAULT       0xF6
#define CMD_DISABLE       0xF5
#define CMD_ENABLE        0xF4
#define CMD_SET_RATE      0xF3
#define CMD_READ_ID       0xF2

#define MESSAGE_LENGTH 10

#endif
