/*
 * OPRoS Component Engine (OCE)
 * Copyright (c) 2008 ETRI. http://www.etri.re.kr.
 * Distributed under the OPRoS License, Version 1.0.
 *
 *  @Created : 2008. 9. 2
 *  @Author  : swjung (swjung@etri.re.kr)
 *
 *  @File    : SysFunction.h
 *
 */

#ifndef OS_FUNC_H_
#define OS_FUNC_H_

#include <string>

class SysFunction
{
public :
	virtual ~SysFunction();
	
	static char fileSeparator();
	static std::string toDir(const std::string &dir);

	static std::string  getFileName(const std::string & fullpath);
	static std::string  getDirName(const std::string & fullpath);
	static std::string  splitExt(const std::string  &filename);
};

#endif /* OS_ERROR_H_ */
