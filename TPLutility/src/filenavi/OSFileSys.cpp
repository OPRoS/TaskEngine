/*
 * OPRoS Component Engine (OCE)
 * Copyright (c) 2008 ETRI. http://www.etri.re.kr.
 * Distributed under the OPRoS License, Version 1.0.
 *
 *  @Created : 2008. 10. 30
 *  @Author  : swjung (swjung@etri.re.kr)
 *
 *  @File    : OSFileSys.cpp
 *
 */

#include "filenavi/OSFileSys.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#else

#include <glob.h>
#include <unistd.h>
#define _access access
#define _S_IFDIR S_IFDIR
#define _rmdir rmdir

#endif

#include "filenavi/SysFunction.h"

using namespace std;

OSFileSys::OSFileSys()
{
	m_opened = false;
}

/*
OSFileSys::OSFileSys(const string &dir)
{
	m_opened = false;
	setDir(dir);
}
*/

OSFileSys:: ~OSFileSys()
{
}

int OSFileSys::setDir(const string &dir)
{
	int mode = isAccessible(dir);
	if(mode != 0){
		return -1;
	}

	if (dir.size() > 0) m_dir = SysFunction::toDir(dir); //디렉토리 끝에 win '\' linux '/'가 붙는다	
	return 0;
}

string OSFileSys::getDir()
{
	return m_dir;
}


bool OSFileSys::open()
{
	dirnameValidCharacters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	dirnameValidCharacters += "abcdefghijklmnopqrstuvwxyz";
	dirnameValidCharacters += "~!@#$%^&*()-=_+,./<>?;':\"[]{}|\\";
	patternValidCharacters = dirnameValidCharacters + "*?";
	//patternValidCharacters = dirnameValidCharacters;

	//Directory test
	/*
	struct stat statbuf;
	if( stat(getDir().c_str() , &statbuf) < 0) {
		string msg = "<OSFileSys::open> lstat failed for " + getDir() ;
		//trace_error(msg);
		return false;
	}
	else {
		if( S_ISDIR( statbuf.st_mode ) == 0 ) {
			string msg = "<OSFileSys::open> " + getDir() + " is not a directory.";
			//trace_error(msg);
			return false;
		}
	}
	*/

	m_opened = true;

	return true;
}

void OSFileSys::close()
{
}

bool OSFileSys::renameFile(const string &oldname, const string &newname)
{
	int result= ::rename(oldname.c_str() , newname.c_str());
	if ( result == 0 ) return true;
	else return false;
}

bool OSFileSys::removeFile(const string &filename)
{
	//trace("<OSFileSys::remove> Removing " << filename);
/*
	if(filename == "") return true;
	if(!existsFile(filename)) return true;

	string fileDir;
	if (!addFileDir(fileDir, filename)) return false;
*/
	//trace("<OSFileSys::remove> File dir is " << fileDir);

	std::string fileDir = filename;
	if(::remove(fileDir.c_str()) < 0) return false;

	return true;
}

bool OSFileSys::copyFile(const string &sourceFileName, const string &destinationFileName)
{
	//trace("<OSFileSys::copy> Copying file from " << sourceFileName << " to " << destinationFileName);

	//test for existence of source file
	if( !existsFile( sourceFileName ) ) {
		string msg = "<OSFileSys::copy> Source file " + sourceFileName + " doesn't exist.";
		//trace_error(msg);
		return false;
	}

	/*
	이부분을 사용하지 않으면 파일명은 절대경로로 입력되어야 한다.
	//add absolute dir
	string source;
	if (!addFileDir(source, sourceFileName)) return false;

	string destination;
	if (!addFileDir(destination, destinationFileName)) return false;
   */
	FILE *fold, *fnew;
	int c;

	// open source file with binary mode
	if( ( fold = fopen( sourceFileName.c_str(), "rb") ) == NULL ) {
		string msg = "<OSFileSys::copy> Couldn't open source file " + sourceFileName;
		//trace_error(msg);
		return false;
	}

	// open destination file with binary mode
	if( ( fnew = fopen( destinationFileName.c_str(), "wb") ) == NULL ) {
		fclose( fold );

		string msg = "<OSFileSys::copy> Couldn't open  destination file " + destinationFileName;
		//trace_error(msg);
		return false;
	}

	// read one byte from source file at once.
	// until meet the end of file
	while(1) {
		c = fgetc( fold );
		if( !feof( fold ) ) fputc( c, fnew );
		else break;
	}

	fclose( fnew );
	fclose( fold );

	return true;
}

