/*
 * Gcontext.h
 *
 *  Created on: 2009. 3. 3
 *      Author: jinseo
 */

#ifndef GCONTEXT_H_
#define GCONTEXT_H_

#include "stxmodel/Gtoken.h"
#include "stxmodel/Grule.h"

class Gcontext
{
private:
	Gtoken*				m_name;
	EList<Grule*>*		m_ruleList;
	EList<Gcontext*>*	m_ctxList;

public:
	Gcontext();
	virtual ~Gcontext();

	void setName(Gtoken*);
	void addRule(Grule*);
	void addContext(Gcontext*);
};

#endif /* GCONTEXT_H_ */
