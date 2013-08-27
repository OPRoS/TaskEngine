/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/

#include "comm/RemoteCLI.h"
#include "run/TaskManager.h"
#include "runtime/RTObjBuilder.h"

TaskManager::TaskManager(ModelBinder* tmPlugin, RemoteCLI* svr, std::ostream* ostr)
{
	m_tmPlugin	= tmPlugin;
	m_ostr		= ostr;
	m_remoteCLI = svr;
	m_taskMem	= new TaskMem(ostr);

	m_log = Log("TASK-LOADING", ostr);
	m_log.setLevel(Log::LOG_LEVEL_ERROR, true);
	m_log.setLevel(Log::LOG_LEVEL_INFO, true);
	m_log.setLevel(Log::LOG_LEVEL_TRACE, true);
	m_log.setLevel(Log::LOG_LEVEL_DEBUG, true);

	pthread_mutex_init(&m_mu, NULL);
}

TaskManager:: ~TaskManager(void)
{
	if(m_taskMem != NULL) {
		delete m_taskMem;
		m_taskMem = NULL;
	}

	if(m_workexeTAB.size()>0){
		std::map<std::string, ExeTaskRoot*>::iterator it;
		for(it=m_workexeTAB.begin() ; it!=m_workexeTAB.end() ; it++){
			delete it->second; 
		}
	}
	m_workexeTAB.clear();
}

