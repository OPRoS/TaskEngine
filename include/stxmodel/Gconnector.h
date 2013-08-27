
#ifndef GCONNECTOR_H_
#define GCONNECTOR_H_

#include "common/antlrIncl.h"
#include "common/Gdatatype.h"
#include "stxmodel/GstmtExpr.h"
#include "stxmodel/GcstBlock.h"
#include "stxmodel/Gparameter.h"
#include <string>

class Gconnector : public GcstBlock
{
private:
	int					m_joinType;
	int					m_synType; //synch, asynch
	int					m_connType;
	EList<Gparameter*>*	m_parameters;	
	GstmtBlock*			m_withs;
	
public:
	Gconnector(std::string file, int eol, int blockid, std::string blockname, std::string blockpath);
	virtual ~Gconnector();
	
	void setJoinType	(int);
	void setRunType		(int type);
	void setConnType	(int type);
	void setParameters	(EList<Gparameter*>*);
	void setWiths		(GstmtBlock*);
	void addParameter	(Gparameter* param);
		
	EList<Gparameter*>*	getParameters();
	GstmtBlock*			getWiths();
	int					getJoinType();
	int					getRunType();
	int					getConnType();
	
	char*	toString(){return NULL;};
	void	print(unsigned int);
};

#endif /* GCONNECTOR_H_ */