bool OSFileSys::existsFile(const string &filename)
{
	StringList listArray;

	//trace("<OSFileSys::exists> Checking for existence of " << filename);

	if( filename == "" ) {
		string msg = "<OSFileSys::exists> Invalid file name.";
		//trace_error(msg);
		return false;
	}

	/* 이 부분 사용안하기 때문에 입력되는 파일명은 절대 경로이어야 한다.
	//add absolute dir
	string fileDir;
	if (!addFileDir(fileDir, filename)) return false;
	*/


	//test for existence of source file
	// 00 : Existence only, 02 : Write-only, 04 : Read-only, 06 : Read and write
	if( _access( filename.c_str() , 0 ) < 0 ) return false;
	else return true;
}

bool OSFileSys::listFiles(StringList &listArray, const string &pattern)
{

	//if (!checkIfValidAndAbsolutePattern(pattern)) return false;

	//trace("<OSFileSys::findFiles> Listing files which match the pattern " << pattern);

	string wildcardPattern;

	//Does the pattern include a wildcard?
	string fileDir = "";
	bool patternWithWildcards = (pattern.find( '*' ) != pattern.npos)
						|| (pattern.find( '?' ) != pattern.npos);
	if( !patternWithWildcards ) {
		//trace("<OSFileSys:listFiles> no wild cards!!");
		//add absolute dir
		if (!addFileDir(fileDir, pattern)) return false;

		//is it directory?
		struct stat statbuf;
		if( stat( fileDir.c_str(), &statbuf ) < 0 ) {
			string msg = "<OSFileSys:listFiles> lstat failed for " + fileDir;
			//trace_error(msg);
			return false;
		}
		if(statbuf.st_mode == _S_IFDIR) {
			//trace("<OSFileSys:listFiles> directory!!");
			//add absolute dir
			fileDir = SysFunction::toDir(fileDir);
			wildcardPattern = "*";
		}
	}
	else {
		fileDir = getDir();
		wildcardPattern = pattern;
	}

	return listFiles(listArray, fileDir, pattern);
}

bool OSFileSys::listAllDirNFiles(std::string &allelements, std::string dir)
{
	//root "./"
	StringList listArray;
	listFiles(listArray, dir, "*");

	StringList::iterator it;	
	for (it=listArray.begin() ; it < listArray.end(); it++){	
		std::string curElem = (*it);
		//1. result에 내용 삽입
		allelements = allelements.append(curElem);

		//2. 서브 디렉토리인지 확인
		size_t found1 = curElem.find_first_of("[");
		size_t found2 = curElem.find_first_of("]");
		if(found1==0 && found2==curElem.size()-1){ //'['로 시작

			std::string subdirname = curElem.substr(found1+1, found2-1);
			std::string targetDir = dir;
			targetDir.append(subdirname).append("/");		

			allelements.append("{");
			listAllDirNFiles(allelements, targetDir);
			allelements.append("}");
		}

		if(it < listArray.end()-1)
			allelements = allelements.append(",");
	}

	return true;
}

