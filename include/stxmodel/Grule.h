/*
 * Gcontext.h
 *
 *  Created on: 2009. 3. 1
 *      Author: jinseo
 */

#ifndef GRULE_H_
#define GRULE_H_

#include "stxmodel/Gtoken.h"
#include "stxmodel/Gcondition.h"

class Grule : public Gcondition
{
private:
	Gtoken*	m_name;

public:
	Grule(std::string file, int eol, std::string blockname, std::string blockpath);
	virtual ~Grule();

	void setName(Gtoken*);

	char* toString(){return NULL;};
	void print(unsigned int);
};

#endif /* GRULE_H_ */
