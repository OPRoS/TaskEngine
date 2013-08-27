#include "comm/RemoteCLISvr.h"

RemoteCLISvr::RemoteCLISvr(ModelBinder* binder, OSFileSys* fsys)
{
	BUFSIZE		= 255;
	m_binder	= binder;
	m_port		= TaskConfig::getCLIport();
	
	/**********************************************************
	클라이언트별 다른 세션을 제공하고자 하는 경우
	 (RemoteCLI가 하나의 세션을 제공)
	***********************************************************/
	//for(unsigned int i =0 ; i<m_connCunt;i++){
	//	RemoteCLI* rc = new RemoteCLI(this, m_binder, fsys);
	//	m_clientPool.push_back(rc);
	//}

	/**********************************************************
	다수 클라이언트들이 단일 세션을 공유하고자 하는 경우
	***********************************************************/
	RemoteCLI* rc = new RemoteCLI(this, m_binder, fsys);
	for(unsigned int i =0 ; i<m_connCunt;i++){
		m_clientPool.push_back(rc);
	}
}

RemoteCLISvr:: ~RemoteCLISvr(void)
{
	//delete m_binder; 외부에서 삭제된다.

	if(m_clientPool.size() > 0){
		std::list<RemoteCLI*>::iterator it;
		for(it=m_clientPool.begin() ; it!=m_clientPool.end() ; it++){
			delete (*it);
		}
		m_clientPool.clear();
	}	
	
	m_running = false;
	wait4join();
}

RemoteCLI* RemoteCLISvr::getRemoteCLI(void)
{
	if(m_clientPool.size() > 0){
		std::list<RemoteCLI*>::iterator it;
		for(it=m_clientPool.begin() ; it!=m_clientPool.end() ; it++){
			RemoteCLI* rc = (*it);
			if(rc->isAvailable())
				return rc;
		}		
	}	

	return NULL;
}

int RemoteCLISvr::closeOStream(SOCKET clSocket)
{
#ifdef _WIN32
	return shutdown(clSocket, SD_SEND);
#else
	return shutdown(clSocket, SHUT_WR);
#endif
}

/*
int RemoteCLISvr::stop()
{
	m_running = false;
	cleanUp();
	return 0;
}
*/

void RemoteCLISvr::cbackBy_Thrfunc()
{	
	if(m_port == 0) return;

	m_running = true;
	createSocket(m_port);
	std::list<RemoteCLI> clientList;

	printf("The remote shell of TaskExecutor is opened with port %d.\n", m_port);
	while(1){
		if(acceptClient()==0){		
			//acceptClient에서 생성된 client socket을 얻어온다.
			SOCKET cliSock = getRecentClient();
			std::string str = "Connection to TPL-R command shell is succeed\r\n";
			//str.append("Control commands { delpoy, undeploy, run, stop, exit }\r\n");
			//str.append("File commands { dir, dirAll, chdir, rmdir, rm, copy, rename, exist, upload }\r\n");

			sendMsgWithSize(cliSock, str, false);

			//새로운 RemoteCLI를 생성하고 이것을 쓰레드로 실행
			RemoteCLI* rc = getRemoteCLI();
			if(rc != NULL){				
				rc->setClient(cliSock);
				rc->init();
				cmdProc(rc, cliSock);
				//rc->create_thr(RThread::ATTACH, "REMOTE_CLI");
				
				/**********************************************************
				다중 클라이언트 지원하려면 아래 wait4join을 마킹하라.
				***********************************************************/
				//rc->wait4join();

				/**********************************************************
				클라이언트와 종료 후에 태스크 실행기가 종료됨
				//break;
				***********************************************************/
				
			}
		}
		else{
			printf("Error in Task-executor while accepting a client.\n");			
		}
	}
	printf("The Remote-CLI server for OPRoS TaskExecutor is stopped.\n");
	m_running = false;
}

