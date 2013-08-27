#pragma once

#include <string>
#include "runtime/rtmodel/RFSMnamedItem.h"
#include "runtime/rtmodel/RFSMstmt.h"
#include "runtime/rtmodel/RFSMtreenode.h"
#include "runtime/rtmodel/RFSMStmtBlock.h"

class RFSMconnector : public RFSMtreenode
{
private:
	int	m_joinType; 
	int	m_conType; //1.conexer 2.seqexer
	int	m_runType; //1.synchro 2.asynchro 3.nothread

	RFSMStmtBlock*		m_withsBlock;
	//std::vector<std::string>*	m_withIDs;

	std::map<int, int>	m_SynchIdMap; 	//(id, num) num이 2개 이상(개수는 id가 포함된 path의수)이면 유효하다
	std::map<int, int>::iterator m_mapIter;

public:
	RFSMconnector(int conType, std::string name, int nameLine, int endline, std::string blockID, std::string file, bool isMission);
	virtual ~RFSMconnector(void);

	void setRunBlocks	(RFSMStmtBlock*);
	void setJoinType	(int);
	void setRunType		(int);
	void setSynchInfo	(std::map<int, int>);

	int	getJoinType	(void);
	int	getConType	(void);
	int	getRunType	(void);

	//std::vector<std::string>*	getWithIDs		(void);
	RFSMStmtBlock*			getRunBlocks	(void);
	std::map<int, int>*		getSynchIdMap	(void);
};
