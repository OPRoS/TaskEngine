#include "comm/RemoteCLI.h"

RemoteCLI::RemoteCLI(RSockServer* svrSock, SOCKET cliSock, ModelBinder* binder, OSFileSys* fsys):RThread("RemoteCLI", 10)
{
	m_svrSocket = svrSock;
	m_cliSocket = cliSock;
	m_binder	= binder;
	m_fsys		= fsys;
	m_isAvailable	= true;
	m_prompt	= true;
	m_withsize	= false;
	m_autoResultPrint = true;

	if(TaskConfig::getIsTaskLog()){	
		m_log.setLevel(Log::LOG_LEVEL_DEBUG, true);
		m_log.setLevel(Log::LOG_LEVEL_TRACE, true);
		m_log.setLevel(Log::LOG_LEVEL_ERROR, true);
	}

	
	m_lc = new LocalCLI(this, &m_ostr, m_fsys, m_binder);
}


RemoteCLI::RemoteCLI(RSockServer* svrSock, ModelBinder* binder, OSFileSys* fsys):RThread("RemoteCLI", 10)
{
	m_svrSocket = svrSock;	
	m_binder	= binder;
	m_fsys		= fsys;
	m_isAvailable	= true;
	m_prompt	= true;
	m_withsize	= false;
	m_autoResultPrint = true;

	if(TaskConfig::getIsTaskLog()){	
		m_log.setLevel(Log::LOG_LEVEL_DEBUG, true);
		m_log.setLevel(Log::LOG_LEVEL_TRACE, true);
		m_log.setLevel(Log::LOG_LEVEL_ERROR, true);
	}

	m_lc = new LocalCLI(this, &m_ostr, m_fsys, m_binder);
}


RemoteCLI:: ~RemoteCLI(void)
{	
	//m_svrSocket(������ ȣ���Ѱ�����),  m_binder�� �������� �ʴ´�.(�̰� ��������)
	if(m_lc != NULL) delete m_lc;
}

/**
 buf
*/
bool RemoteCLI::isAvailable()
{
	return m_isAvailable;
}

void RemoteCLI::setClient(SOCKET cliSock)
{
	m_cliSocket = cliSock;
	m_isAvailable = false;
}

/*
MonitorRequest* RemoteCLI::recvMonReq()
{
	const int BS = 1;

	MonitorRequest* argv = NULL;
	char buf[BS];
	std::string str = "";
	bool reqMsg = false;

	while(1){
		memset(buf, 0, BS);
		int len = m_svrSocket->readSocket(m_cliSocket, buf, BS);
		if(len == -1 || len == 0){//������ ����
			return NULL;
		}
		else{
			//buf�� �Էµ� ��� ���ڿ��� ����
			int i;
			for(i=0; i<BS; i++){
				if(buf[i] != 0 && buf[i] != 13){
					if(buf[i] == 9 || buf[i] == 32){//��� �����߿� ���� tab(9), space(32)
						//���鹫��
					}					
					else if(buf[i] != ';'){//������ ���� �д´�.
						str = str.insert(str.size(), 1, buf[i]);
					}
					else if(str.size()>0){
						if(str==REQ_end){
							return argv;
						}
						std::vector<std::string> ms = Estring::split(str, ":");
						str="";//�ʱ�ȭ
					}					
				}
				else{
					//exit if
					break;
				}
			}
			if(buf[i] == 13){
				//exit while
				break;
			}
		}
	}
	//������ ����
	

	return NULL;
}
*/

std::vector<std::string> RemoteCLI::getArgv()
{
	const int bsize = 1;
	std::vector<std::string> argv;
	unsigned char buf[bsize];
	std::string cmd = "";

	while(1){
		//���� ����� 1�̴�(�����ϱ�)
		memset(buf, 0, bsize);
		int len = m_svrSocket->readSocket(m_cliSocket, (char*)buf, bsize);
		if(len == -1 || len == 0){//������ ����
			//printf("error");
			//error_handling("Socket Read error");
			cmd="disconnected";
			argv.push_back(cmd);
			break;
		}
		else{
			//buf�� �Էµ� ��� ���ڿ��� ����
			int i;
			for(i=0; i<bsize; i++){
				if(buf[i] != 0 && buf[i] != 10 && buf[i] != 13){ //LF(10) CR(13), ���ʹ� CRLR������
					if(buf[i] == 9 || buf[i] == 32|| buf[i]==';'){//��� ������ �и��� = ���� tab(9), space(32) �Ǵ� ";" ����
						if(cmd.compare("")!=0){//�и��� �ձ��� argv�� ����(���� �����̸� �������)								
							argv.push_back(cmd);
						}
						//�ʱ�ȭ, �߰��� ������ ������ ��� �ʱ�ȭ�� �Ѵ�.
						cmd="";
					}
					else{
						//�����ϳ��ϳ��� �Է��Ͽ� cmd ����
						cmd = cmd.insert(cmd.size(), 1, buf[i]);//cmd.push_back(buf[i]);					
					}
				}
				else{
					//exit for
					break;
				}
			}
			if(buf[i] == 13){
				//exit while
				break;
			}
			if(cmd=="REQ-end"){
				//REQ-end�ٷ� ���� ���鹮�� �б�
				m_svrSocket->readSocket(m_cliSocket, (char*)buf, bsize);
				break;
			}
		}
	}
	//������ ����

	argv.push_back(cmd);
	return argv;
}

std::string RemoteCLI::getCommand()
{
	const int BSIZE = 255;

	char buf[BSIZE];
	std::string cmd = "";
	bool eoc = false;

	while(1){
		memset(buf, 0, BSIZE);
		int len = m_svrSocket->readSocket(m_cliSocket, buf, BSIZE-1);
		if(len == -1 || len == 0){//������ ����
			//printf("error");
			//error_handling("Socket Read error");
			cmd="exit";
			break;
		}
		else{
			//buf�� �Էµ� ��� ���ڿ��� ����
			int i;
			for(i=0; i<BSIZE; i++){
				if(buf[i] != 0 && buf[i] != 10 && buf[i] != 13){//LF(10) CR(13)
					cmd = cmd.insert(cmd.size(), 1, buf[i]);
				}
				else{
					eoc = true;
					break;
				}
			}
			if(buf[i] == 13){
				//exit while
				break;
			}
		}		
	}

	return cmd;
}


void GetFileList (std::vector<std::string>& list, std::string dir, std::string extension)
{
#ifdef WIN32
	//WIN32_FIND_DATA findData;
	WIN32_FIND_DATAA findData;
	HANDLE fileHandle;
	int flag = 1;
	std::string search ("*.");
	if (dir == "") dir = ".";
	dir += "/";
	search = dir + search + extension;
	// SetCurrentDirectory(dir.c_str());
	fileHandle = FindFirstFileA(search.c_str(), &findData);
	if (fileHandle == INVALID_HANDLE_VALUE) return;
	while (flag)
	{
		list.push_back(findData.cFileName);
		flag = FindNextFileA(fileHandle, &findData);
	}
	FindClose(fileHandle);

#else

	std::string search(".");
	search += extension;
	DIR* directory = opendir(dir.c_str()); // *change
	struct dirent* entry;
	std::string temp;

	while ((entry = readdir(directory)) != NULL) // see above change
	{
		temp = entry->d_name;
		if (temp.find(search, 0) != std::string::npos) // *change
		{
			list.push_back(temp);
		}
	}
	closedir(directory); // change, see first change
#endif
}