bool OSFileSys::listFiles(StringList &listArray, const string &dir, const string &pattern)
{
	string fileDir = dir + pattern;

	//trace("<FileSystem:listFiles> list pattern : " << fileDir);

#ifdef _WIN32
	WIN32_FIND_DATAA wfd = {0};

	HANDLE hFind = FindFirstFileA(fileDir.c_str(), &wfd);
	if(INVALID_HANDLE_VALUE == hFind) {
		return false;
	}

    BOOL bFound =  TRUE;
	while(bFound) {
		if (strcmp((const char *)wfd.cFileName, ".") == 0 || strcmp((const char *)wfd.cFileName, "..") == 0) {
		    bFound =  FindNextFileA(hFind, &wfd);
			continue;
		}

		if (wfd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
			string filename((const char *)wfd.cFileName);
			listArray.push_back(filename);
			//trace("<FileSystem:listFiles> adding files : " << wfd.cFileName);
        }
		else{
			string dirname("[");
			dirname.append((const char *)wfd.cFileName);
			dirname.append("]");
			listArray.push_back(dirname);
		}

	    bFound =  FindNextFileA(hFind, &wfd);
	}
	FindClose(hFind);

#else

	int ret;
	int num;

	glob_t globbuf;
	globbuf.gl_pathc = 0;
	globbuf.gl_pathv = NULL;
	globbuf.gl_offs = 0;
	ret = glob( fileDir.c_str(), GLOB_DOOFFS | GLOB_MARK, NULL, &globbuf );

	if( ret != 0 && ret != GLOB_NOMATCH) {
		string msg = "<OSFileSys::listFiles> glob failed for " + fileDir;
		//trace_error(msg);
		return false;
	}

	if(ret == GLOB_NOMATCH) num = 0;
	else num = globbuf.gl_pathc;

	StringList temp;
	for( int i = 0; i < num; i++ ) {
		string tempListArray = (char*)((globbuf.gl_pathv)[i]);
		string aa = tempListArray.substr(dir.size());
		temp.push_back(aa);
		//trace("<FileSystem:listFiles> " << (i+1) << " : " << tempListArray);
	}

	for( int i = 0; i < num; i++ ) {
		if (temp[i].at(temp[i].size()-1) == '/') {
			std::string dn;
			dn.append("[");
			dn.append(temp[i].substr(0, temp[i].size()-1));
			dn.append("]");
			listArray.push_back(dn);
		}
		else {
			listArray.push_back(temp[i]);
		}
	}

	//frees the dynamically allocated storage for glob
	globfree( &globbuf );

#endif

	return true;
}

bool OSFileSys::findFiles(StringList &listArray, const string &pattern, bool recursive)
{
	if (!checkIfValidAndAbsolutePattern(pattern)) return false;

	//trace("<OSFileSys::findFiles> Listing files which match the pattern " << pattern);

	string wildcardPattern;

	//Does the pattern include a wildcard?
	string fileDir = "";
	bool patternWithWildcards = (pattern.find( '*' ) != pattern.npos)
						|| (pattern.find( '?' ) != pattern.npos);
	if( !patternWithWildcards ) {
		//trace("<OSFileSys:findFiles> no wild cards!!");
		//add absolute dir
		if (!addFileDir(fileDir, pattern)) return false;

		//is it directory?
		struct stat statbuf;
		if( stat( fileDir.c_str(), &statbuf ) < 0 ) {
			string msg = "<OSFileSys:findFiles> lstat failed for " + fileDir;
			//trace_error(msg);
			return false;
		}
		if(statbuf.st_mode == _S_IFDIR) {
			//trace("<OSFileSys:findFiles> directory!!");
			//add absolute dir
			fileDir = SysFunction::toDir(fileDir);
			wildcardPattern = "*";
		}
	}
	else {
		fileDir = getDir();
		wildcardPattern = pattern;
	}

	return findFiles(listArray, fileDir, pattern, recursive);
}

