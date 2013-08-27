#ifndef DBGTABLE_H_
#define DBGTABLE_H_

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "ds/Estring.h"
#include "util/Utility.h"

//����� ���� ����
#define DP_ALREADY_EXIS			100
#define DP_NOT_EXIST			102
#define WRONG_TH_ID				103

class DbgInfo
{
public:
	//file+line���� �����ϰų� target+line���� ���� �ƴϸ� ��δ�.
	std::string file;   //examples/test.tpl
	std::string target; //greeting.s1.entry
	std::string line;
};

class EmulInfo
{
public:
	std::string		etype;	//value, randomD_U, randomF_U, randomD_G, randomD_F, enum, notinited
	std::string		value;	//valueŸ���� ���� 
	double			min;	//random�� ��
	double			max;	//random�� ��
	std::vector<std::string> enumlist; // enum�� ����
};

class DbgTable
{
private:
	//static std::vector<DbgInfo> DbgList;
	static	std::map<std::string, DbgInfo>	s_dbgList;
	typedef std::pair<std::string, DbgInfo> infopair;

	static std::map<std::string, EmulInfo> s_emulTAB;
	static std::set<std::string> s_emulTarget;
	typedef std::pair<std::string, EmulInfo> emulpair;

public:
	static	bool s_hasEmulTarget;

	DbgTable();
	virtual ~DbgTable();

	void init(void);
	
	static void	clearTable(void);
	static int	addDbgInfo(DbgInfo);
	static int	updateEmulInfo(std::string, EmulInfo);	
	static int	removeDbgInfo(DbgInfo);
	static bool findByTarget(std::string target, int line);
	static bool findByFile(std::string file, int line);
	static bool isBreakTargetBlock(std::string target);
	static bool isEmulTarget(std::string target);
	static int	updateEmulTarget(std::string, std::string);
	static EmulInfo getEmulInfo(std::string target);
};

#endif /* DBGTABLE_H_ */
