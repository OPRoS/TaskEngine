/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "run/control/DbgTable.h"

std::map<std::string, DbgInfo> DbgTable::s_dbgList;
std::map<std::string, EmulInfo> DbgTable::s_emulTAB;
std::set<std::string> DbgTable::s_emulTarget;
bool DbgTable::s_hasEmulTarget = false;

DbgTable::DbgTable()
{
}

DbgTable::~DbgTable()
{
}

void DbgTable::init()
{
}

void DbgTable::clearTable()
{
	s_dbgList.clear();
}

int DbgTable::addDbgInfo(DbgInfo info)
{
	//1.key생성
	//std::string key = info.target;
	std::string key = info.file;
	key.append(info.line);

    //2. 테이블에 존재하는지 검사 및 삽입
	std::map<std::string, DbgInfo>::iterator it = s_dbgList.find(key);
	if(it != s_dbgList.end()){
		//이미 존재하면
		return DP_ALREADY_EXIS;
	}
	else{
		s_dbgList.insert(infopair(key, info));
		return 0;
	}
}

int DbgTable::removeDbgInfo(DbgInfo info)
{
	//1.key생성
	//std::string key = info.target;
	std::string key = info.file;
	key.append(info.line);

    //2. 테이블에 존재하는지 검사 및 삽입
	std::map<std::string, DbgInfo>::iterator it = s_dbgList.find(key);
	if(it != s_dbgList.end()){ 	// 존재하면
		s_dbgList.erase(key);
		return 0;
	}
	return DP_NOT_EXIST;
}

bool DbgTable::findByTarget(std::string target, int line)
{
	//1.key생성
	std::string key = target;
	key.append(int2str(line));

	//2. 검사
	std::map<std::string, DbgInfo>::iterator it = s_dbgList.find(key);
	if(it != s_dbgList.end()){
		return true;
	}
	else{
		return false;
	}	
}

bool DbgTable::isBreakTargetBlock(std::string target)
{
	std::map<std::string, DbgInfo>::iterator it;

	for(it=s_dbgList.begin(); it!=s_dbgList.end() ; it++){
		DbgInfo info = it->second;
		if(info.target == target)
			return true;
	}

	return false;
}

bool DbgTable::findByFile(std::string file, int line)
{
	//1.key생성
	std::string key = file;
	key.append(int2str(line));

	//2. 검사
	std::map<std::string, DbgInfo>::iterator it = s_dbgList.find(key);
	if(it != s_dbgList.end()){
		return true;
	}
	else{
		return false;
	}	
}

int DbgTable::updateEmulInfo(std::string target, EmulInfo ei)
{
	std::map<std::string, EmulInfo>::iterator it = s_emulTAB.find(target);
	if(it != s_emulTAB.end())
		it->second = ei;
	else
		s_emulTAB.insert(emulpair(target, ei));
	return 0;
}

EmulInfo DbgTable::getEmulInfo(std::string target)
{
	if(s_emulTAB.size()>0){
		std::map<std::string, EmulInfo>::iterator it = s_emulTAB.find(target);
		if(it != s_emulTAB.end()){
			return it->second;
		}	
	}

	return EmulInfo();
}

bool DbgTable::isEmulTarget(std::string target)
{
	//if(s_emulTAB.size()<1) return false;
	std::set<std::string>::iterator it = s_emulTarget.find(target);
	if(it != s_emulTarget.end()){
		return true;
	}
	else{
		return false;
	}	
}

int	DbgTable::updateEmulTarget(std::string on, std::string off)
{
	std::vector<std::string> onlist = Estring::split(on, ",");
	std::vector<std::string> offlist = Estring::split(off, ",");

	for(unsigned int i =0;i<offlist.size();i++){
		std::set<std::string>::iterator iter = s_emulTarget.find(offlist[i]);
		if(iter!=s_emulTarget.end()){
			s_emulTarget.erase(offlist[i]);
			std::map<std::string, EmulInfo>::iterator it = s_emulTAB.find(offlist[i]);
			if(it != s_emulTAB.end())
				s_emulTAB.erase(it);
		}
	}
	for(unsigned int i =0;i<onlist.size();i++){
		std::set<std::string>::iterator iter = s_emulTarget.find(onlist[i]);
		if(iter==s_emulTarget.end()){
			s_emulTarget.insert(onlist[i]);
		}
	}

	if(s_emulTarget.size()>0) 
		s_hasEmulTarget = true;
	else 
		s_hasEmulTarget = false;

	return 0;
}