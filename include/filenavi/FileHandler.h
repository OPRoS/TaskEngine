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

#ifndef _OS_FILE_H_
#define _OS_FILE_H_

#include <cstdio>
#include <list>
#include <string>

#define FILE_IO_MAX_TRIAL 5

typedef FILE *OSFileHandle;

class FileHandler
{
	OSFileHandle m_handle;
	bool m_isClosed;

	std::string  m_filename;
	std::string  m_dir;
	std::string  m_flag;

public:
	FileHandler() : m_handle(NULL), m_isClosed(false) {};
	FileHandler(OSFileHandle handle) : m_handle(handle), m_isClosed(false) {};
	virtual ~FileHandler() {
		if (m_handle != NULL && m_isClosed == false) close();
	}

	virtual bool open(const std::string  &filename, std::string  &mode);
	virtual bool close(void) ;

	virtual unsigned long read(unsigned char *data, unsigned long len);
	virtual unsigned long write(unsigned char *data, unsigned long len);
	virtual unsigned long sizeOf(void);

	void setHandle(OSFileHandle handle) { m_handle = handle; };
	OSFileHandle getHandle(OSFileHandle handle) { return m_handle; };

	std::string  getFileName(void) { return m_filename; };
	void setFileName(const std::string  &fn) { m_filename = fn; };

	std::string  getDir(void) { return m_dir; };
	void setDir(const std::string  &dir) { m_dir = dir; };

	OSFileHandle getFileHandle(void) { return m_handle; };
	void setFileHandle(OSFileHandle handle) { m_handle=handle; };

	void setFlag(const std::string  &flag) { m_flag = flag; };
	std::string  getFlag() { return m_flag; };
};

typedef std::list<FileHandler> OSFileList;

#endif /* _OS_FILE_SYSTEM_H_ */