void RemoteCLISvr::cmdProc(RemoteCLI* rc, SOCKET cliSocket)
{
	std::string prompt = "TPL-R> ";
	std::vector<std::string> argv;
	std::ostringstream* ostr = rc->ostrStream();
	Log* log = rc->getLog();
	while(1){
		//prompt 전송		
		//m_svrSocket->sendMsgWithSize(m_cliSocket, prompt);
		if(rc->isPromptOn()) rc->writeMsg2Sock(prompt);
		
		//pthread_mutex_lock같은 걸 이 부분에서 하면 안됨, 내부 함수에서 lock, unlock을 많이함

		//m_svrSocket->writeSocket(m_cliSocket, (char*)prompt.c_str(), prompt.size());		

		argv = rc->getArgv();
		std::vector<std::string>::iterator it = argv.begin();
		std::string arg1 = (*it);
		it++;

		bool hascmd = false;
		std::string cmd;
		if(arg1 == "REQ-start"){
			cmd = "Debug-";
			if(argv.size()>1){ //REQ-start만 들어오는 경우에 대비
				hascmd = true;
				cmd.append(*it);
			}
		}
		else
			cmd = arg1;

		LOG_INFO((*log), "Command: %s", cmd.c_str());

		if(arg1 == "REQ-start" && hascmd){
			unsigned int type;
			if((*it)=="register")		type = CMD_register;
			else if((*it)=="remove")	type = CMD_remove;
			else if((*it)=="start")		type = CMD_start;
			else if((*it)=="suspend")	type = CMD_suspend;
			else if((*it)=="resume")	type = CMD_resume;
			else if((*it)=="stop")		type = CMD_stop;
			else if((*it)=="set")		type = CMD_set;
			else if((*it)=="emul")		type = CMD_emul;
			it++;
			MonitorRequest mr = rc->getMonitorReq(type, *it);	
			rc->procMonitorCmd(rc->getLocalCLI(), mr);
		}
		else if(arg1 == "on"){
			std::string obj;
			if(argv.size() == 1){
				std::string msg = "on ('size'|'prompt'): ";
				rc->writeMsg2Sock(msg);				
				obj = rc->getCommand();
			}
			else if(argv.size() == 2){
				obj = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: on ['size'|'prompt']\r\n";				
				rc->writeMsg2Sock(emsg);	
			}
			if(obj.compare("")!=0){
				if(obj == "size") rc->setMsgSizeOn(true);
				if(obj == "prompt")rc->setPromptOn(true);
			}
		}
		else if(arg1 == "off"){
			std::string obj;
			if(argv.size() == 1){
				std::string msg = "off ('size'|'prompt): ";
				rc->writeMsg2Sock(msg);				
				obj = rc->getCommand();
			}
			else if(argv.size() == 2){
				obj = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: off ['size'|'prompt']\r\n";				
				rc->writeMsg2Sock(emsg);	
			}
			if(obj.compare("")!=0){
				if(obj == "size") rc->setMsgSizeOn(false);
				if(obj == "prompt") rc->setPromptOn(false);
			}
		}
		else if(arg1 == "resultprint"){
			rc->writeStream2Sock();
		}
		else if(arg1.compare("deploy")==0){
			//printf(">>deploy\n");
			std::string fName = "";
			if(argv.size() ==1){
				std::string msg = "Enter Task file name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task file name: ", 22);				
				rc->writeMsg2Sock(msg);
				fName = rc->getCommand();
			}
			else if(argv.size() == 2){
				fName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: deploy [filename]\r\n";
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);
				rc->writeMsg2Sock(emsg);				
			}
			if(fName.compare("")!=0){
				std::string tplFname = rc->getFnameTPLRoot(fName);
			
				//std::string tn = tplFname;
				//if(tplFname.find_first_of("/")==0){   
				//	tn = tplFname.replace(0, 1, "");
				//}
				//m_lc->loadWorker(tn);
				int result = rc->getLocalCLI()->loadWorker(tplFname);		
				if(TaskConfig::isAutoResultPrint()) rc->writeStream2Sock();
			}
		}
		else if(arg1.compare("setDeployHome")==0){
			std::string deployHome ="";
			if(argv.size() == 1){
				std::string msg = "Enter deployment target path: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);				
				rc->writeMsg2Sock(msg);
				deployHome = rc->getCommand();
				TaskConfig::setDeployHome(deployHome);
			}
			else if(argv.size() == 2){
				deployHome = (*it);
				TaskConfig::setDeployHome(deployHome);
			}
			else{
				std::string emsg = "Too many arguments. Usage: setDeployHOme [targetPath]\r\n";
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);				
				rc->writeMsg2Sock(emsg);
			}
		}
		else if(arg1.compare("undeploy")==0){
			//printf(">>undeloy\n");
			std::string tName = "";
			if(argv.size() == 1){
				std::string msg = "Enter Task name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);				
				rc->writeMsg2Sock(msg);
				tName = rc->getCommand();
			}
			else if(argv.size() == 2){
				tName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: undeploy [taskname]\r\n";				
				rc->writeMsg2Sock(emsg);
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);
			}
			if(tName.compare("")!=0){
				rc->getLocalCLI()->unloadWorker(tName);
				if(TaskConfig::isAutoResultPrint()) rc->writeStream2Sock();
			}
		}
		else if(arg1.compare("run")==0){
			std::string taskName ="";
			if(argv.size() == 1){
				std::string msg = "Enter Task name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);				
				rc->writeMsg2Sock(msg);
				taskName = rc->getCommand();
			}
			else if(argv.size() == 2){
				taskName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: run [taskname]\r\n";
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);				
				rc->writeMsg2Sock(emsg);
			}

			if(taskName.compare("")!=0){
				int result = rc->getLocalCLI()->startWorker(taskName, NORMAL); //false는 디버깅모드가 아님을 의미
				if(TaskConfig::isAutoResultPrint()) rc->writeStream2Sock();
				if(result==0){//만약 정상 동작이면 모니터링할 때 exeTask에 SOCKET값을 할당하기 위해서
					ExeTask* exeTask = rc->getLocalCLI()->getExeTask(taskName);			
				}
			}
		}
		else if(arg1.compare("stop")==0){
			std::string taskName ="";
			if(argv.size() == 1){
				std::string msg = "Enter Task name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);
				rc->writeMsg2Sock(msg);				
				taskName = rc->getCommand();
			}
			else if(argv.size() == 2){
				taskName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: stop [taskname]\r\n";				
				rc->writeMsg2Sock(emsg);	
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);
			}
			if(taskName.compare("")!=0){
				rc->getLocalCLI()->stopWorker(taskName);
				DbgTable::clearTable();
				if(TaskConfig::isAutoResultPrint()) rc->writeStream2Sock();
			}
		}
		else if(arg1.compare("list")==0){
			rc->getLocalCLI()->list();
			rc->writeStream2Sock();
		}
		else if(arg1.compare("getDeployedTasks")==0){
			std::string list = rc->getLocalCLI()->getLoadedWorkerList();
			std::ostringstream* os = rc->ostrStream();
			if(os != NULL){
				(*os)<<list<<"\r\n";
				rc->writeStream2Sock();
			}
		}
		else if(arg1.compare("getRunningTasks")==0){
			std::string list = rc->getLocalCLI()->getRunningWorkerList();
			std::ostringstream* os = rc->ostrStream();
			if(os != NULL){
				(*os)<<list<<"\r\n";
				rc->writeStream2Sock();
			}
		}
		else if(arg1.compare("dir")==0){	
			StringList list;
			bool result = false;
			if(argv.size()>1){
				std::string path0 = (*it);
				std::string path = rc->changeDir2FullPath(path0, true);
				path = Estring::trim(path);				
				result = rc->getFileSys()->listFiles(list, path, "*");
			}
			else{
				result = rc->getFileSys()->listFiles(list, "*");				
			}
			if(result)
				rc->writeMsgList2Sock(list);
			else
				rc->writeMsg2Sock("Directory path error!\r\n");
		}
		else if(arg1.compare("dirAll")==0){		
			std::string result;
			std::string homedir = rc->changeDir2FullPath("/", true);
			rc->getFileSys()->listAllDirNFiles(result, homedir);
			rc->writeMsg2Sock(result);
//			writeMsgList2Sock(result);
		}
		else if(arg1.compare("pwd")==0){
			std::string pwd = rc->getFileSys()->getDir();
			Estring::replaceAll(pwd, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
			pwd.append("\r\n");
			rc->writeMsg2Sock(pwd);
		}
		else if(arg1.compare("home")==0){
			rc->getFileSys()->setDir(TaskConfig::getDeployHome());
			std::string pwd = rc->getFileSys()->getDir();
			Estring::replaceAll(pwd, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
			pwd.append("\r\n");
			rc->writeMsg2Sock(pwd);
		}
		else if(arg1.compare("exist")==0){					
			if(argv.size()>1){
				std::string fname = (*it);
				fname = Estring::trim(fname);
				std::string fname2 = rc->changeDir2FullPath(fname, false);
				bool result = rc->getFileSys()->existsFile(fname2);
				std::string msg = "No";
				if(result) msg = "Yes";
				msg.append("\r\n");
				rc->writeMsg2Sock(msg);
			}		
		}
		else if(arg1.compare("chdir")==0){//파일을 업로드 하기전에 target 디렉토리로 변경한다.
			int i = argv.size();
			if(argv.size()>1){
				std::string dir = (*it);
				dir = Estring::trim(dir);
				std::string dir1 = rc->changeDir2FullPath(dir, true);
				int r = rc->getFileSys()->setDir(dir1);	
				
				//결과출력
				std::string result;
				if(r==0){
					result = rc->getFileSys()->getDir();
				}
				else{
					result = std::string("Error:202, Not accessible directory");
				}
				Estring::replaceAll(result, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
				result.append("\r\n");
				rc->writeMsg2Sock(result);
			}
		}
		else if(arg1.compare("mkdir")==0){
			for(;it<argv.end();it++){ //여러개의 디렉토리 생성 가능 mkdir aaa bbb ccc
				std::string dir = (*it);
				dir = Estring::trim(dir);
				std::string targetDir = rc->changeDir2FullPath(dir, true);
				int ok = rc->getFileSys()->mkdir(targetDir);
	std::cout<<ok<<std::endl;

				//결과
				std::string msg;
				Estring::replaceAll(targetDir, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
				if(ok!=0){				
					std::string emsg = "Error, mkdir, "+ targetDir +"\r\n";
					rc->writeMsg2Sock(emsg);					
				}			
				else{
					//ostr<<"Ok, mkdir, "<<targetDir<<"\r\n";
					//writeStream2Sock(ostr);
					rc->writeMsg2Sock("RES-start;msgack;@reqid["+dir+"]@error["+int2str(ok)+"];RES-end");
				}
			}
		}
		else if(arg1.compare("rmdir")==0){
			for(;it<argv.end();it++){ //여러개의 디렉토리 삭제 가능rmdir aaa bbb ccc
				std::string dir = (*it);
				dir = Estring::trim(dir);
				std::string targetDir = rc->changeDir2FullPath(dir, true);			
				bool ok = rc->getFileSys()->rmdir(targetDir);

				//결과
				std::string msg;
				Estring::replaceAll(targetDir, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
				if(!ok){				
					std::string emsg = "Error, rmdir, "+ targetDir +"\r\n";
					rc->writeMsg2Sock(emsg);					
				}			
				else{
					(*ostr)<<"Ok, rmdir, "<<targetDir<<"\r\n";
					rc->writeStream2Sock();
				}
			}
		}		
		else if(arg1.compare("rm")==0){
			for(;it<argv.end();it++){ 
			
				std::string fname = (*it);
				fname = Estring::trim(fname);				
				std::string fname2 = rc->changeDir2FullPath(fname, false);				
				bool ok = rc->getFileSys()->removeFile(fname2);

				//결과
				std::string msg;
				Estring::replaceAll(fname2, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
				if(!ok){				
					std::string emsg = "Error, rm, "+ fname2 +"\r\n";
					rc->writeMsg2Sock(emsg);					
				}			
				else{
					(*ostr)<<"Ok, rm, "<<fname2<<"\r\n";
					rc->writeStream2Sock();
				}
			}		
		}
		else if(arg1.compare("rename")==0){
			if(argv.size()>2){
				std::string f0 = (*it);
				std::string f1 = (*++it);
				f0 = Estring::trim(f0);				
				f1 = Estring::trim(f1);
				std::string fname0 = rc->changeDir2FullPath(f0, false);		
				std::string fname1 = rc->changeDir2FullPath(f1, false);		

				bool ok = rc->getFileSys()->renameFile(fname0, fname1);

				std::string msg;
				Estring::replaceAll(fname0, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
				Estring::replaceAll(fname1, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
				if(!ok){				
					std::string emsg = "Error, rename, "+ fname0+" to "+ fname1 +"\r\n";
					rc->writeMsg2Sock(emsg);					
				}			
				else{
					(*ostr)<<"Ok, rename, "<< fname0<<" to "<<fname1<<"\r\n";
					rc->writeStream2Sock();
				}
			}		
		}
		else if(arg1.compare("copy")==0){
			if(argv.size()>2){
				std::string f0 = (*it);
				std::string f1 = (*++it);
				f0 = Estring::trim(f0);				
				f1 = Estring::trim(f1);
				std::string fname0 = rc->changeDir2FullPath(f0, false);		
				std::string fname1 = rc->changeDir2FullPath(f1, false);		

				bool ok = rc->getFileSys()->copyFile(fname0, fname1);

				std::string msg;
				Estring::replaceAll(fname0, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
				Estring::replaceAll(fname1, TaskConfig::getDeployHome(), ""); //homeDir(절대 경로를 제거)
				if(!ok){				
					std::string emsg = "Error, copy, "+ fname0+" to "+ fname1 +"\r\n";
					rc->writeMsg2Sock(emsg);					
				}			
				else{
					(*ostr)<<"Ok, copy, "<< fname0<<" to "<<fname1<<"\r\n";
					rc->writeStream2Sock();
				}
			}		
		}
		else if(arg1.compare("download")==0){				
			for(;it<argv.end();it++){
				std::string filename = (*it);
				std::string tplfname = rc->getFnameTPLRoot(filename);
				std::string home;
				if(tplfname == "tpl.cfg")
					home = TaskConfig::getExecutorHome() + tplfname;
				else
					home = TaskConfig::getDeployHome() + tplfname;
				int r = rc->tranFile(home, (char*)tplfname.c_str());
			}			
		}
		else if(arg1.compare("upload")==0){
			//1. upload 명령전송
			//2. 헤더와 내용 전송, fname%test.txt%10:1234567890
			//3. EOJ: 문자 전송
			while(1){
				std::string head = rc->recvHead();
				if(head.compare("EOJ")==0)
					break;
			
				if(head.find_first_of("ERROR")== 0){//ERROR로 시작
					printf("%s", (char*)head.c_str());
					break;
				}

				std::string key("fname%");			
				std::vector<std::string> strs = Estring::split(head, '%');
				if(strs.size()==3){
					//std::vector<std::string>::iterator it2 = strs.begin();
					std::string fname;
					if(fname == "tpl.cfg")
						fname = TaskConfig::getExecutorHome();
					else
						fname = rc->getFileSys()->getDir();//upload를 fsys를 사용해야 한다. homedir은 고정(TaskConfig파일).. chdir은 유동
					fname.append(strs[1]);
					std::string fsizestr = strs[2];
					int fsize = str2int(fsizestr);
					rc->recvFile((char*)fname.c_str(), fsize);
				}				
			}
		}
		else if(arg1.compare("exit")==0){	
			std::string u = "";
			do{
				std::string msg = "Do you want to stop all running tasks? [yes/no] : ";			
				rc->writeMsg2Sock(msg);
				u = rc->getCommand();
			}while(u != "yes" && u != "y" && u != "no" && u!="n");
			if(u == "yes" || u == "y")
				rc->getLocalCLI()->stopAllWorker();
			rc->writeStream2Sock();
			std::string msg = "Bye bye!\r\n";		
			rc->writeMsg2Sock(msg);	
			break;
		}
		else if (arg1.compare("disconnected")==0){
			rc->writeStream2Sock();
			std::string msg = "Bye bye!\r\n";		
			rc->writeMsg2Sock(msg);	
			break;
		}
		else if(arg1.compare("help")==0){
			rc->commandHelp();
		}
		else if(arg1.compare("stopserver")==0){	
			//m_svrSocket->writeSocket(m_cliSocket, "Bye bye!",8);			
			rc->getLocalCLI()->stopAllWorker();
			rc->writeStream2Sock();		
			break;
		}		
		else if(arg1.compare("")==0){
			//break;
		}
		else{
			std::string msg = "Invalid command\r\n";
			rc->writeMsg2Sock(msg);				
			//m_svrSocket->writeSocket(m_cliSocket, "Invalid command\r\n", 17);
		}
		
		//LOG_INFO(log, "RequestType:%s ...%s", arg1.c_str(), result.c_str());
		thread_sleep(20);

	}

	//클라이언트 소켓 닫기
	closeClient(cliSocket);
	rc->setAvailable(true);
	printf("Client Connection is closed.\n");
}