void RemoteCLI::writeMsg2Sock(std::string msg)
{
	m_svrSocket->sendMsgWithSize(m_cliSocket, msg, m_withsize);
}

void RemoteCLI::writeStream2Sock()
{
	std::string msg = m_ostr.str();
	m_svrSocket->sendMsgWithSize(m_cliSocket, msg, m_withsize);
	//m_svrSocket->writeSocket(m_cliSocket, (char*)msg.c_str(), msg.size());
	//ostr.str("");
	//ostr.clear();
	m_ostr.str("");
}

void RemoteCLI::writeMsgList2Sock(std::vector<std::string> msglist)
{
	std::vector<std::string>::iterator it;

	std::string msg = "";
	for ( it=msglist.begin() ; it < msglist.end(); it++ ){
		msg.append((*it));
		msg.append("\r\n");
	}
	//msg.append("\r\n");
	m_svrSocket->sendMsgWithSize(m_cliSocket, msg, m_withsize);
	//m_svrSocket->writeSocket(m_cliSocket, (char*)msg.c_str(), msg.size());
}

MonitorRequest RemoteCLI::getMonitorReq(unsigned int msgType, std::string attr)
{
	MonitorRequest mr = MonitorRequest(msgType);
	unsigned int result = mr.setAttributes(attr);
	if(result == REQ_ATTR_ERROR_no_target){
		//mr.setError();
	}
	return mr;
}

std::string RemoteCLI::recvHead()
{
	char buf[1];
	std::string head = "";

	while(1){	
		int len = m_svrSocket->readSocket(m_cliSocket, buf, 1);
		if(len == -1 || len == 0){ //������ ����
			printf("error");			
			m_svrSocket->closeClient(m_cliSocket);
			exit(0);
		}
		else{
			//buf�� �Էµ� ��� ���ڿ��� ����
			int i;
			for(i=0; i<len; i++){			
				if(buf[i] != ':'){
					head.push_back(buf[i]);					
				}
				else{					
					break;//exit for
				}
			}
			if(buf[i] == ':'){				
				break;//exit while
			}
		}
	}
	
	return head;
}

//������ : ĳ���� ����(Cr) & ���� �ǵ�(Lf). ������: ���� �ǵ�(Lf). ��: ĳ���� ����(Cr). 
int RemoteCLI::recvFile(char* filename, int filesize)
{
	const int BUFSIZE = 1024;
	//char* buf= new char[BUFSIZE];
	char buf[BUFSIZE+1];
		
	FILE * pFile;
	pFile = fopen (filename , "wb+");

	int rDataSize = 0;		//�̹� ���� ũ��
	int remainDSize = 0;	//���Ͽ��� ���� ũ��
	int len = 0;

	while(rDataSize<filesize){//����ũ�⺸�� ���� ����		
		remainDSize = filesize - rDataSize;	
		if(remainDSize > BUFSIZE){//���� ���� ���ۺ��� ũ��: ���۸�ŭ �д´�.
			len = m_svrSocket->readSocket(m_cliSocket, buf, BUFSIZE);
		}
		else{//���ۺ��� ������ ���� �縸ũ�� �д´�.
			len = m_svrSocket->readSocket(m_cliSocket, buf, remainDSize);
		}
	
		if(len == -1 || len == 0){//������ ����
			printf("Disconnected error\n");
			//error_handling("Socket Read error");
			//cmd="exit";
			m_svrSocket->closeClient(m_cliSocket);
			//delete buf;
			exit(0);
		}

		rDataSize = rDataSize + len;
	
		//fputs�Լ����� char* �� '\0'������ �д´�.
        buf[len] = '\0';
		
		//fputs(buf, pFile); //�Ʒ��� ���ҽ� ������ ���Ͼ��Ⱑ ������ ���� ����� ��
		unsigned int ret = 0 ;
		unsigned int trial = 0;
		const int MAX_TRIAL = 100;
		while(ret<len && MAX_TRIAL){
			ret += fwrite(buf+ret, sizeof(char), len-ret, pFile);
			++trial;
		}

		if(trial>=MAX_TRIAL){
			LOG_ERROR(m_log, "File upload failure, '%s'", getFnameTPLRoot(filename).c_str());
			break;
		}
		//std::cout<<buf;
	}
	fclose(pFile);
	LOG_INFO(m_log, "File upload success, '%s'", getFnameTPLRoot(filename).c_str());
	//delete buf;
	return 1;
}

int RemoteCLI::tranHead(char* name, unsigned long fileLen)
{	
	//���� �����̸��� ����
	std::string fname("RES-start;download;@fname[");
	fname.append(name).append("]");
	
	//������
	std::string dsize = int2str(fileLen);
	fname.append("@lengh[").append(dsize).append("];RES-end");;
	//fname.append("%").append(dsize).append(":"); //':'���� ���ʹ� ���� ������
	//m_svrSocket->writeSocket(m_cliSocket, (char*)fname.c_str(), fname.size());
	writeMsg2Sock(fname);

	return 1;
}

int RemoteCLI::tranFile(std::string home, char *name)
{
	const int BUFSIZE = 80;
	FILE *file;	
	unsigned long fileLen;

	//���� ����
	std::string fname = home;
	file = fopen(fname.c_str(), "rb");//���̳ʸ� ���
	if (!file){
		fprintf(stderr, "Unable to open file %s\n", name);
		//�Ʒ����� ':'���ڸ� ���� �ʴ� ������ �̷����� �����޽�� ������ �������� Prompt(TPL-R> :)�� ���̸� Ŭ���̾�Ʈ���� prompt���� �д´�.
		std::string errorMsg = "ERROR, Unable to open the file ";
		errorMsg.append(name).append("\n");
		writeMsg2Sock(errorMsg); 
		return -1;
	}
	
	//������ ������ ũ�⸦ ���, SEEK_SET(����ó��),SEEK_CUR, SEEK_END(��)
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);

	//�������
	tranHead(name, fileLen);

	//���� �����͸� ����
	char buffer[BUFSIZE];

	int sentSize = 0;
	int pckSize = BUFSIZE;
	
	while(sentSize<fileLen){
		
		if(fileLen- sentSize < BUFSIZE){
			pckSize = fileLen-sentSize;
		}
		else{
			pckSize = BUFSIZE;			
		}
		for(unsigned int i=0; i<pckSize; i++){     //������ ũ�� ��ŭ..
			buffer[i] = fgetc(file);        // 1����Ʈ�� �о�´�			
			sentSize++;					
		}
		m_svrSocket->writeSocket(m_cliSocket, buffer, pckSize);		
	}
	m_svrSocket->writeSocket(m_cliSocket, "RES-EOF", 7);

	fclose(file);
	return 0;
}

/*
int RemoteCLI::stop()
{
	//m_running = false;
	//cleanUp();
	stop();
	return 0;
}
*/

