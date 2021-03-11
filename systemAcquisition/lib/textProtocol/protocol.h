/*
 * protocol.h
 *
 *  Created at:		11/03/2021 12:45:55
 *      Author:		Pedro Igor B. S.
 *	Email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 * -------------------------------------------------------------------
 *   Copyright (C) Pedro Igor B. S 2019
 * -------------------------------------------------------------------
 *
 *   Licença: GNU GPL 2
 * -------------------------------------------------------------------
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; version 2 of the
 *   License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * -------------------------------------------------------------------
 * ########################################################
 *
 * TODO: documentation or resume or Abstract
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* modificação */
#include <FreeRTOS/include/FreeRTOSConfig.h>

#ifndef protocol_H_
#define protocol_H_

/* macro ---------------------------------------------------------------------*/
#define TOKENS 		{',', ':', '=', ';', '&', '\n', '\r', '$', 0x00}

/* Definition ----------------------------------------------------------------*/

_bool bIsStringHere( CCHR *pcFindAt, CCHR *pcFindThis);
_bool bIsToken(CCHR pcTok);
_bool bIsHexNum(CCHR *pcStr);
_bool bIsDecNum(CCHR *pcStr);
_bool bGetLabelInfo(CCHR *pcStr, CCHR *pcLabel, const u16 cuArg , char *pcBff);
_bool bGetValueInCSV(CCHR *pcLine, const size_t cuWhere, char *pcSaveTo);
_bool bGetValueInStr(CCHR *pcStr, const size_t cuWhere, CCHR div, char *pcSaveTo);

u8 *puCleanBlk(u8 *puBlk, u16 uLen);

char *pcCleanStr(char *pcStr);
char *pcZeroLeft(char *pcStr, const size_t cuSizeWithZeros);
char *pcRmChr( char *pcStr, CCHR ccRemoveChar );
char *pcCharToChar(char *pcStr, CCHR ccFind, CCHR ccReplaceTo);
char *pcToUpperCase(char *pcStr);


#endif /* protocol_H_ */