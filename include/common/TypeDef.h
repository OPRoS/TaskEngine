#include "runtime/rtmodel/RFSMvalueEval.h"

typedef std::map<std::string, std::string>		StrMap;
typedef std::pair<std::string, std::string>		StrPair;

typedef std::pair<std::string, RFSMvalueEval>	ValuePair;
typedef std::map<std::string, RFSMvalueEval>	ValueMap;
typedef std::map<std::string, int>				ValueLineMap;
typedef std::pair<std::string, int>				ValueLinePair;
//typedef std::vector<int>						SynchList; //runtime������ RFSMconnector��� synch id�� ����
                                                           //syntax������ 
//typedef std::vector<std::string>				SynchStmtLocTAB;//100 + "bhv1.s1.stay" ���߿� validation�� ���� ���
