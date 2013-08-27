/*
 * OPRoS Component Engine (OCE)
 * Copyright (c) 2008 ETRI. http://www.etri.re.kr.
 * Distributed under the OPRoS License, Version 1.0.
 *
 *  @Created : 2008. 10. 30
 *  @Author  : swjung (swjung@etri.re.kr)
 *
 *  @File    : OSFileSys.h
 *
 */

#ifndef _OS_FILE_SYSTEM_H_
#define _OS_FILE_SYSTEM_H_

#include <cstdio>
#include "filenavi/FileHandler.h"
#include "util/Utility.h"
#include <vector>
#include <string>

typedef std::vector<std::string> StringList;

// OSFileSys
class OSFileSys {
protected:
	std::string		m_dir;
	bool			m_opened;

private:
	std::string  dirnameValidCharacters;
	std::string  patternValidCharacters;

	bool isAbsolute(const std::string  &dir);
	bool containCharInString(char c, const std::string  &str);
	bool checkIfValidAndAbsolutePattern(const std::string  &pattern);
	bool checkIfValidAndAbsoluteDirname(const std::string  &dirname);
	bool validateDirname(const std::string  &dirname, const std::string  &validChar);
	bool validateFirstCharIsNotHyphen(const std::string  &dirname);
	bool openFileWithFlag(FileHandler &file, const std::string  &filenane, const std::string  &flag);
	bool addFileDir(std::string  &absoluteDir, const std::string  &dir);

public:
	OSFileSys();
	//OSFileSys(const std::string  &dir);
	virtual ~OSFileSys();

	bool open();
	void close();
	bool isOpen() { return m_opened; };

	int setDir(const std::string  &dir);
	std::string  getDir();
	std::string  toDir(const std::string  &dir);

	virtual bool createFile(FileHandler &file, const std::string  &fileName);
	virtual bool openFile(FileHandler &file, const std::string  &fileName, const  std::string  &flag);

	virtual bool renameFile(const std::string  &oldname, const std::string  &newname);
	virtual bool removeFile(const std::string  &filename);
	virtual bool copyFile(const std::string  &sourceFileName, const std::string  &destinationFileName);
	virtual bool existsFile(const std::string  &filename);

	virtual bool listFiles(StringList &listArray, const std::string  &pattern);
	virtual bool listAllDirNFiles(std::string &allelements, std::string dir);
	virtual bool listFiles(StringList &listArray, const std::string  &dir, const std::string  &pattern);

	virtual bool findFiles(StringList &listArray, const std::string  &pattern,  bool recursive = false);
	virtual bool findFiles(StringList &listArray, const std::string  &dir, const std::string  &pattern, bool recursive = false);

	virtual bool mkdir(const std::string  &directoryName);
	virtual bool rmdir(const std::string  &directoryName);
};


#endif /* _OS_FILE_SYSTEM_H_ */
