#pragma once

#include <string>
#include <time.h>
#include <vector>
#include "run/control/ExeTask.h"
#include "run/control/ExeTaskTreeNode.h"
#include "runtime/rtmodel/RFSMnamedItem.h"
#include "common/TypeDef.h"
#include "thread/RThread.h"
#include "ds/EList.h"
#include "log/LogFile.h"

class ExeConnector: public ExeTaskTreeNode
{
private:	
	int							m_conType;
	int							m_joinType;		
	unsigned int				m_seq;	//Sequexer �϶��� ���
	ValueMap					m_params;
	std::vector<ExeTransition*> m_withList;
	
	std::map<int, int>*			m_SynchIdMap; //�Ľ� �ܰ迡�� ����� ��������� synch id�� �� ��(RFSMconnector���� ���´�)
	
public:
	ExeConnector(RemoteCLI*, LogFile*, ExeTaskTreeNode*, std::string name, int nameLine, std::string blockID, std::string filename, std::map<int, int>*, int, unsigned int);
	virtual ~ExeConnector	(void);

	int		execute		(std::vector<ExeTaskTreeNode*>, int conType);
	void	setWithList	(std::vector<ExeTransition*> wlist);
	//void	removeWith	(unsigned int index);
	void	setConType	(int);
	void	setJoinType	(int);
	void	setCurActiveSeq(unsigned int);

	int getConType		(void);
	int getJoinType		(void);
	int getSynchIDNum	(int id);
	unsigned int		getCurActiveSeq(void);
	unsigned int		getNumOfRunBlocks(void);
	std::vector<ExeTransition*> getWithList		(void);
	//std::map<int, int>*		getSynchInfoMap	(void); 
		
	virtual void cbackBy_Thrfunc	(void);
};