int TaskManager::loadWorker(std::string workerFileName, OSFileSys* fsys)
{
	int result = 0;
	int errorCount = 0;

	std::auto_ptr<EParser> aTest(new EParser(m_ostr));
	//EParser* aTest = new EParser(m_ostr);

	aTest->setFilename(workerFileName);
	int res = aTest->extractInclude(workerFileName, TaskConfig::getDeployHome());
	if(res != 0){
		result = res;
		errorCount = res;
	}

	if(aTest->getInclude() != 0){
		//preprocessing include
		EList<std::string> includedFiles = aTest->getInclude()->getIncludeFiles();
		if(includedFiles.moveHead()){
			do{
				std::string fName = includedFiles.getCurObject();
				//parsing
				LOG_TRACE(m_log, "Reading the file, '%s'...", fName.c_str());
				aTest->setFilename(fName);
				errorCount = aTest->parseScript(TaskConfig::getDeployHome() + fName);
				if(errorCount>0){					
					result = errorCount; ///////////////////////////////////에러코드정리
					LOG_ERROR(m_log, "Parser can not open the file '%s'.", fName.c_str());
					break;
				}
			}
			while(includedFiles.moveNext());
		}
	}

	if(errorCount == 0){
		GparsedResult* pRslt = aTest->getParsingResult();

		EList<Genum*>*		enumlist	= pRslt->getEnums();
		EList<Gmodel*>*		modellist	= pRslt->getModels();
		EList<Gaction*>*	actionlist	= pRslt->getActions();
//		EList<Gworker*>*	workerlist	= pRslt->getWorkers();     //예전 방법 task 정의
		EList<Gtask*>*		tasklist	= pRslt->getTasks();       //최근 방법 task 정의
		EList<Gbehavior*>*	bhvlist		= pRslt->getbehaviors();
		EList<Gconnector*>* connlist	= pRslt->getconnectors();

		
		
		SymTAB4Task* newSymT = new SymTAB4Task();
		std::auto_ptr<RTObjBuilder> rtmBuilder(new RTObjBuilder(m_ostr, newSymT));
		//RTObjBuilder *rtmBuilder = new RTObjBuilder(m_ostr, newSymT);

		EList<RFSMtask*>*		rtasks	= NULL;
		EList<RFSMbehavior*>*	rbhvs	= NULL;
		EList<RFSMconnector*>*	rconns	= NULL;

		try{
			//model, action, enum 검사
			LOG_TRACE(m_log, "Checking Model composed of Symbol, Function, Action and Enum ...");
			rtmBuilder->makeSymTable(pRslt);

			//TASK 명세 검사
			LOG_TRACE(m_log, "Checking Tasks ...");
			rtasks	= rtmBuilder->checkSemantics(tasklist);
			rbhvs	= rtmBuilder->checkSemantics(bhvlist);
			rconns	= rtmBuilder->checkSemantics(connlist);

			//Model 파일수가 여러개 이더라도 1개의 심볼테이블이 만들어진다.
			//파일이 나누어져도 하나의 파일로 간주
			//SymTAB4Task* newSymT = rtmBuilder->getSymT();

	/*		
			if(workerlist !=NULL && workerlist->getSize()>0 && workerlist->moveHead()){
				do{
					Gworker* worker = workerlist->getCurObject();
					std::string workerName = worker->getName()->getData();

					//Task 들을 등록
					result = m_taskMem->addTask(workerName, worker->getStartBhv()->getData());

					//심볼테이블 등록
					if(result ==0){
						newSymT->pluRefCount();
						result = m_taskMem->addSymbolT(workerName, newSymT);
					}
					else
						result = 100; ///////////////////////////////////에러코드정리
				}
				while(workerlist->moveNext());
			}
*/
			if(rtasks !=NULL && rtasks->getSize()>0 && rtasks->moveHead()){
				do{
					RFSMtask* task = rtasks->getCurObject();
					std::string taskName = task->getName();

					//Task 들을 등록
					result = m_taskMem->addTask2(taskName, task);
					
//여기까지 했다 다음부터 구현 할 것

					//심볼테이블 등록
					if(result ==0){
						newSymT->pluRefCount();
						result = m_taskMem->addSymbolT(taskName, newSymT);
					}
					else
						result = 100; ///////////////////////////////////에러코드정리
				}
				while(rtasks->moveNext());
			}
	//		else
	//			delete newSymT; //task가 없으면 나중에 지울 수가 없기 때문에...

			//Behavior 등록
			if(result ==0){
				if(rbhvs != NULL && rbhvs->getSize()>0 && rbhvs->moveHead()){
					do{
						RFSMbehavior* bhv = rbhvs->getCurObject();
						//아래 순서 중요.. 모니터링에서 behavior테이블로 중복을 검사하기 때문에
						//1. 모니터링 정보 등록
						m_taskMem->addMonitorInfo(bhv);
						//2. behavior등록
						result = m_taskMem->addBehavior(bhv);						
					}
					while(rbhvs->moveNext());
				}

				if(rconns != NULL && rconns->getSize()>0 && rconns->moveHead()){
					do{
						RFSMconnector*  rconn= rconns->getCurObject();
						//아래 순서 중요.. 모니터링에서 behavior테이블로 중복을 검사하기 때문에
						//1. 모니터링 정보 등록
						//m_taskMem->addMonitorInfo(rconn);
						//2. behavior등록
						result = m_taskMem->addConnector(rconn);						
					}
					while(rconns->moveNext());
				}
			}
			else
				result = 100; ///////////////////////////////////에러코드정리
/*
			//bhv에 관련된 worker이름을 저장
			if(result ==0){
				if(workerlist !=NULL && workerlist->getSize()>0 && workerlist->moveHead()){
					do{
						Gworker* worker = workerlist->getCurObject();
						std::string workerName = worker->getName()->getData();
						std::string startBhvName = worker->getStartBhv()->getData();

						RFSMbehavior* t = m_taskMem->findBehavior(startBhvName);
						if(t == NULL){//task의 시작 behavior가 존재하지 않는 경우							
							LOG_ERROR(m_log, "The root behavior, <%s()>, cannot be found.", startBhvName.c_str());
							return 100;////////////////////////에러코드정리							
						}
						else{
							t->addRefTask(workerName);
							EList<std::string> tlist;
							setWorkerName(t, workerName, tlist);

							//각각의 worker에 대해서 dot파일 생성
						  
							std::string dot = getDot(workerName, t, tlist);
							std::ofstream dotfile;
							std::string dotfilename = workerName;
							dotfilename.append(".dot");
							dotfile.open(dotfilename.c_str());
							dotfile << dot.c_str() << "\n";
							dotfile.flush();
							dotfile.close();

							LOG_TRACE(m_log, "%s.dot file, which represents graphical view of a worker, is created.", workerName.c_str());
					    }
						
					}
					while(workerlist->moveNext());
				}
			}
*/

			//bhv에 관련된 worker이름을 저장
			if(result ==0){
				if(rtasks !=NULL && rtasks->getSize()>0 && rtasks->moveHead()){
					do{						
						RFSMtask* task = rtasks->getCurObject();
						std::string taskName = task->getName();

						//task에 포함된 모든 behavior와 conexer의 이름
						std::vector<RFSMstmt*>* runBlock = task->getRunBlock();
						std::vector<std::string> startList;
						task->extractStartNodes(runBlock, startList);

						//runblock에 포함된 모든 behavior나 conexer에 대해서
						std::vector<std::string>::iterator it;
						for(it=startList.begin(); it<startList.end(); it++){
							std::string startNodeName = *it;
							RFSMtreenode* startNode = m_taskMem->findBehavior(startNodeName);
							if(startNode == NULL){//task의 시작 behavior가 존재하지 않는 경우
								startNode = m_taskMem->findConnector(startNodeName);
								if(startNode == NULL){
									LOG_ERROR(m_log, "The root node(behavior or conexer), <%s()>, cannot be found.", startNodeName.c_str());
									result = 100;////////////////////////에러코드정리
									break;
								}
							}
							
							startNode->addRefTask(taskName);
							EList<std::string> tlist; // for dot
							setWorkerName(startNode, taskName, tlist);
							tlist.clearAllchild(); //이걸해야 tlist의 new EListElement<T>(obj); 으로 만들어진 부분이 삭제된다.
						}

						//각각의 worker에 대해서 dot파일 생성
						
						
						// 아래 부분은 task노드를 root로 하는 구조로 바뀌어야 한다.

						/*
						std::string dot = getDot(taskName, t, tlist);
						std::ofstream dotfile;
						std::string dotfilename = taskName;
						dotfilename.append(".dot");
						dotfile.open(dotfilename.c_str());
						dotfile << dot.c_str() << "\n";
						dotfile.flush();
						dotfile.close();

						LOG_TRACE(m_log, "%s.dot file, which represents graphical view of a worker, is created.", taskName.c_str());
					    
						*/
					}
					while(result == 0 && rtasks->moveNext());
				}
			}

			if(rtasks != 0 && rtasks->getSize()>0 && rtasks->moveHead()){
				do{
					RFSMtreenode* rtask = rtasks->getCurObject();
					setSubNodes(rtask);
				}while(rtasks->moveNext());
			}

			if(rbhvs != 0 && rbhvs->getSize()>0 && rbhvs->moveHead()){
				do{
					RFSMtreenode* rbhv = rbhvs->getCurObject();
					setSubNodes(rbhv);
				}while(rbhvs->moveNext());
			}

			if(rconns != 0 && rconns->getSize()>0 && rconns->moveHead()){
				do{
					RFSMtreenode* rconn = rconns->getCurObject();
					setSubNodes(rconn);
				}while(rconns->moveNext());
			}

			if(rconns != 0 && rconns->getSize()>0 && rconns->moveHead()){
				do{
					RFSMconnector* rconn = (RFSMconnector*)rconns->getCurObject();
					std::map<int, int>	synchInfo = checkValidSynchIDs(rconn);
					rconn->setSynchInfo(synchInfo);
					//rconn->checkValidSynchIDs();
				}while(rconns->moveNext());
			}

			if(rtasks != 0 && rtasks->getSize()>0 && rtasks->moveHead()){
				do{
					std::set<int> ids;
					RFSMtask* rtask = (RFSMtask*)(rtasks->getCurObject());
					rtask->setSynchidInTask();
				}while(rtasks->moveNext());
			}
			

			
			/* 삭제(껍데기만 내용은 TaskMem에 저장) */
			if(rtasks != NULL){
				while(rtasks->getSize()>0 && rtasks->moveHead()){
					//RFSMtask* obj = rtasks->getCurObject();
					//delete obj;
					rtasks->delHead();
				}
				delete rtasks; rtasks = NULL;
			}
			if(rbhvs != NULL){
				while(rbhvs->getSize()>0 && rbhvs->moveHead()){
					//RFSMbehavior* obj = rbhvs->getCurObject();
					//delete obj;
					rbhvs->delHead();
				}
				delete rbhvs; rbhvs = NULL;
			}
			if(rconns != NULL){
				while(rconns->getSize()>0 && rconns->moveHead()){
					//RFSMconnector* obj = rconns->getCurObject();
					//delete obj;
					rconns->delHead();
				}
				delete rconns; rconns = NULL;
			}		
/*			
			//시나리오 파일 처리
			if(pRslt->getSCs().size()>0){
				std::string inclstr;
				if(aTest->getInclude() != 0){
					Ginclude* inclList = aTest->getInclude();
					if(inclList->moveHead()){
						do{
							Gtoken* tok = inclList->getCurObject();
							std::string fname = tok->getFileName();
							std::string incl = tok->getData();
							if(fname==workerFileName)
								inclstr.append("#include \"").append(incl).append("\"\n");
						}
						while(inclList->moveNext());
					}
				}
				TPLGenerator tplGen(fsys, workerFileName, inclstr, pRslt->getSTasks(), pRslt->getSCs(), pRslt->getSBehaviors(), pRslt->getSEvents());
				tplGen.generateTPL();
			}
*/
		}
		catch(SemanticsEH& semEH){
			LOG_ERROR(m_log, "The parser returned %d errors, Task loading aborted.", semEH.hasError_());
			delete newSymT; newSymT = NULL;
			delete rtasks; rtasks = NULL;
			delete rbhvs; rbhvs = NULL;
			delete rconns; rconns = NULL;
			result = 100;/////////////////////////////////////////////////////에러코드 정리
		}

	}

	
	//aTest 에서 pRslt가 지워진다.
	//delete aTest;

	return result;
}

