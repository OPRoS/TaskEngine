/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "run/control/PerformTable.h"

std::vector<PeRecord>	PerformTable::s_TimeList;
pthread_mutex_t			PerformTable::s_lmu;
std::ofstream			PerformTable::s_perfile;
unsigned long			PerformTable::seqNum = 0;

PerformTable::PerformTable()
{
}

PerformTable::~PerformTable()
{
}

void PerformTable::init()
{
	pthread_mutex_init(&PerformTable::s_lmu, NULL);
	clearTable();
	seqNum = 0;
}

void PerformTable::clearTable()
{
	pthread_mutex_unlock(&PerformTable::s_lmu);
	s_TimeList.clear();
}

int PerformTable::addPeRecord(PeRecord rec)
{
	pthread_mutex_lock(&PerformTable::s_lmu);
	rec.seq = ++seqNum;
	s_TimeList.push_back(rec);
	pthread_mutex_unlock(&PerformTable::s_lmu);
	return 0;
}

void PerformTable::makeFile(std::string taskname)
{
	if(s_TimeList.size()>0){
		s_perfile.open(taskname.append(".csv").c_str());
		for(unsigned int i =0; i<s_TimeList.size();i++){
			PeRecord tinfo = s_TimeList[i];
			#ifdef _WIN32
			s_perfile<<tinfo.seq<<","<<tinfo.name<<","<<tinfo.cycle * 1000<<","<<tinfo.proc*1000<<std::endl;
			#else
			s_perfile<<tinfo.cycle<<" "<<tinfo.proc<<std::endl;
			#endif
			s_perfile.flush();
		}
		s_perfile.close();
	}
}