/**
���丮�� ������ �� ���� ���� �ý��ۿ��� ��밡���� ��η� �ٲپ� ����
*/
std::string RemoteCLI::changeDir2FullPath(std::string dir, bool isdir)
{	
	std::string deployHome = TaskConfig::getDeployHome();

	if(dir == ".."){
		std::string curDir = m_fsys->getDir();
		size_t found = curDir.find_last_of("/");
		std::string temp = curDir.substr(0, found);
		size_t found2 = temp.find_last_of("/");
		return temp.substr(0, found2);
	}

	std::string postdelim = "";
	if(isdir) postdelim = "/";
	
	if(dir == "/"){
		return deployHome + postdelim;
		
	}
	else if(dir.find_first_of("/")==0){   // '/'�� �����ϴ� ���(������)
		return deployHome + dir + postdelim;  // ex) dir /direct
	}
	else{ //����η� ���۵Ǵ� ���
		std::string curDir = m_fsys->getDir();
		if(dir == ".") 
			return curDir;
		else
			return curDir + dir + postdelim;	  // ex) dir direct
	}

	return "";
}

int RemoteCLI::procMonitorCmd(LocalCLI* lc, MonitorRequest monReq)
{
	int result;
	if(monReq.getType() == CMD_register){
		DbgInfo info;
		info.file = monReq.getAttrValue(ATTR_file);
		info.line = monReq.getAttrValue(ATTR_line);
		info.target = monReq.getAttrValue(ATTR_target);
		std::string msgid = monReq.getAttrValue(ATTR_msgid);

		if(info.file == "" || info.line == "" || info.target == "") result = -1;
		else result = DbgTable::addDbgInfo(info);
		
		//DP_ALREADY_EXIS 100
		writeMsg2Sock("RES-start;msgack;@reqid["+msgid+"]@error["+int2str(result)+"];RES-end");
	}
	if(monReq.getType() == CMD_remove){
		DbgInfo info;
		info.file = monReq.getAttrValue(ATTR_file);
		info.line = monReq.getAttrValue(ATTR_line);
		info.target = monReq.getAttrValue(ATTR_target);
		std::string msgid = monReq.getAttrValue(ATTR_msgid);

		if(info.file == "" || info.line == "" || info.target == "") result = -1;		
		else result = DbgTable::removeDbgInfo(info);	
		//DP_NOT_EXIST 102
		writeMsg2Sock("RES-start;msgack;@reqid["+msgid+"]@error["+int2str(result)+"];RES-end");
	}
	if(monReq.getType() == CMD_start){
		//�̺κ��� RemoteCLI���� ����ó���Ѵ�. (�Ķ���͸� �����ϴ� ���� ������)
		std::string thid		= monReq.getAttrValue(ATTR_thread);
		std::string isMon		= monReq.getAttrValue(ATTR_isMonitoring);
		std::string taskName	= monReq.getAttrValue(ATTR_task);
		std::string sbys		= monReq.getAttrValue(ATTR_stepbystep);
		std::string msgid		= monReq.getAttrValue(ATTR_msgid);

		//�½�ũ debugging ����
		if(thid == "0"){
			int exeMode = DEBUG;
			if(isMon == "true") exeMode = MONITOR;
			result = m_lc->startWorker(taskName, exeMode); //true�� ������带 �ǹ�
			if(result==0){//���� ���� �����̸� ����͸��� �� exeTask�� SOCKET���� �Ҵ��ϱ� ���ؼ�
				ExeTask* exeTask = m_lc->getExeTask(taskName);
				m_ostr<<"[NOTICE]: Task<"<<taskName<<"> debugging is now started...\r\n";
			}
			else{ 	
				// �����޽��� GerrorHandler.h�� ���ǵǾ�����
				// DB_EXE_WORKER_ALREADY_EXIST_ERR			10011
				// DB_EXE_WORKER_ALREADY_EXIST_SKIP			10012
				// DB_EXE_WORKER_ALREADY_EXIST_OVERWRITE	10013
				if(result == DB_EXE_WORKER_ALREADY_EXIST_ERR){
					m_ostr<<"[ERROR]: Task<"<<taskName<<"> debugging is already started...\r\n";
				}
			}
			writeMsg2Sock("RES-start;msgack;@reqid["+msgid+"]@error["+int2str(result)+"];RES-end");
		}
		else{		
			result = 0;
			ExeTask* exeTask = m_lc->getExeTask(taskName);
			//exeTask->getThreadID(); ������ ��ȣ ��??
			if(exeTask != NULL){
				if(sbys == "true")//stepbystep
					//WRONG_TH_ID 103
					result = exeTask->getController()->setMove2NextStep_On(str2int(thid));
				else
					//WRONG_TH_ID	103
					result = exeTask->getController()->setMove2NextBP_On(str2int(thid));
			}
			else{
				//WRONG_TH_ID	103
				result = WRONG_TH_ID;
			}
			writeMsg2Sock("RES-start;msgack;@reqid["+msgid+"]@error["+int2str(result)+"];RES-end");
		}
	}
	
	if(monReq.getType() == CMD_suspend){
		std::string taskName = monReq.getAttrValue(ATTR_task);
		std::string msgid = monReq.getAttrValue(ATTR_msgid);
		ExeTask* exeTask = m_lc->getExeTask(taskName);
		if(exeTask != NULL){
			result = exeTask->getController()->setSuspendMonitor();
		}
		else{
			result = WRONG_TH_ID;
		}
		writeMsg2Sock("RES-start;msgack;@reqid["+msgid+"]@error["+int2str(result)+"];RES-end");
	}

	if(monReq.getType() == CMD_resume){
		std::string taskName = monReq.getAttrValue(ATTR_task);
		std::string msgid = monReq.getAttrValue(ATTR_msgid);
		ExeTask* exeTask = m_lc->getExeTask(taskName);
		if(exeTask != NULL){
			result = exeTask->getController()->setResumeMonitor();
		}
		else{
			result = WRONG_TH_ID;
		}
		writeMsg2Sock("RES-start;msgack;@reqid["+msgid+"]@error["+int2str(result)+"];RES-end");
	}

	if(monReq.getType() == CMD_stop){
		std::string taskName = monReq.getAttrValue(ATTR_task);
		std::string msgid = monReq.getAttrValue(ATTR_msgid);
		result = m_lc->stopWorker(taskName);
		if(result == -1) result = DB_STOP_ERR_NO_EXE_WORKER;
		writeMsg2Sock("RES-start;msgack;@reqid["+msgid+"]@error["+int2str(result)+"];RES-end");
		DbgTable::clearTable();
	}
	if(monReq.getType() == CMD_set){
		std::string target	= monReq.getAttrValue(ATTR_target);		
		std::string msgid	= monReq.getAttrValue(ATTR_msgid);
		std::string etype	= monReq.getAttrValue(ATTR_etype);
		std::string value	= monReq.getAttrValue(ATTR_value);
		std::string min		= monReq.getAttrValue(ATTR_min);
		std::string max		= monReq.getAttrValue(ATTR_max);
		std::string enumlist= monReq.getAttrValue(ATTR_enumlist);

		if(target == "" || etype == "") 
			result = -1;
		else{
			EmulInfo einfo;
			einfo.etype = etype;
			if(etype == E_value)
				einfo.value = value;
			if(etype == E_randomD_G || etype ==E_randomD_U ||etype == E_randomF_G || etype == E_randomF_U){
				if(min != "") einfo.min	= str2float(min);
				if(max != "") einfo.max	= str2float(max);
			}
			if(etype == E_enum)
				einfo.enumlist = Estring::split(enumlist, ",");
					
			result = DbgTable::updateEmulInfo(target, einfo);
		}
		
		//DP_ALREADY_EXIS 100
		writeMsg2Sock("RES-start;msgack;@reqid["+msgid+"]@error["+int2str(result)+"];RES-end");
	}
	if(monReq.getType() == CMD_emul){
		std::string taOn	= monReq.getAttrValue(ATTR_targetOn);
		std::string taOff	= monReq.getAttrValue(ATTR_targetOff);
		std::string msgid	= monReq.getAttrValue(ATTR_msgid);

		result = DbgTable::updateEmulTarget(taOn, taOff);

		writeMsg2Sock("RES-start;msgack;@reqid["+msgid+"]@error["+int2str(result)+"];RES-end");
	}

	if(m_autoResultPrint) writeStream2Sock();	
	return result;
}

