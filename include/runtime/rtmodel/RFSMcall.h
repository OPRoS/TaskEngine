/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMparam.h"
#include "runtime/rtmodel/RFSMstmt.h"
#include "runtime/rtmodel/RFSMnamedItem.h"
#include <string>

/**
��Ŭ������ ���� ��ӵǴ� Ŭ������ ȣ�⹮�� ���� Ŭ���� �����̴�.
(�̿� ���� RFSMbehavior�� call ���Ǻκ�)
*/
class RFSMcall : public RFSMstmt, public RFSMnamedItem
{
private:
	std::vector<RFSMparam*>*  m_params;

public:
	RFSMcall(std::string name, std::string className, std::string file, std::string blockpath, std::string blockname, int line);
	virtual ~RFSMcall(void);

	void		addParameter	(RFSMparam*);
	std::string toString		(void);
	std::vector<RFSMparam*>*	getParameters	(void);

};
const unsigned int CALL_TASK	= 0;
const unsigned int CALL_ACTION	= 1;
const unsigned int CALL_FUNC	= 2;