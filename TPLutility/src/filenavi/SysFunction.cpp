/*
 * OPRoS Component Engine (OCE)
 * Copyright (c) 2008 ETRI. http://www.etri.re.kr.
 * Distributed under the OPRoS License, Version 1.0.
 *
 *  @Created : 2008. 9. 2
 *  @Author  : swjung (swjung@etri.re.kr)
 *
 *  @File    : SysFunction.cpp
 *
 */

#include "filenavi/SysFunction.h"

using namespace std;

#ifdef _WIN32

char SysFunction::fileSeparator() {
	//return  '\\';
	return '/';
}

#else // OS_LINUX

#include <errno.h>
#include <unistd.h>

char SysFunction::fileSeparator() {
	return  '/';
}

#endif


string SysFunction::toDir(const string &path)
{
	string dir = "";

	if (path.size() > 0) {
		int idx = path.size()-1;
		for (; idx >= 0; --idx) {
			if (path[idx] != ' ') break;
		}
		if (idx >= 0) {
			dir = path.substr(0, idx+1);
			if (dir[idx] != SysFunction::fileSeparator()) {
				dir += SysFunction::fileSeparator();
			}
		}
	}

	return dir;
}

string SysFunction::getFileName(const string& fullpath)
{
	size_t found;
	found = fullpath.find_last_of(SysFunction::fileSeparator());
	if (found != string::npos) {
		return fullpath.substr(found+1);
	}
	return fullpath;
}

string SysFunction::getDirName(const string& fullpath)
{
	size_t found;
	found = fullpath.find_last_of(SysFunction::fileSeparator());
	if (found != string::npos) {
		return fullpath.substr(0, found);
	}
	return "";
}

string SysFunction::splitExt(const string &filename)
{
	string fname = SysFunction::getFileName(filename);

	unsigned int idx = fname.find_last_of('.');
	if (idx != string::npos) return fname.substr(0, idx);
	else return fname;
}