void RemoteCLI::commandHelp()
{
	m_ostr<<"--------------------------------------------------------"<<"\r\n";
	
	m_ostr<<"dir [directory_name]"<<"\r\n";
	m_ostr<<"    ex) dir /, dir /aaa, dir bbb/ccc"<<"\r\n";
	m_ostr<<"dirAll"<<"\r\n";
	m_ostr<<"chdir [directory_name]"<<"\r\n";
	m_ostr<<"    ex) chdir /, chdir /aaa, chdir bbb/ccc"<<"\r\n";
	m_ostr<<"rmdir [directory_name]"<<"\r\n";
	m_ostr<<"    ex) rmdir /, rmdir /aaa, rmdir bbb/ccc"<<"\r\n";
	m_ostr<<"rm [target_file]"<<"\r\n";
	m_ostr<<"    ex) rm /a.txt, rm /aaa/b.txt, rm bbb/ccc/c.txt"<<"\r\n";
	m_ostr<<"copy [source_file] [target_file]"<<"\r\n";
	m_ostr<<"    ex) copy /a.txt /ccc/b.txt , copy /a.txt /c.txt"<<"\r\n";
	m_ostr<<"rename [source_file] [target_file]"<<"\r\n";
	m_ostr<<"    ex) rename /a.txt /ccc/b.txt , rename /a.txt /c.txt"<<"\r\n";
	m_ostr<<"exist"<<"\r\n";
	m_ostr<<"    ex) exist [target_file|target_directory]"<<"\r\n";
	m_ostr<<"download [target_file(s)]"<<"\r\n";
	m_ostr<<"    ex) download /a.txt /b.txt, download /aaa/b.txt, download bbb/ccc/c.txt"<<"\r\n";
	m_ostr<<"upload"<<"\r\n";
	
	m_ostr<<"\r\n";
	m_ostr<<"deploy"<<"\r\n";
	m_ostr<<"undeploy"<<"\r\n";
	m_ostr<<"run"<<"\r\n";
	m_ostr<<"stop"<<"\r\n";
	m_ostr<<"list"<<"\r\n";
	m_ostr<<"exit"<<"\r\n";

	m_ostr<<"--------------------------------------------------------"<<"\r\n";
	writeStream2Sock();
}

std::string	RemoteCLI::getFnameTPLRoot(std::string fName)
{
	std::string tplFname = fName;
	//std::string pwd		= m_fsys->getDir(); //������ (root���� full path)
	
	std::string deployHome = TaskConfig::getDeployHome(); // deployHome�� executorHome�� ���ų� �ٸ���.
	Estring::replaceAll(tplFname, deployHome, "");

	//std::string homeDir = TaskConfig::getHomeDir();
	//if(fName.find_first_of("/")==0){   // '/'�� �����ϴ� ���(������)
	//	tplFname = fName;
	//}
	//else{
	//	Estring::replaceAll(tplFname, homeDir, "");
		//Estring::replaceAll(pwd, homeDir, ""); //������-homedir, pwd�� tpl��η� �ٲ��.
	//}	
	return tplFname;
}


