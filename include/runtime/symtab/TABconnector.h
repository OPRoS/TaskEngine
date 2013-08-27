#pragma once

#include "TABparameter.h"
#include "TABcall.h"

class TABconnector : public TABcall
{

public:
	TABconnector(std::string, EList<TABparameter*>*);
	virtual ~TABconnector(void);
};

