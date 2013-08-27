/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#ifndef __GEXCEPTION_H__
#define __GEXCEPTION_H__

#include <antlr3.h>
#include "util/MultiOS.h"

void produceError(pANTLR3_BASE_RECOGNIZER, pANTLR3_UINT8*);
void produceError4Lexer(pANTLR3_BASE_RECOGNIZER, pANTLR3_UINT8*);
void produceError4Parser(pANTLR3_BASE_RECOGNIZER, pANTLR3_UINT8*);

#endif