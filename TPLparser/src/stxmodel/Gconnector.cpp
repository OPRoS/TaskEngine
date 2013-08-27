#include "stxmodel/Gconnector.h"

Gconnector::Gconnector(std::string file, int eol, int blockid, std::string blockname, std::string blockpath): GcstBlock(file, eol, blockid, blockname, blockpath)
{
	m_parameters	= NULL;	
	m_synType		= ASYNCHRO; //default
}

Gconnector::~Gconnector()
{
	if(m_parameters != NULL){
		while(m_parameters->moveHead()){
			Gparameter* obj = m_parameters->getCurObject();
			delete obj;
			m_parameters->delHead();
		}
		delete m_parameters;
		m_parameters = NULL;
	}
	

	if(m_withs != NULL){
		delete m_withs;
		m_withs = NULL;
	}
}

void Gconnector::setJoinType(int type)
{
	m_joinType = type;
}

void Gconnector::setRunType(int type)
{
	m_synType = type;
}

void Gconnector::setConnType(int type)
{
	m_connType = type;
}

int Gconnector::getJoinType()
{
	return m_joinType;
}

int Gconnector::getRunType()
{
	return m_synType;
}

int Gconnector::getConnType()
{
	return m_connType;
}

void Gconnector::print(unsigned int indent)
{
}

void Gconnector::setParameters(EList<Gparameter*>* params)
{
	m_parameters = params;
}

void Gconnector::setWiths(GstmtBlock* withs)
{
	m_withs = withs;
}

void Gconnector::addParameter(Gparameter* param)
{
	if(m_parameters == NULL)
		m_parameters = new EList<Gparameter*>();

	m_parameters->addTail(param);
}

EList<Gparameter*>* Gconnector::getParameters()
{
	return m_parameters;
}

GstmtBlock* Gconnector::getWiths()
{
	return m_withs;
}