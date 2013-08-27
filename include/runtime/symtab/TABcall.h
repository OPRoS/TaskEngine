#pragma once

#include "TABparameter.h"
#include "runtime/rtmodel/RFSMnamedItem.h"
#include "common/Gdatatype.h"

class TABcall  : public RFSMnamedItem
{

private:
	EList<TABparameter*>* m_params;
	
public:
	TABcall(std::string, EList<TABparameter*>*);
	virtual ~TABcall(void);

	EList<TABparameter*>* getParameters();
	void addParameter(TABparameter*);
	void setParameters(EList<TABparameter*>*);

	TABparameter* getParameter(std::string name);

};