std::string TaskManager::getDot(std::string workerName, RFSMtreenode* startNode, EList<std::string> tlist)
{
	RFSMbehavior* startTask = (RFSMbehavior*)startNode;
	if(typeid(*startNode)==typeid(RFSMbehavior)){

	}
	else if(typeid(*startNode)==typeid(RFSMconnector)){
		return "";
	}
	else{
		return "";
	}

	std::string dot;
	dot.append("digraph G{\n");
	dot.append("compound=true;\n");

	// 1.Pre declaration
	if(tlist.getSize()>0 && tlist.moveHead()){
		do{
			dot.append("subgraph cluster");//RFSMtask에서 이름앞에 'cluster'를 사용
			dot.append(tlist.getCurObject());
			dot.append(";\n");
		}
		while(tlist.moveNext());
	}

	// 2. worker의 시작 bhv
	dot.append(startTask->getDot());


	// 3. worker의 시작 Task에서 분기되는 Task들
	std::vector<std::string> linkedTasks = startTask->getLinkedNodes();
	if(linkedTasks.size()>0){
		for(unsigned int i =0; i<linkedTasks.size();i++){
			std::string tName = linkedTasks[i];
			RFSMbehavior* t = m_taskMem->findBehavior(tName);
			if(t != NULL){
				std::string taskDot = t->getDot();
				dot.append(taskDot);
			}

		}
	}

	// 4. %taskname%을 initial state 명으로 바꾼다.
	while(true){
		int s = dot.find_first_of("%");
		int e = dot.find_first_of("*");
		if(s<0 || e<0)break;

		std::string sub = dot.substr(s, e-s+1);//시작, 시작으로부터offset
		std::string stName = dot.substr(s+1, e-s-1);
		RFSMbehavior* tt = m_taskMem->findBehavior(stName);
		if(tt != NULL && tt->getInitialState() != NULL){
			std::string isName = tt->getInitialState()->getName();
			Estring::replaceAll(dot, sub, RFSMnamedItem::removePath(isName));
		}
		else{
			//무한루프를 빠져나오기 위해서 bhv가 없더라도 반드시 %,*가 없는 이름으로 변환
			Estring::replaceAll(dot, sub, stName);
		}
	}

	dot.append("}\n");

	return dot;
}