/*
void RemoteCLI::cbackBy_Thrfunc()
{
	//SOCKET cliSock = getRecentClient();
	m_running = true;	

	//std::ostringstream SysFunction;
	//Log log("RemoteCLI", &SysFunction);
	//Log log;
	std::string clisoketStr = int2str(m_cliSocket);	
	m_log.setName("RemoteCLI: "+clisoketStr);
	m_log.setLevel(Log::LOG_LEVEL_INFO, true);
	m_log.setLevel(Log::LOG_LEVEL_ERROR, true);

	//const int BUFSIZE =  255;
	//char buf[BUFSIZE];
	//std::auto_ptr<LocalCLI> lc(new LocalCLI(this, &ostr, m_binder));
	//LocalCLI* lc = new LocalCLI(this, &ostr, m_fsys, m_binder);
	
	m_homeDir = TaskConfig::getHomeDir();
	m_autoResultPrint = TaskConfig::isAutoResultPrint();
	
	// Ȩ���丮 ����
	//fsys.open();
	//if(homeDir == "") homeDir = getCurPath();
	//fsys.setDir(homeDir);

	if(TaskConfig::isAutoDeploy()){
		std::string msg = "Auto deploy ...\r\n";
		//m_svrSocket->writeSocket(m_cliSocket, "Auto deploy ...\r\n", 17);		
		writeMsg2Sock(msg);
		m_lc->autoLoad();
		writeStream2Sock();
	}
	


	std::string prompt = "TPL-R> ";
	std::vector<std::string> argv;

	while(1){
		//prompt ����		
		//m_svrSocket->sendMsgWithSize(m_cliSocket, prompt);
		if(m_prompt)writeMsg2Sock(prompt);
		
		//pthread_mutex_lock���� �� �� �κп��� �ϸ� �ȵ�, ���� �Լ����� lock, unlock�� ������

		//m_svrSocket->writeSocket(m_cliSocket, (char*)prompt.c_str(), prompt.size());		

		argv = getArgv();
		std::vector<std::string>::iterator it = argv.begin();
		std::string arg1 = (*it);
		it++;

		bool hascmd = false;
		std::string cmd;
		if(arg1 == "REQ-start"){
			cmd = "Debug-";
			if(argv.size()>1){ //REQ-start�� ������ ��쿡 ���
				hascmd = true;
				cmd.append(*it);
			}
		}
		else
			cmd = arg1;

		LOG_INFO(m_log, "Command: %s", cmd.c_str());

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
			MonitorRequest mr = getMonitorReq(type, *it);	
			procMonitorCmd(m_lc, mr);
		}
		else if(arg1 == "on"){
			std::string obj;
			if(argv.size() == 1){
				std::string msg = "on ('size'|'prompt'): ";
				writeMsg2Sock(msg);				
				obj = getCommand();
			}
			else if(argv.size() == 2){
				obj = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: on ['size'|'prompt']\r\n";				
				writeMsg2Sock(emsg);	
			}
			if(obj.compare("")!=0){
				if(obj == "size") m_withsize = true;
				if(obj == "prompt") m_prompt = true;
			}
		}
		else if(arg1 == "off"){
			std::string obj;
			if(argv.size() == 1){
				std::string msg = "off ('size'|'prompt): ";
				writeMsg2Sock(msg);				
				obj = getCommand();
			}
			else if(argv.size() == 2){
				obj = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: off ['size'|'prompt']\r\n";				
				writeMsg2Sock(emsg);	
			}
			if(obj.compare("")!=0){
				if(obj == "size") m_withsize = false;
				if(obj == "prompt") m_prompt = false;
			}
		}
		else if(arg1 == "resultprint"){
			writeStream2Sock();
		}
		else if(arg1.compare("deploy")==0){
			//printf(">>deploy\n");
			std::string fName = "";
			if(argv.size() ==1){
				std::string msg = "Enter Task file name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task file name: ", 22);				
				writeMsg2Sock(msg);
				fName = getCommand();
			}
			else if(argv.size() == 2){
				fName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: deploy [filename]\r\n";
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);
				writeMsg2Sock(emsg);				
			}
			if(fName.compare("")!=0){
				std::string tplFname = getFnameTPLRoot(fName);
			
				//std::string tn = tplFname;
				//if(tplFname.find_first_of("/")==0){   
				//	tn = tplFname.replace(0, 1, "");
				//}
				//m_lc->loadWorker(tn);
				int result = m_lc->loadWorker(tplFname);		
				if(m_autoResultPrint) writeStream2Sock();
			}
		}
		else if(arg1.compare("undeploy")==0){
			//printf(">>undeloy\n");
			std::string tName = "";
			if(argv.size() == 1){
				std::string msg = "Enter Task name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);				
				writeMsg2Sock(msg);
				tName = getCommand();
			}
			else if(argv.size() == 2){
				tName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: undeploy [taskname]\r\n";				
				writeMsg2Sock(emsg);
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);
			}
			if(tName.compare("")!=0){
				m_lc->unloadWorker(tName);
				if(m_autoResultPrint) writeStream2Sock();
			}
		}
		else if(arg1.compare("run")==0){
			std::string taskName ="";
			if(argv.size() == 1){
				std::string msg = "Enter Task name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);				
				writeMsg2Sock(msg);
				taskName = getCommand();
			}
			else if(argv.size() == 2){
				taskName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: run [taskname]\r\n";
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);				
				writeMsg2Sock(emsg);
			}

			if(taskName.compare("")!=0){
				int result = m_lc->startWorker(taskName, NORMAL); //false�� ������尡 �ƴ��� �ǹ�
				if(m_autoResultPrint) writeStream2Sock();
				if(result==0){//���� ���� �����̸� ����͸��� �� exeTask�� SOCKET���� �Ҵ��ϱ� ���ؼ�
					ExeTask* exeTask = m_lc->getExeTask(taskName);			
				}
			}
		}
		else if(arg1.compare("stop")==0){
			std::string taskName ="";
			if(argv.size() == 1){
				std::string msg = "Enter Task name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);
				writeMsg2Sock(msg);				
				taskName = getCommand();
			}
			else if(argv.size() == 2){
				taskName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: stop [taskname]\r\n";				
				writeMsg2Sock(emsg);	
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);
			}
			if(taskName.compare("")!=0){
				m_lc->stopWorker(taskName);
				DbgTable::clearTable();
				if(m_autoResultPrint) writeStream2Sock();
			}
		}
		else if(arg1.compare("list")==0){
			m_lc->list();
			writeStream2Sock();
		}
		else if(arg1.compare("dir")==0){	
			StringList list;
			bool result = false;
			if(argv.size()>1){
				std::string path0 = (*it);
				std::string path = getTargetDir(path0, true);
				path = Estring::trim(path);				
				result = m_fsys->listFiles(list, path, "*");
			}
			else{
				result = m_fsys->listFiles(list, "*");				
			}
			if(result)
				writeMsgList2Sock(list);
			else
				writeMsg2Sock("Directory path error!\r\n");
		}
		else if(arg1.compare("dirAll")==0){		
			std::string result;
			std::string homedir = getTargetDir("/", true);
			m_fsys->listAllDirNFiles(result, homedir);
			writeMsg2Sock(result);
//			writeMsgList2Sock(result);
		}
		else if(arg1.compare("pwd")==0){
			std::string pwd = m_fsys->getDir();
			Estring::replaceAll(pwd, m_homeDir, ""); //homeDir(���� ��θ� ����)
			pwd.append("\r\n");
			writeMsg2Sock(pwd);
		}
		else if(arg1.compare("home")==0){
			m_fsys->setDir(m_homeDir);
			std::string pwd = m_fsys->getDir();
			Estring::replaceAll(pwd, m_homeDir, ""); //homeDir(���� ��θ� ����)
			pwd.append("\r\n");
			writeMsg2Sock(pwd);
		}
		else if(arg1.compare("exist")==0){					
			if(argv.size()>1){
				std::string fname = (*it);
				fname = Estring::trim(fname);
				std::string fname2 = getTargetDir(fname, false);
				bool result = m_fsys->existsFile(fname2);
				std::string msg = "No";
				if(result) msg = "Yes";
				msg.append("\r\n");
				writeMsg2Sock(msg);
			}		
		}
		else if(arg1.compare("chdir")==0){//������ ���ε� �ϱ����� target ���丮�� �����Ѵ�.
			int i = argv.size();
			if(argv.size()>1){
				std::string dir = (*it);
				dir = Estring::trim(dir);
				std::string dir1 = getTargetDir(dir, true);
				int r = m_fsys->setDir(dir1);	
				
				//������
				std::string result;
				if(r==0){
					result = m_fsys->getDir();
				}
				else{
					result = std::string("Error:202, Not accessible directory");
				}
				Estring::replaceAll(result, m_homeDir, ""); //homeDir(���� ��θ� ����)
				result.append("\r\n");
				writeMsg2Sock(result);
			}
		}
		else if(arg1.compare("mkdir")==0){
			for(;it<argv.end();it++){ //�������� ���丮 ���� ���� mkdir aaa bbb ccc
				std::string dir = (*it);
				dir = Estring::trim(dir);
				std::string targetDir = getTargetDir(dir, true);
				int ok = m_fsys->mkdir(targetDir);
	std::cout<<ok<<std::endl;

				//���
				std::string msg;
				Estring::replaceAll(targetDir, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(ok!=0){				
					std::string emsg = "Error, mkdir, "+ targetDir +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					//ostr<<"Ok, mkdir, "<<targetDir<<"\r\n";
					//writeStream2Sock(ostr);
					writeMsg2Sock("RES-start;msgack;@reqid["+dir+"]@error["+int2str(ok)+"];RES-end");
				}
			}
		}
		else if(arg1.compare("rmdir")==0){
			for(;it<argv.end();it++){ //�������� ���丮 ���� ����rmdir aaa bbb ccc
				std::string dir = (*it);
				dir = Estring::trim(dir);
				std::string targetDir = getTargetDir(dir, true);			
				bool ok = m_fsys->rmdir(targetDir);

				//���
				std::string msg;
				Estring::replaceAll(targetDir, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(!ok){				
					std::string emsg = "Error, rmdir, "+ targetDir +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					m_ostr<<"Ok, rmdir, "<<targetDir<<"\r\n";
					writeStream2Sock();
				}
			}
		}		
		else if(arg1.compare("rm")==0){
			for(;it<argv.end();it++){ 
			
				std::string fname = (*it);
				fname = Estring::trim(fname);				
				std::string fname2 = getTargetDir(fname, false);				
				bool ok = m_fsys->removeFile(fname2);

				//���
				std::string msg;
				Estring::replaceAll(fname2, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(!ok){				
					std::string emsg = "Error, rm, "+ fname2 +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					m_ostr<<"Ok, rm, "<<fname2<<"\r\n";
					writeStream2Sock();
				}
			}		
		}
		else if(arg1.compare("rename")==0){
			if(argv.size()>2){
				std::string f0 = (*it);
				std::string f1 = (*++it);
				f0 = Estring::trim(f0);				
				f1 = Estring::trim(f1);
				std::string fname0 = getTargetDir(f0, false);		
				std::string fname1 = getTargetDir(f1, false);		

				bool ok = m_fsys->renameFile(fname0, fname1);

				std::string msg;
				Estring::replaceAll(fname0, m_homeDir, ""); //homeDir(���� ��θ� ����)
				Estring::replaceAll(fname1, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(!ok){				
					std::string emsg = "Error, rename, "+ fname0+" to "+ fname1 +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					m_ostr<<"Ok, rename, "<< fname0<<" to "<<fname1<<"\r\n";
					writeStream2Sock();
				}
			}		
		}
		else if(arg1.compare("copy")==0){
			if(argv.size()>2){
				std::string f0 = (*it);
				std::string f1 = (*++it);
				f0 = Estring::trim(f0);				
				f1 = Estring::trim(f1);
				std::string fname0 = getTargetDir(f0, false);		
				std::string fname1 = getTargetDir(f1, false);		

				bool ok = m_fsys->copyFile(fname0, fname1);

				std::string msg;
				Estring::replaceAll(fname0, m_homeDir, ""); //homeDir(���� ��θ� ����)
				Estring::replaceAll(fname1, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(!ok){				
					std::string emsg = "Error, copy, "+ fname0+" to "+ fname1 +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					m_ostr<<"Ok, copy, "<< fname0<<" to "<<fname1<<"\r\n";
					writeStream2Sock();
				}
			}		
		}
		else if(arg1.compare("download")==0){				
			for(;it<argv.end();it++){
				std::string filename = (*it);
				std::string tplfname = getFnameTPLRoot(filename);
				int r = tranFile((char*)tplfname.c_str());
			}			
		}
		else if(arg1.compare("upload")==0){
			while(1){
				std::string head = recvHead();
				if(head.compare("EOJ")==0)
					break;
			
				if(head.find_first_of("ERROR")== 0){//ERROR�� ����
					printf("%s", (char*)head.c_str());
					break;
				}

				std::string key("fname%");			
				std::vector<std::string> strs = Estring::split(head, '%');
				if(strs.size()==3){
					//std::vector<std::string>::iterator it2 = strs.begin();
					std::string fname = m_fsys->getDir();//upload�� fsys�� ����ؾ� �Ѵ�. homedir�� ����(TaskConfig����).. chdir�� ����
					fname.append(strs[1]);
					std::string fsizestr = strs[2];
					int fsize = str2int(fsizestr);
					recvFile((char*)fname.c_str(), fsize);
				}				
			}
		}
		else if(arg1.compare("exit")==0){	
			//m_svrSocket->writeSocket(m_cliSocket, "Bye bye!",8);			
			m_lc->stopAllWorker();
			writeStream2Sock();
			std::string msg = "Bye bye!\r\n";		
			writeMsg2Sock(msg);	
			break;
		}
		else if(arg1.compare("help")==0){
			commandHelp();
		}
		else if(arg1.compare("stopserver")==0){	
			//m_svrSocket->writeSocket(m_cliSocket, "Bye bye!",8);			
			m_lc->stopAllWorker();
			writeStream2Sock();		
			break;
		}		
		else if(arg1.compare("")==0){
			//break;
		}
		else{
			std::string msg = "Invalid command\r\n";
			writeMsg2Sock(msg);				
			//m_svrSocket->writeSocket(m_cliSocket, "Invalid command\r\n", 17);
		}
		
		//LOG_INFO(log, "RequestType:%s ...%s", arg1.c_str(), result.c_str());
		thread_sleep(20);

	}

	//Ŭ���̾�Ʈ ���� �ݱ�
	m_svrSocket->closeClient(m_cliSocket);
	m_isAvailable = true;
	//delete lc;
	printf("Client Connection is closed.\n");
	//getchar();
	m_running = false;
}

*/

