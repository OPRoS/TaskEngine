#pragma once

#include "thread/RThread.h"
#include "common/TypeDef.h"
#include "util/MultiOS.h"
#include "runtime/rtmodel/RFSMvalueEval.h"
#include "run/control/TaskRunner.h"
#include "run/control/ExeTask.h"
#include "comm/MonitorResponse.h"
#include "log/LogFile.h"
#include <vector>
#include <set>
#include <sys/timeb.h>
#include <time.h>

class ExeTask;
class RemoteCLI;

class ExeTaskTreeNode : public RThread
{
private:
	ValueMap		m_valueTAB;		//�ϳ��� Behavior���� ���Ǵ� ������ state_time, taks_time, ��Ÿ ����������
									//���� �����? ����ٰ� �ٽ� ������: ���Ź���
	//ValueLineMap	m_valueLineTAB;
	//bool			m_loopSynched;  //�θ��尡 �˷��ִ� ������(�������� �ڽ��� ���� ���θ� �˼�����)
	struct timeb	m_nodeStart;
	bool			m_isConstructed;
	bool			m_isDestructed;
	
	int				m_nodeNameLine; //���ǵ� ������ ����
	int				m_callstmtLine;
	std::string		m_nodeName;
	std::string		m_fileName;
	std::string		m_blockID;
	std::string		m_nextTarget; //���� ���� ���� �� ���� target, (behavior���� state)�� (break)��
	bool			m_completed;  //���� 
	
	ExeTaskTreeNode*				m_parent;
	std::vector<ExeTaskTreeNode*>	m_childNodes;
	MonitorResponse*				m_monRes;

	std::map<int, int>				m_WaitingSynchIDs; //���� ��忡�� ��� ���� synch id�� �׼�
	std::map<int, int>::iterator	m_mapIter;
	//std::set<int>					m_notSynchedIds;

	pthread_mutex_t					m_lmu2;

protected:
	//std::string	m_position;			//behavior�� ȣ���ϴ� ȣ�⹮ ��ġ
	int				m_runType;
	bool			m_stop;				//�ܺη� ���� ������ ��������� ���޵Ǵ� ��� ���
	bool			m_suspend;			//����͸����� ��� ���� ����� ������
	bool			m_isMission;
	bool			m_joining;			//stopRunning�� join�ϴ� �߿� �� ȣ��Ǵ� ���� ����
	                                    //stopRunning�� behavior,connector�� ���� �� Ȥ�� ���� ���ῡ�� ȣ��Ǳ� ������ �ߺ� ȣ���� ������
		
	
	bool			m_isEOI;			//end of iteration
	bool			m_isActBlockSelected;
	bool			m_isActBlockExecuted;
	bool			m_enterAfterConstBL;
	bool			m_isDbgCondCreated;

	ExeTask*		m_exeTask;
	LogFile*		m_logFile;
	RemoteCLI*		m_svr;				// �����(����͸�) remote�� ������ ���� �ʿ�

public:
	ExeTaskTreeNode(ExeTaskTreeNode*, RemoteCLI*, std::string, int, std::string, std::string, int, unsigned int);
	virtual ~ExeTaskTreeNode(void);

	void setExeTask				(ExeTask*);
	void addChild				(ExeTaskTreeNode*);
	void setParent				(ExeTaskTreeNode*);
	void removeAllChildNodes	(bool);
	void removeChildNode		(std::string, bool, int);//with block�� ������ ���� �� �ִ�.

	int			getNodeNameLine	(void);
	int			getCallStmtLine	(void);
	std::string getNodeName		(void);
	std::string getFileName		(void);
	//std::string getPosition();
	std::string	getBlockID		(void);
	std::string	getNextTarget	(void);


	void enterAfterConstBL		(bool);
	bool isRunnedAfterConstBL	(void);
	bool isChildExist			(std::string, int);
	void setNextTarget			(std::string);

	ExeTaskTreeNode*				getChildNode		(std::string name, int);
	std::vector<ExeTaskTreeNode*>	getAllChildNodes	(void);
	std::vector<ExeTaskTreeNode*>*	getAllChildNodesP	(void);
	
	void			addLocalValue(std::string varName, RFSMvalueEval);
	RFSMvalueEval	getLocalValue(std::string);
	
	void setConstructed	(void);		
	void setDestructed	(void);
	bool isConstructed	(void);
	bool isDestructed	(void);
	void setAsMission	(void);
	bool isMission		(void);
	bool isJoining		(void);

	//BEHAVIOR_TIME
	void			nodeStart		(void);
	RFSMvalueEval	getNodeExeTime	(int);
	
	int		getRunType	(void);
	void	setRunType	(int);
	
	virtual bool	isCompleted	(void); 
	virtual void	setCompleted(bool);
	virtual int		stopRunning	(void);
	void			setStopFlag	(void);

	//bool isSynched4Loop();
	//void setSynched4Loop();
	//void setUnSynched4Loop();
	//bool isEOI();
	//void setEOI(bool);

	void removeWaitingID	(int synchid);
	void notifyWaitingID	(int synchid);
	void currentWaitingIDs	(std::set<int>&);	//��Ӵ���ؾ��� synch id�� �˷���
	
	ExeTaskTreeNode*	getConnectorParent	(void);
	unsigned int		getChildNum			(void);
	bool				isActBlockSelected	(void);
	bool				isActBlockExecuted	(void);
	void				setActBlockSelected	(bool);
	void				setActBlockExecuted	(bool);

	/*�����, ����͸� API*/
	bool isStopped		(void); //�������(������) �ܺο��� �½�ũ ���� ����� ���� ���
	void sendDebugMsg	(ValueMap*, std::string, int line, std::string blockpath);
	void sendMonitorMsg	(std::string msg);
	void sendThreadInfo	(std::string cmd);

	std::string			makeDebugMsg(std::string taskname, std::string file, int dbgLine, std::string name, RFSMvalueEval);
	std::string			makeEmptyDebugMsg(std::string taskname, std::string file, int dbgLine);
	RemoteCLI*			getRemoteCLI		(void);
	ExeTaskTreeNode*	getParent			(void);
	int					getParentThreadID	(void);
	int					getCreatedThreadID	(void);
	ValueMap*			getVariableTAB		(void);		
	MonitorResponse*	getMonitorRes		(void);
};

//static const std::string BEHAVIOR_TIME	= std::string("behavior_time");