void TaskManager::setSubNodes(RFSMtreenode* node)
{
	std::vector<std::string> subnames = node->getLinkedNodes();
	if(subnames.size()>0){
		for(unsigned int i =0; i<subnames.size();i++){
			std::string subname = subnames[i];
			RFSMtreenode* subnode = m_taskMem->findBehavior(subname);
			if(subnode == NULL) subnode = m_taskMem->findConnector(subname);
			if(subnode != NULL) node->addSubNode(subname, subnode);
			
		}
	}
}

std::map<int, int> TaskManager::checkValidSynchIDs(RFSMconnector* rconn)
{
	std::map<std::string, RFSMtreenode*> subNodes = rconn->getSubNodes();
	std::map<std::string, RFSMtreenode*>::iterator node_iter;
	
	std::map<int, int> synchIdMap;
	std::map<int, int>::iterator mapIter;

	//2. synch id map 생성
	std::set<int> ids;
	if(subNodes.size()>0){		
		for(node_iter=subNodes.begin() ; node_iter!=subNodes.end() ; node_iter++){
			RFSMtreenode* node = node_iter->second;
			
			//각각의 sub node에 대해서 sych id를 얻어와서 temp에 저장
			node->collectSynIDs(ids); 
			std::set<int>::iterator ids_iter;
			for(ids_iter=ids.begin();ids_iter!=ids.end();ids_iter++){
				mapIter = synchIdMap.find(*ids_iter);
				if(mapIter != synchIdMap.end()){//map에 이미 존재하면
					int count = mapIter->second;
					mapIter->second = ++count;
				}
				else{//map에 존재하지 않으면
					synchIdMap.insert(std::pair<int, int>(*ids_iter, 1));
				}
			}
			ids.clear();
		}
	}

	//유효성 검사를 하는 것이 아니라 글로벌하게 std::map<int, int> 를 유지하면서 
	//SYNCH문을 만나면 개수를 카운드 하면된다.??? 안된다. 된다.다만 계층적으로 2개 이상은 1개로 카운드
	//동시는 더하기


	//3.유효한 synch id를 리턴
	std::set<int> result;
	for(mapIter=synchIdMap.begin() ; mapIter!=synchIdMap.end() ; mapIter++){
		if(mapIter->second>=2){
			result.insert(mapIter->first);
		}
	}
	
	//4. 하위 노드에 유효한 id를 전달
	for(node_iter=subNodes.begin() ; node_iter!=subNodes.end() ; node_iter++){
		RFSMtreenode* node = node_iter->second;
		node->setValidSynchIDs(result);
	}

	return synchIdMap;
}