void RemoteCLI::init()
{
		//SOCKET cliSock = getRecentClient();
	m_running = true;	

	//std::ostringstream SysFunction;
	//Log log("RemoteCLI", &SysFunction);
	//Log log;
	std::string clisoketStr = int2str(m_cliSocket);	
	m_log.setName("RemoteCLI: "+clisoketStr);
	m_log.setLevel(Log::LOG_LEVEL_INFO, true);
	m_log.setLevel(Log::LOG_LEVEL_ERROR, true);

	//const int BUFSIZE =  255;
	//char buf[BUFSIZE];
	//std::auto_ptr<LocalCLI> lc(new LocalCLI(this, &ostr, m_binder));
	//LocalCLI* lc = new LocalCLI(this, &ostr, m_fsys, m_binder);
	
	//m_homeDir = TaskConfig::getHomeDir();
	m_autoResultPrint = TaskConfig::isAutoResultPrint();
	
	/* Ȩ���丮 ����*/	
	//fsys.open();
	//if(homeDir == "") homeDir = getCurPath();
	//fsys.setDir(homeDir);

	if(TaskConfig::isAutoDeploy()){
		std::string msg = "Auto deploy ...\r\n";
		//m_svrSocket->writeSocket(m_cliSocket, "Auto deploy ...\r\n", 17);		
		writeMsg2Sock(msg);
		m_lc->autoLoad();
		writeStream2Sock();
	}
}

