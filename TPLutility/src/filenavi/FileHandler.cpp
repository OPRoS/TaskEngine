/*
 * OPRoS Component Engine (OCE)
 * Copyright (c) 2008 ETRI. http://www.etri.re.kr.
 * Distributed under the OPRoS License, Version 1.0.
 *
 *  @Created : 2008. 10. 30
 *  @Author  : swjung (swjung@etri.re.kr)
 *
 *  @File    : FileHandler.cpp
 *
 */

#include "filenavi/SysFunction.h"
#include "filenavi/FileHandler.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

bool FileHandler::open(const string &filename, string &mode)
{
	//open file
	m_handle = ::fopen(filename.c_str(), mode.c_str());
	if( m_handle == NULL) return false;

	m_filename = filename;
	m_dir = filename;
	m_flag = mode;

	return true;
}

bool FileHandler::close(void)
{
	if (m_handle != NULL) {
		::fclose(m_handle);
		m_handle = NULL;
		m_isClosed = true;
	}

	return true;
}

unsigned long FileHandler::read(unsigned char *data, unsigned long len)
{
	unsigned int ret = 0;
	unsigned int trial = 0;

	while (ret < len && trial <= FILE_IO_MAX_TRIAL) {
		ret += ::fread(data + ret, 1, len - ret, m_handle);
		++trial;
	}
	return ret;
}

unsigned long FileHandler::write(unsigned char *data, unsigned long len)
{
	unsigned int trial = 0;
	unsigned int ret = 0;

	while (ret < len && trial <= FILE_IO_MAX_TRIAL) {
		ret += ::fwrite(data + ret, 1, len - ret, m_handle);
		++trial;
	}

	return ret;
}
unsigned long FileHandler::sizeOf(void)
{
	//get file size
	unsigned int cur = ::fseek(m_handle , 0, SEEK_CUR);
	unsigned int fileSize = ::fseek(m_handle , 0, SEEK_END);
	::fseek(m_handle, cur, SEEK_SET);

	return fileSize;
}

