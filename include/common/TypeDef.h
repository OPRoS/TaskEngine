#include "runtime/rtmodel/RFSMvalueEval.h"

typedef std::map<std::string, std::string>		StrMap;
typedef std::pair<std::string, std::string>		StrPair;

typedef std::pair<std::string, RFSMvalueEval>	ValuePair;
typedef std::map<std::string, RFSMvalueEval>	ValueMap;
typedef std::map<std::string, int>				ValueLineMap;
typedef std::pair<std::string, int>				ValueLinePair;
//typedef std::vector<int>						SynchList; //runtime에서는 RFSMconnector사용 synch id를 유지
                                                           //syntax에서는 
//typedef std::vector<std::string>				SynchStmtLocTAB;//100 + "bhv1.s1.stay" 나중에 validation을 위해 사용