void TaskManager::setWorkerName(RFSMtreenode* startnode, std::string workName, EList<std::string>& tlist)
{
	std::vector<std::string> linkedTasks = startnode->getLinkedNodes();
	if(linkedTasks.size()>0){
		for(unsigned int i =0; i<linkedTasks.size();i++){
			std::string tName = linkedTasks[i];
			RFSMtreenode* t = m_taskMem->findBehavior(tName);
			if(t == NULL){
				t = m_taskMem->findConnector(tName);
			}

			if(t != NULL && !t->isReferred(workName)){	
				tlist.addTail(t->getName());
				t->addRefTask(workName);
				setWorkerName(t, workName, tlist);
			}
		}
	}
}

int TaskManager::unloadWorker(std::string workerName)
{
	//먼저 실행 중이 task는 모두 삭제부터 한다.
	stopAllWorker();

	//worker목록 제거
	/*
	StrMap*	taskTAB = m_taskMem->getTaskTAB();
	StrMap::iterator it1 = taskTAB->find(workerName);
	if(it1 != taskTAB->end()){
		taskTAB->erase(it1);
	}
	else{
		TaskTAB* taskTAB2 = m_taskMem->getTaskTAB2();
		TaskTAB::iterator it2 = taskTAB2->find(workerName);
		if(it2 != taskTAB2->end()){
			delete it2->second;
			taskTAB2->erase(it2);
		}
		else{
			LOG_ERROR(m_log, "Task <%s> dose not exist in the loaded task list.", workerName.c_str());
			return DB_UNLOAD_ERR_NO_WORKER;
		}
	}
	*/

	TaskTAB* taskTAB2 = m_taskMem->getTaskTAB2();
	TaskTAB::iterator it2 = taskTAB2->find(workerName);
	if(it2 != taskTAB2->end()){
		delete it2->second;
		taskTAB2->erase(it2);
	}
	else{
		LOG_ERROR(m_log, "Task <%s> dose not exist in the loaded task list.", workerName.c_str());
		return DB_UNLOAD_ERR_NO_WORKER;
	}

	//worker에서 이용하는 model(심볼테이블) 삭제
	SymTAB* symTAB = m_taskMem->getSymbolTAB();
	SymTAB::iterator it = m_taskMem->getSymbolTAB()->find(workerName);
	if(it != symTAB->end()){
		SymTAB4Task* symT = it->second;
		if(symT->getWorkerRefCount() <= 1){
			delete symT;
		}
		else{
			symT->minRefCount();
		}
		symTAB->erase(it);
	}
	else{
		LOG_ERROR(m_log, "The symbol table for task <%s> dose not exist", workerName.c_str());
		return DB_UNLOAD_ERR_NO_SYMTAB;
	}

	//worker에 포함된 bhv 찾기
	EList<std::string> delList;
	BehaviorTAB* behaviorTAB = m_taskMem->getBehaviorTAB();
	BehaviorTAB::iterator it3 = behaviorTAB->begin();
	for(; it3!=behaviorTAB->end(); it3++){
		RFSMbehavior* t = it3->second;
		int wNum = t->unRefTask(workerName);
		if(wNum == 0){//bhv를 이용하는 worker가 하나도 없으면
			//unloadTask(it3->first); //map을 검색하면서 자신을 지우면 안된다.
			delList.addTail(it3->first);
		}
	}
/*
	if(delList.moveHead()){
		do{
			std::string tName = delList.getCurObject();
			unloadBehavior(tName);			
		}
		while(delList.moveNext());
	}
*/
	while(delList.moveHead()){		
		std::string tName = delList.getCurObject();
		unloadBehavior(tName);			
		delList.delHead();
	}

	//m_taskMem->leakTest();

	//conexer삭제
	EList<std::string> delConList;
	ConnectorTAB* conTAB = m_taskMem->getConnectorTAB();
	ConnectorTAB::iterator it4 = conTAB->begin();
	for(; it4!=conTAB->end(); it4++){
		RFSMconnector* con = it4->second;
		int wNum = con->unRefTask(workerName);
		if(wNum == 0){//bhv를 이용하는 worker가 하나도 없으면
			//unloadTask(it3->first); //map을 검색하면서 자신을 지우면 안된다.
			delConList.addTail(it4->first);
		}
	}
	while(delConList.getSize()>0 && delConList.moveHead()){		
		std::string cName = delConList.getCurObject();
		unloadConnector(cName);			
		delConList.delHead();
	}

	//monitor리스트 삭제

	return 0;
}

int TaskManager::unloadBehavior(std::string behaviorName)
{
	//삭제 대상 태스크를 참조하는 카운드 계산
	/*int taskRefcount = 0;
	std::map<std::string, EList<std::string>>::iterator it1;
	for(m_taskSetInWorker.begin(); it1!=m_taskSetInWorker.end(); it1++){
		EList<std::string> taskList = it1->second;
		if(taskList.getSize()>0 && taskList.moveHead()){
			do{
				std::string bhv = taskList.getCurObject();
				if(bhv.compare(taskName)==0)
					taskRefcount ++;
			}
			while(taskList.moveNext());
		}
	}

	if(taskRefcount>1){
		LOG_INFO(m_log, "As it used in other Worker(s), bhv %s is not unloaded.", taskName);
		return 0;
	}*/

	BehaviorTAB* behaviorTAB = m_taskMem->getBehaviorTAB();
	BehaviorTAB::iterator it2 = behaviorTAB->find(behaviorName);

	if(it2 != behaviorTAB->end()){
		RFSMbehavior* bhv = it2->second;
		delete bhv;
		behaviorTAB->erase(it2);
	}
	else{
		LOG_ERROR(m_log, "Behavior <%s> dose not exist.", behaviorName.c_str());
		return DB_UNLOAD_ERR_NO_TASK;
	}
	return 0;
}

int TaskManager::unloadConnector(std::string cName)
{
	//삭제 대상 태스크를 참조하는 카운드 계산
	/*int taskRefcount = 0;
	std::map<std::string, EList<std::string>>::iterator it1;
	for(m_taskSetInWorker.begin(); it1!=m_taskSetInWorker.end(); it1++){
		EList<std::string> taskList = it1->second;
		if(taskList.getSize()>0 && taskList.moveHead()){
			do{
				std::string bhv = taskList.getCurObject();
				if(bhv.compare(taskName)==0)
					taskRefcount ++;
			}
			while(taskList.moveNext());
		}
	}

	if(taskRefcount>1){
		LOG_INFO(m_log, "As it used in other Worker(s), bhv %s is not unloaded.", taskName);
		return 0;
	}*/

	ConnectorTAB* conTAB = m_taskMem->getConnectorTAB();
	ConnectorTAB::iterator it2 = conTAB->find(cName);

	if(it2 != conTAB->end()){
		RFSMconnector* con = it2->second;
		delete con;
		conTAB->erase(it2);
	}
	else{
		LOG_ERROR(m_log, "Conexer <%s> dose not exist.", cName.c_str());
		return DB_UNLOAD_ERR_NO_TASK;
	}
	return 0;
}

