#ifndef PERFORMTABLE_H_
#define PERFORMTABLE_H_

extern "C"{
#include "thread/ThreadDef.h"
}

#include <string>
#include <iostream>
#include <vector>
#include "ds/Estring.h"
#include "util/Utility.h"


class PeRecord{
public:
	unsigned long	seq;
	std::string		name;
	double			cycle;
	double			proc;
};

class PerformTable
{
private:
	static std::vector<PeRecord>	s_TimeList;
	static pthread_mutex_t			s_lmu;
	static std::ofstream			s_perfile;

	static unsigned long			seqNum;

public:
	PerformTable();
	virtual ~PerformTable();

	static void init(void);
	static void	clearTable(void);
	static int	addPeRecord(PeRecord);
	static void makeFile(std::string);
};

#endif /* PERFORMTABLE_H_ */