bool OSFileSys::findFiles(StringList &listArray, const string &dir, const string &pattern, bool recursive)
{
	string fileDir = dir + pattern;

	//trace("<FileSystem:findFiles> list pattern : " << fileDir);

#ifdef _WIN32
	WIN32_FIND_DATA wfd = {0};

#ifdef  UNICODE
	HANDLE hFind = FindFirstFile((LPCWSTR) fileDir.c_str(), &wfd);
#else
	HANDLE hFind = FindFirstFile(fileDir.c_str(), &wfd);
#endif


	if(INVALID_HANDLE_VALUE == hFind) return false;

    BOOL bFound =  TRUE;
	while(bFound) {
		if (strcmp((const char *)wfd.cFileName, ".") == 0 || strcmp((const char *)wfd.cFileName, "..") == 0) {
		    bFound =  FindNextFile(hFind, &wfd);
			continue;
		}

		if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
			//trace("<FileSystem:findFiles> finding directory : " << " : " << wfd.cFileName);

			if (recursive) {
				string filename((const char *)wfd.cFileName);
				string fullDir = SysFunction::toDir(dir + filename);
				findFiles(listArray, fullDir, pattern, recursive);
			}
         }


		string filename((const char *)wfd.cFileName);
		string tmp = dir + filename;
		listArray.push_back(tmp);
		//trace("<FileSystem:findFiles> adding files : " << tmp);

	    bFound =  FindNextFile(hFind, &wfd);
	}
	FindClose(hFind);

#else

	int ret;
	int num;

	glob_t globbuf;
	globbuf.gl_pathc = 0;
	globbuf.gl_pathv = NULL;
	globbuf.gl_offs = 0;
	ret = glob( fileDir.c_str(), GLOB_DOOFFS | GLOB_MARK, NULL, &globbuf );

	if( ret != 0 && ret != GLOB_NOMATCH) {
		string msg = "<OSFileSys::findFiles> glob failed for " + fileDir;
		//trace_error(msg);
		return false;
	}

	if(ret == GLOB_NOMATCH) num = 0;
	else num = globbuf.gl_pathc;

	for( int i = 0; i < num; i++ ) {
		string tmp = dir + (char*)((globbuf.gl_pathv)[i]);
		listArray.push_back(tmp);
		//trace("<FileSystem:findFiles> " << (i+1) << " : " << tmp);
	}

	//frees the dynamically allocated storage for glob
	globfree( &globbuf );

#endif

	return true;
}

bool OSFileSys::createFile(FileHandler &file, const string &filename)
{
	//fileName is valid?
	checkIfValidAndAbsoluteDirname( filename );

	//trace("<OSFileSys::create> Creating file  " << filename);

	if( existsFile( filename ) ) {
		string msg = "<OSFileSys::create> File " + filename + " already exist.";
		//trace_error(msg);
		return false;
	}

	return openFileWithFlag( file, filename , "wb+");
}

bool OSFileSys::openFile(FileHandler &file, const string &filename, const string &flag)
{
	//fileName is valid?
	checkIfValidAndAbsoluteDirname( filename );

	//trace("<OSFileSys::openFile> Opening file " << filename);

	if( !existsFile( filename ) ) {
		string msg = "<OSFileSys::open> File " + filename + " doesn't exist.";
		//trace_error(msg);
		return false;
	}

	return openFileWithFlag(file, filename, flag);
}

bool OSFileSys::openFileWithFlag(FileHandler &file, const string &filename, const string &flag)
{
	try {
		//add absolute dir
		string fileDir;
		if (!addFileDir(fileDir, filename)) return false;

		//trace("<OSFileSys::openFileWithFlag] Opening file " << fileDir);
		file.setFileName(filename);
		file.setDir(fileDir);
		file.setFlag(flag);

		FILE *fh = ::fopen(fileDir.c_str(), flag.c_str());
		file.setHandle(fh);
		if (fh == NULL) {
			return false;
		}
		return true;
	}
	catch(...) {
		string msg = "<OSFileSys::openSCAFile> Exception occured.";
		//trace_error(msg);
		return false;
	}

	return false;
}

