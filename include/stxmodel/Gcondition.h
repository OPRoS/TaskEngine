/*
 * Gcondition.h
 *
 *  Created on: 2009. 3. 2
 *      Author: jinseo
 */

#ifndef GCONDITION_H_
#define GCONDITION_H_

#include "stxmodel/GstmtExpr.h"
#include "common/EXTree.h"

class Gcondition: public Gstmt
{
private:
	GstmtExpr* 	m_cond;

	bool iterDNF(EXTree* parent, EXTree* child, std::string blockpath);

public:
	Gcondition(std::string, int, std::string, std::string);
	virtual ~Gcondition();

	void setCondition(GstmtExpr*);
	GstmtExpr* getCondition();
	void toDNF(std::string);
	void print(unsigned int);

	char* toString(){return NULL;};
};

#endif /* GCONDITION_H_ */