int TaskManager::startTask(std::string tname, int exeMode)
{
	RFSMtask* task = m_taskMem->getTask2(tname);
	
	std::map<std::string, ExeTaskRoot*>::iterator it = m_workexeTAB.find(tname);
	if(it != m_workexeTAB.end()){
		//같은 이름의 태스크가 이미 실행
		LOG_INFO(m_log, "Task <%s> is already running.", tname.c_str());
		return DB_EXE_WORKER_ALREADY_EXIST_ERR;       
	}
	
	if(task != NULL){
		LOG_TRACE(m_log, "Task <%s> is newly starting...", tname.c_str());
		int parentid = 0;
		ExeTask* extTask = new ExeTask(m_remoteCLI, NULL, tname, task->getNameLine(), "", task, m_tmPlugin, exeMode, -1, 10);
		extTask->setTaskSynchIds(task->getTaskSynchIds());
		extTask->setBehaviorList(m_taskMem->getBehaviorTAB());
		extTask->setConnectorList(m_taskMem->getConnectorTAB());
		ExeTaskRoot* etr = new ExeTaskRoot(extTask, 10);
		
		//extTask->setMonitorList(*(m_taskMem->getMonitorInfoTAB()));
		//int result = addExeWorker(extTask);
		typedef std::pair<std::string, ExeTaskRoot*> valpair;
		m_workexeTAB.insert(valpair(tname, etr));
		LOG_TRACE(m_log, "New execution object for Task <%s> is added", tname.c_str());

		//extTask->execute2();
		etr->execute();

		return 0;
		
	}
	LOG_ERROR(m_log, "Task <%s> do not exist. Please confirm your input task name", tname.c_str());
	return DB_NO_TASK_ERROR;
}

int TaskManager::stopWorker(std::string workName)
{
	std::map<std::string, ExeTaskRoot*>::iterator it = m_workexeTAB.find(workName);
	if(it != m_workexeTAB.end()){
		ExeTaskRoot* eTaskRoot = it->second;		
		//만약 eTask의 m_runner에서 디버깅 중에서 다음 명령을 대기 중에 있다면 
		//terminate()함수에서도 eTask가 join되기를 대기하기 때문에 대기가 풀여야 termiante()가 완료된다.		
		eTaskRoot->getExeTask()->forceStop();
		eTaskRoot->wait4join();
		delete eTaskRoot;
		m_workexeTAB.erase(it);
DbgTable::clearTable();
		return 0;
	}
	LOG_ERROR(m_log, "Task <%s> is not running. Please confirm your input task name", workName.c_str());
	return DB_STOP_ERR_NO_EXE_WORKER;
}

int TaskManager::stopAllWorker()
{
	std::map<std::string, ExeTaskRoot*>::iterator it;
	for(it=m_workexeTAB.begin(); it!=m_workexeTAB.end(); it++){
		ExeTaskRoot* eTaskRoot = it->second;
		if(eTaskRoot != NULL){
			eTaskRoot->getExeTask()->forceStop();
			eTaskRoot->wait4join();
			delete eTaskRoot; 
			eTaskRoot = NULL;
		}
	}
	m_workexeTAB.clear();
	return 0;
}

std::string TaskManager::getLoadedWorkersInfo()
{
/*	std::string result = "";
	bool first = true;
	StrMap*	taskTAB = m_taskMem->getTaskTAB();
	StrMap::iterator it;
	for(it=taskTAB->begin() ; it!=taskTAB->end() ; it++){
		if(!first)result.append("\r\n");
		std::string worker	=  it->first;
		std::string itask	=  it->second;
		result.append("Task:").append(worker).append("  ").append("Start_Behavior:").append(itask);
		first = false;
	}

	
	first = true;
*/
	std::string result = "";
	bool first = true;
	TaskTAB* taskTAB2 = m_taskMem->getTaskTAB2();
	TaskTAB::iterator it2;
	for(it2=taskTAB2->begin() ; it2!=taskTAB2->end() ; it2++){
		if(!first)result.append("\r\n");
		std::string worker	=  it2->first;
		RFSMtask* itask	= it2->second;
		result.append("Task:").append(worker).append(",  ").append("Start_Behavior:");
		std::vector<std::string> startNodes = itask->getLinkedNodes();
		if(startNodes.size()>0){
			for(unsigned int i =0; i<startNodes.size();i++){
				result.append(startNodes[i]).append(" ");
			}
		}
		
		first = false;
	}

	return result;
}