/*
void RemoteCLI::cmdProc()
{
	std::string prompt = "TPL-R> ";
	std::vector<std::string> argv;

	while(1){
		//prompt ����		
		//m_svrSocket->sendMsgWithSize(m_cliSocket, prompt);
		if(m_prompt)writeMsg2Sock(prompt);
		
		//pthread_mutex_lock���� �� �� �κп��� �ϸ� �ȵ�, ���� �Լ����� lock, unlock�� ������

		//m_svrSocket->writeSocket(m_cliSocket, (char*)prompt.c_str(), prompt.size());		

		argv = getArgv();
		std::vector<std::string>::iterator it = argv.begin();
		std::string arg1 = (*it);
		it++;

		bool hascmd = false;
		std::string cmd;
		if(arg1 == "REQ-start"){
			cmd = "Debug-";
			if(argv.size()>1){ //REQ-start�� ������ ��쿡 ���
				hascmd = true;
				cmd.append(*it);
			}
		}
		else
			cmd = arg1;

		LOG_INFO(m_log, "Command: %s", cmd.c_str());

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
			MonitorRequest mr = getMonitorReq(type, *it);	
			procMonitorCmd(m_lc, mr);
		}
		else if(arg1 == "on"){
			std::string obj;
			if(argv.size() == 1){
				std::string msg = "on ('size'|'prompt'): ";
				writeMsg2Sock(msg);				
				obj = getCommand();
			}
			else if(argv.size() == 2){
				obj = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: on ['size'|'prompt']\r\n";				
				writeMsg2Sock(emsg);	
			}
			if(obj.compare("")!=0){
				if(obj == "size") m_withsize = true;
				if(obj == "prompt") m_prompt = true;
			}
		}
		else if(arg1 == "off"){
			std::string obj;
			if(argv.size() == 1){
				std::string msg = "off ('size'|'prompt): ";
				writeMsg2Sock(msg);				
				obj = getCommand();
			}
			else if(argv.size() == 2){
				obj = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: off ['size'|'prompt']\r\n";				
				writeMsg2Sock(emsg);	
			}
			if(obj.compare("")!=0){
				if(obj == "size") m_withsize = false;
				if(obj == "prompt") m_prompt = false;
			}
		}
		else if(arg1 == "resultprint"){
			writeStream2Sock();
		}
		else if(arg1.compare("deploy")==0){
			//printf(">>deploy\n");
			std::string fName = "";
			if(argv.size() ==1){
				std::string msg = "Enter Task file name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task file name: ", 22);				
				writeMsg2Sock(msg);
				fName = getCommand();
			}
			else if(argv.size() == 2){
				fName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: deploy [filename]\r\n";
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);
				writeMsg2Sock(emsg);				
			}
			if(fName.compare("")!=0){
				std::string tplFname = getFnameTPLRoot(fName);
			
				//std::string tn = tplFname;
				//if(tplFname.find_first_of("/")==0){   
				//	tn = tplFname.replace(0, 1, "");
				//}
				//m_lc->loadWorker(tn);
				int result = m_lc->loadWorker(tplFname);		
				if(m_autoResultPrint) writeStream2Sock();
			}
		}
		else if(arg1.compare("undeploy")==0){
			//printf(">>undeloy\n");
			std::string tName = "";
			if(argv.size() == 1){
				std::string msg = "Enter Task name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);				
				writeMsg2Sock(msg);
				tName = getCommand();
			}
			else if(argv.size() == 2){
				tName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: undeploy [taskname]\r\n";				
				writeMsg2Sock(emsg);
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);
			}
			if(tName.compare("")!=0){
				m_lc->unloadWorker(tName);
				if(m_autoResultPrint) writeStream2Sock();
			}
		}
		else if(arg1.compare("run")==0){
			std::string taskName ="";
			if(argv.size() == 1){
				std::string msg = "Enter Task name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);				
				writeMsg2Sock(msg);
				taskName = getCommand();
			}
			else if(argv.size() == 2){
				taskName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: run [taskname]\r\n";
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);				
				writeMsg2Sock(emsg);
			}

			if(taskName.compare("")!=0){
				int result = m_lc->startWorker(taskName, NORMAL); //false�� ������尡 �ƴ��� �ǹ�
				if(m_autoResultPrint) writeStream2Sock();
				if(result==0){//���� ���� �����̸� ����͸��� �� exeTask�� SOCKET���� �Ҵ��ϱ� ���ؼ�
					ExeTask* exeTask = m_lc->getExeTask(taskName);			
				}
			}
		}
		else if(arg1.compare("stop")==0){
			std::string taskName ="";
			if(argv.size() == 1){
				std::string msg = "Enter Task name: ";
				//m_svrSocket->writeSocket(m_cliSocket, "Enter Task name: ", 17);
				writeMsg2Sock(msg);				
				taskName = getCommand();
			}
			else if(argv.size() == 2){
				taskName = (*it);
			}
			else{
				std::string emsg = "Too many arguments. Usage: stop [taskname]\r\n";				
				writeMsg2Sock(emsg);	
				//m_svrSocket->writeSocket(m_cliSocket, (char*)emsg.c_str(), emsg.size()+1);
			}
			if(taskName.compare("")!=0){
				m_lc->stopWorker(taskName);
				DbgTable::clearTable();
				if(m_autoResultPrint) writeStream2Sock();
			}
		}
		else if(arg1.compare("list")==0){
			m_lc->list();
			writeStream2Sock();
		}
		else if(arg1.compare("dir")==0){	
			StringList list;
			bool result = false;
			if(argv.size()>1){
				std::string path0 = (*it);
				std::string path = getTargetDir(path0, true);
				path = Estring::trim(path);				
				result = m_fsys->listFiles(list, path, "*");
			}
			else{
				result = m_fsys->listFiles(list, "*");				
			}
			if(result)
				writeMsgList2Sock(list);
			else
				writeMsg2Sock("Directory path error!\r\n");
		}
		else if(arg1.compare("dirAll")==0){		
			std::string result;
			std::string homedir = getTargetDir("/", true);
			m_fsys->listAllDirNFiles(result, homedir);
			writeMsg2Sock(result);
//			writeMsgList2Sock(result);
		}
		else if(arg1.compare("pwd")==0){
			std::string pwd = m_fsys->getDir();
			Estring::replaceAll(pwd, m_homeDir, ""); //homeDir(���� ��θ� ����)
			pwd.append("\r\n");
			writeMsg2Sock(pwd);
		}
		else if(arg1.compare("home")==0){
			m_fsys->setDir(m_homeDir);
			std::string pwd = m_fsys->getDir();
			Estring::replaceAll(pwd, m_homeDir, ""); //homeDir(���� ��θ� ����)
			pwd.append("\r\n");
			writeMsg2Sock(pwd);
		}
		else if(arg1.compare("exist")==0){					
			if(argv.size()>1){
				std::string fname = (*it);
				fname = Estring::trim(fname);
				std::string fname2 = getTargetDir(fname, false);
				bool result = m_fsys->existsFile(fname2);
				std::string msg = "No";
				if(result) msg = "Yes";
				msg.append("\r\n");
				writeMsg2Sock(msg);
			}		
		}
		else if(arg1.compare("chdir")==0){//������ ���ε� �ϱ����� target ���丮�� �����Ѵ�.
			int i = argv.size();
			if(argv.size()>1){
				std::string dir = (*it);
				dir = Estring::trim(dir);
				std::string dir1 = getTargetDir(dir, true);
				int r = m_fsys->setDir(dir1);	
				
				//������
				std::string result;
				if(r==0){
					result = m_fsys->getDir();
				}
				else{
					result = std::string("Error:202, Not accessible directory");
				}
				Estring::replaceAll(result, m_homeDir, ""); //homeDir(���� ��θ� ����)
				result.append("\r\n");
				writeMsg2Sock(result);
			}
		}
		else if(arg1.compare("mkdir")==0){
			for(;it<argv.end();it++){ //�������� ���丮 ���� ���� mkdir aaa bbb ccc
				std::string dir = (*it);
				dir = Estring::trim(dir);
				std::string targetDir = getTargetDir(dir, true);
				int ok = m_fsys->mkdir(targetDir);
	std::cout<<ok<<std::endl;

				//���
				std::string msg;
				Estring::replaceAll(targetDir, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(ok!=0){				
					std::string emsg = "Error, mkdir, "+ targetDir +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					//ostr<<"Ok, mkdir, "<<targetDir<<"\r\n";
					//writeStream2Sock(ostr);
					writeMsg2Sock("RES-start;msgack;@reqid["+dir+"]@error["+int2str(ok)+"];RES-end");
				}
			}
		}
		else if(arg1.compare("rmdir")==0){
			for(;it<argv.end();it++){ //�������� ���丮 ���� ����rmdir aaa bbb ccc
				std::string dir = (*it);
				dir = Estring::trim(dir);
				std::string targetDir = getTargetDir(dir, true);			
				bool ok = m_fsys->rmdir(targetDir);

				//���
				std::string msg;
				Estring::replaceAll(targetDir, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(!ok){				
					std::string emsg = "Error, rmdir, "+ targetDir +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					m_ostr<<"Ok, rmdir, "<<targetDir<<"\r\n";
					writeStream2Sock();
				}
			}
		}		
		else if(arg1.compare("rm")==0){
			for(;it<argv.end();it++){ 
			
				std::string fname = (*it);
				fname = Estring::trim(fname);				
				std::string fname2 = getTargetDir(fname, false);				
				bool ok = m_fsys->removeFile(fname2);

				//���
				std::string msg;
				Estring::replaceAll(fname2, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(!ok){				
					std::string emsg = "Error, rm, "+ fname2 +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					m_ostr<<"Ok, rm, "<<fname2<<"\r\n";
					writeStream2Sock();
				}
			}		
		}
		else if(arg1.compare("rename")==0){
			if(argv.size()>2){
				std::string f0 = (*it);
				std::string f1 = (*++it);
				f0 = Estring::trim(f0);				
				f1 = Estring::trim(f1);
				std::string fname0 = getTargetDir(f0, false);		
				std::string fname1 = getTargetDir(f1, false);		

				bool ok = m_fsys->renameFile(fname0, fname1);

				std::string msg;
				Estring::replaceAll(fname0, m_homeDir, ""); //homeDir(���� ��θ� ����)
				Estring::replaceAll(fname1, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(!ok){				
					std::string emsg = "Error, rename, "+ fname0+" to "+ fname1 +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					m_ostr<<"Ok, rename, "<< fname0<<" to "<<fname1<<"\r\n";
					writeStream2Sock();
				}
			}		
		}
		else if(arg1.compare("copy")==0){
			if(argv.size()>2){
				std::string f0 = (*it);
				std::string f1 = (*++it);
				f0 = Estring::trim(f0);				
				f1 = Estring::trim(f1);
				std::string fname0 = getTargetDir(f0, false);		
				std::string fname1 = getTargetDir(f1, false);		

				bool ok = m_fsys->copyFile(fname0, fname1);

				std::string msg;
				Estring::replaceAll(fname0, m_homeDir, ""); //homeDir(���� ��θ� ����)
				Estring::replaceAll(fname1, m_homeDir, ""); //homeDir(���� ��θ� ����)
				if(!ok){				
					std::string emsg = "Error, copy, "+ fname0+" to "+ fname1 +"\r\n";
					writeMsg2Sock(emsg);					
				}			
				else{
					m_ostr<<"Ok, copy, "<< fname0<<" to "<<fname1<<"\r\n";
					writeStream2Sock();
				}
			}		
		}
		else if(arg1.compare("download")==0){				
			for(;it<argv.end();it++){
				std::string filename = (*it);
				std::string tplfname = getFnameTPLRoot(filename);
				int r = tranFile((char*)tplfname.c_str());
			}			
		}
		else if(arg1.compare("upload")==0){
			while(1){
				std::string head = recvHead();
				if(head.compare("EOJ")==0)
					break;
			
				if(head.find_first_of("ERROR")== 0){//ERROR�� ����
					printf("%s", (char*)head.c_str());
					break;
				}

				std::string key("fname%");			
				std::vector<std::string> strs = Estring::split(head, '%');
				if(strs.size()==3){
					//std::vector<std::string>::iterator it2 = strs.begin();
					std::string fname = m_fsys->getDir();//upload�� fsys�� ����ؾ� �Ѵ�. homedir�� ����(TaskConfig����).. chdir�� ����
					fname.append(strs[1]);
					std::string fsizestr = strs[2];
					int fsize = str2int(fsizestr);
					recvFile((char*)fname.c_str(), fsize);
				}				
			}
		}
		else if(arg1.compare("exit")==0 ||arg1.compare("disconnected")==0){	
			//m_svrSocket->writeSocket(m_cliSocket, "Bye bye!",8);			
			m_lc->stopAllWorker();
			writeStream2Sock();
			std::string msg = "Bye bye!\r\n";		
			writeMsg2Sock(msg);	
			break;
		}
		else if(arg1.compare("help")==0){
			commandHelp();
		}
		else if(arg1.compare("stopserver")==0){	
			//m_svrSocket->writeSocket(m_cliSocket, "Bye bye!",8);			
			m_lc->stopAllWorker();
			writeStream2Sock();		
			break;
		}		
		else if(arg1.compare("")==0){
			//break;
		}
		else{
			std::string msg = "Invalid command\r\n";
			writeMsg2Sock(msg);				
			//m_svrSocket->writeSocket(m_cliSocket, "Invalid command\r\n", 17);
		}
		
		//LOG_INFO(log, "RequestType:%s ...%s", arg1.c_str(), result.c_str());
		thread_sleep(20);

	}

	//Ŭ���̾�Ʈ ���� �ݱ�
	m_svrSocket->closeClient(m_cliSocket);
	m_isAvailable = true;
	//delete lc;
	printf("Client Connection is closed.\n");
	//getchar();
	m_running = false;
}
*/

OSFileSys* RemoteCLI::getFileSys()
{
	return m_fsys;
}

LocalCLI* RemoteCLI::getLocalCLI()
{
	return m_lc;
}

std::ostringstream* RemoteCLI::ostrStream()
{
	return &m_ostr;
}

void RemoteCLI::setAvailable(bool isAvailable)
{
	m_isAvailable = isAvailable;
}

void RemoteCLI::setPromptOn(bool prompt)
{
	m_prompt = prompt;
}

void RemoteCLI::setMsgSizeOn(bool sizeon)
{
	m_withsize = sizeon;
}

bool RemoteCLI::isPromptOn()
{
	return m_prompt;
}			

Log* RemoteCLI::getLog()
{
	return &m_log;
}