bool OSFileSys::mkdir(const string &directoryName)
{
	/*
	//trace("<OSFileSys::mkdir> Creating directory " << directoryName);

	if( directoryName == "" ) {
		string msg = "<ERROR-OSFileSys::mkdir> Invalid directory name " + directoryName;
		//trace_error(msg);
		return false;
	}

	if(existsFile(directoryName)) {
		string msg = "<ERROR-OSFileSys::mkdir> Directory " +  directoryName + " already exists";
		//trace_error(msg);
		return false;
	}

	//add absolute dir
	string dirName;
	if (!addFileDir(dirName, directoryName)) return false;

	//make directory
	//read/write/search permissions for owner and group, and with read/search permissions for others.

*/
	std::string dirName = directoryName;
	int status = 0;

#ifdef _WIN32
	status = ::_mkdir(dirName.c_str());
#else
	status = ::mkdir( dirName.c_str() , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
#endif

	if( status < 0 ) {
		string msg = "<ERROR-OSFileSys::mkdir> mkdir for " + directoryName + " failed";
		//trace_error(msg);
		//return false;
	}

	return status;
}

bool OSFileSys::rmdir(const string &directoryName)
{
	//trace("<OSFileSys::rmdir> Removing directory " << directoryName);

	/*
	if( directoryName == "" ) {
		string msg = "<OSFileSys::rmdir> Invalid directory name " + directoryName;
		//trace_error(msg);
		return false;
	}

	 //add absolute dir
	string dirName;
	if (!addFileDir(dirName, directoryName)) return false;

	struct stat statbuf;
	if( stat(dirName.c_str() , &statbuf) < 0 ) {
		string msg = "<OSFileSys::rmdir> lstat failed for " + directoryName;
		//trace_error(msg);
		return false;
	}
	if(statbuf.st_mode == _S_IFDIR) {
		string msg = "<OSFileSys::rmdir> " + directoryName + " is not a directory";
		//trace_error(msg);
		return false;
	}
	*/

	//TODO
	// rm -r recursive??

	//remove directory
	std::string dirName = directoryName;
	int status = _rmdir(dirName.c_str()) ;

	if( status < 0 ) {
		string msg = "<OSFileSys::rmdir> rmdir for " + directoryName + " failed. ";
		msg += "Native filename was " + dirName;
		//trace_error(msg);
		return false;
	}

	return true;
}


bool OSFileSys::addFileDir(string &absoluteDir, const string &dir)
{
	// test for absolute dir?
	if (!checkIfValidAndAbsoluteDirname( dir )) return false;

	// concaternate
	absoluteDir = getDir() + dir;

	return true;
}

// return true if the dirname dir is absolute
bool OSFileSys::isAbsolute(const string &dir)
{
#ifdef _WIN32
	return true;
#else
	if( dir[ 0 ] == '/' ) return true;
	else return false;
#endif
}


bool OSFileSys::containCharInString(char c, const string &str)
{
	for(unsigned int i = 0; i < str.size(); i++) {
		if(str[i] == c) return true;
	}
	return false;
}

bool OSFileSys::checkIfValidAndAbsolutePattern(const string &pattern)
{
	return validateDirname(pattern , patternValidCharacters);
}

bool OSFileSys::checkIfValidAndAbsoluteDirname(const string &dirName)
{
	return validateDirname(dirName, dirnameValidCharacters);
}

bool OSFileSys::validateDirname(const string &dirName, const string &validChar)
{
	if(dirName == "") {
		string msg ="*<OSFileSys::validateDirname> Bad args, dirName was NULL";
		//trace_error(msg);
		return false;
	}

	if(!isAbsolute( dirName )) {
		string msg = "<OSFileSys:validateDirname> " + dirName + " is not an absolute filename";
		//trace_error(msg);
		return false;
	}

	if (!validateFirstCharIsNotHyphen(dirName)) return false;

	string tem = dirName;

	for(unsigned int i=0 ; i < tem.length() ; i++) {
		if(!containCharInString(dirName[ i ] , validChar)) {
			string msg = "<OSFileSys::validateDirname> " + dirName + " contains an invalid character";
			//trace_error(msg);
			return false;
		}
	}

	return true;
}

bool OSFileSys::validateFirstCharIsNotHyphen(const string &dirName)
{
	if( dirName[0] == '-' ) {
		string msg = "<OSFileSys::validateFirstCharIsNotHyphen> ";
		msg += "The file name " + dirName + " starts with a hyphen character";
		//trace_error(msg);
		return false;
	}
	return true;
}