std::string TaskManager::getLoadedWorkersList()
{
	std::string result = "[";
	bool first = true;
	TaskTAB* taskTAB2 = m_taskMem->getTaskTAB2();
	TaskTAB::iterator it2;
	for(it2=taskTAB2->begin() ; it2!=taskTAB2->end() ; it2++){
		if(!first)result.append(",");
		std::string worker	=  it2->first;
		result.append(worker.c_str());
		first = false;
	}

	return result.append("]");
}

std::string TaskManager::getRunningWorkersInfo()
{
	std::string result = "";
	bool first = true;
	std::map<std::string, ExeTaskRoot*>::iterator it;
	for(it=m_workexeTAB.begin() ; it!=m_workexeTAB.end() ; it++){
		if(!first)result.append("\r\n");
		std::string worker	=  it->first;
		ExeTaskRoot* exeRoot =  it->second;
		ExeTask* exeW =  exeRoot->getExeTask();
		result.append("Task:").append(exeW->getNodeName().c_str()).append(",  ").append("Start_Behavior:").append(exeW->getStartBehaviorName().c_str());
		first = false;
	}
	return result;
}

std::string TaskManager::getRunningWorkersList()
{
	std::string result = "[";
	bool first = true;
	std::map<std::string, ExeTaskRoot*>::iterator it;
	for(it=m_workexeTAB.begin() ; it!=m_workexeTAB.end() ; it++){
		if(!first)result.append(",");
		std::string worker	=  it->first;
		result.append(worker.c_str());
		first = false;
	}
	return result.append("]");
}
std::string TaskManager::getBehaviorsInfo()
{
	std::string result = "";
	bool first0 = true;
	BehaviorTAB::iterator it;
	for(it=m_taskMem->getBehaviorTAB()->begin(); it!=m_taskMem->getBehaviorTAB()->end(); it++){
		if(!first0)result.append("\r\n");
		std::string		tName	=  it->first;
		RFSMbehavior*	bhv		=  it->second;
		std::string statelist = "";

		std::vector<RFSMstate*>* states = bhv->getAllStates();
		if(states != NULL){
			bool first = true;
			for(unsigned int i=0 ; i<states->size(); i++){
				if(!first) statelist.append(", ");
				RFSMstate* state = (*states)[i];
				std::string sName  = state->getName();
				std::vector<std::string> ns = Estring::split(sName, ".");
				statelist.append(ns.at(1));
				first =false;
			}
		}
		result.append("Behavior(").append(tName.c_str()).append(")");
		result.append("\r\n   +");
		result.append("States={").append(statelist).append("}");
		first0 = false;
	}
	return result;
}

std::string TaskManager::getConnectorsInfo()
{
	std::string result = "";
	ConnectorTAB::iterator it;
	for(it=m_taskMem->getConnectorTAB()->begin(); it!=m_taskMem->getConnectorTAB()->end(); it++){
		result.append("\r\n");
		std::string tName	=  it->first;
		RFSMconnector* bhv	=  it->second;
		result.append("Connector(").append(tName.c_str()).append(")");
	}
	return result;
}

ExeTask* TaskManager::getExeTask(std::string tName)
{
	std::map<std::string, ExeTaskRoot*>::iterator it = m_workexeTAB.find(tName);
	if(it != m_workexeTAB.end()){
		ExeTaskRoot* eTaskRoot = it->second;
		return eTaskRoot->getExeTask();
	}
	return NULL;
}

bool TaskManager::isTaskRunning()
{
	if(m_workexeTAB.size()>0){
		return true;
	}
	return false;
}

RFSMtask* TaskManager::getTask(std::string tName)
{
	return m_taskMem->getTask2(tName);
}



