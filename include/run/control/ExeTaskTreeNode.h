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
	ValueMap		m_valueTAB;		//하나의 Behavior에서 사용되는 변수들 state_time, taks_time, 기타 블럭내변수들
									//블럭을 벗어나면? 벗어났다가 다시 들어오면: 갱신문제
	//ValueLineMap	m_valueLineTAB;
	//bool			m_loopSynched;  //부모노드가 알려주는 정보임(하위노드는 자신의 실행 여부를 알수없음)
	struct timeb	m_nodeStart;
	bool			m_isConstructed;
	bool			m_isDestructed;
	
	int				m_nodeNameLine; //정의된 파일의 라인
	int				m_callstmtLine;
	std::string		m_nodeName;
	std::string		m_fileName;
	std::string		m_blockID;
	std::string		m_nextTarget; //실행 실행 끝난 후 다음 target, (behavior내의 state)나 (break)문
	bool			m_completed;  //실행 
	
	ExeTaskTreeNode*				m_parent;
	std::vector<ExeTaskTreeNode*>	m_childNodes;
	MonitorResponse*				m_monRes;

	std::map<int, int>				m_WaitingSynchIDs; //현재 노드에서 대기 중인 synch id와 그수
	std::map<int, int>::iterator	m_mapIter;
	//std::set<int>					m_notSynchedIds;

	pthread_mutex_t					m_lmu2;

protected:
	//std::string	m_position;			//behavior를 호출하는 호출문 위치
	int				m_runType;
	bool			m_stop;				//외부로 부터 강제적 중지명령이 전달되는 경우 사용
	bool			m_suspend;			//모니터링에서 잠시 중지 명령이 들어오면
	bool			m_isMission;
	bool			m_joining;			//stopRunning이 join하는 중에 또 호출되는 것을 방지
	                                    //stopRunning은 behavior,connector이 실행 중 혹은 강제 종료에도 호출되기 때문에 중복 호출이 가능함
		
	
	bool			m_isEOI;			//end of iteration
	bool			m_isActBlockSelected;
	bool			m_isActBlockExecuted;
	bool			m_enterAfterConstBL;
	bool			m_isDbgCondCreated;

	ExeTask*		m_exeTask;
	LogFile*		m_logFile;
	RemoteCLI*		m_svr;				// 디버깅(모니터링) remote로 보내기 위해 필요

public:
	ExeTaskTreeNode(ExeTaskTreeNode*, RemoteCLI*, std::string, int, std::string, std::string, int, unsigned int);
	virtual ~ExeTaskTreeNode(void);

	void setExeTask				(ExeTask*);
	void addChild				(ExeTaskTreeNode*);
	void setParent				(ExeTaskTreeNode*);
	void removeAllChildNodes	(bool);
	void removeChildNode		(std::string, bool, int);//with block에 조건이 있을 수 있다.

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
	void currentWaitingIDs	(std::set<int>&);	//계속대기해야할 synch id를 알려줌
	
	ExeTaskTreeNode*	getConnectorParent	(void);
	unsigned int		getChildNum			(void);
	bool				isActBlockSelected	(void);
	bool				isActBlockExecuted	(void);
	void				setActBlockSelected	(bool);
	void				setActBlockExecuted	(bool);

	/*디버깅, 모니터링 API*/
	bool isStopped		(void); //디버깅중(대기상태) 외부에서 태스크 정지 명령을 보낼 경우
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