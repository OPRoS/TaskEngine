/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <fstream>
#include "util/ErrorHandler.h"


#define RTMERR_GET_VARIABLE_VALUE	1000

class ErrorLog : public ErrorHandler
{

private:
  std::ofstream logfile;
  

public:
  ErrorLog() { 
	  logfile.open("messages.log"); 
  }
  
  virtual ~ErrorLog() { 
	  logfile.close(); 
  }

  void printError(std::string text) {
	  logfile << "error: " << text.c_str() << "\n"; 
	  logfile.flush();
  }
  
  void printMsg(std::string text){
	  logfile << text.c_str() << "\n"; 
	  logfile.flush();
  }
}; 


#define DB_SYMTAB_ALREADY_EXIST_ERR			10002
#define DB_SYMTAB_ALREADY_EXIST_SKIP		10003
#define DB_SYMTAB_ALREADY_EXIST_OVERWRITE	10004

#define DB_WORKER_ALREADY_EXIST_ERR			10005
#define DB_WORKER_ALREADY_EXIST_SKIP		10006
#define DB_WORKER_ALREADY_EXIST_OVERWRITE	10007

#define DB_TASK_ALREADY_EXIST_ERR			10008
#define DB_TASK_ALREADY_EXIST_SKIP			10009
#define DB_TASK_ALREADY_EXIST_OVERWRITE		10010

#define DB_EXE_WORKER_ALREADY_EXIST_ERR			10011
#define DB_EXE_WORKER_ALREADY_EXIST_SKIP		10012
#define DB_EXE_WORKER_ALREADY_EXIST_OVERWRITE	10013

#define DB_UPDATE_ERR_NO_EXE_WORKER			10014
#define DB_STOP_ERR_NO_EXE_WORKER			10015
#define DB_UNLOAD_ERR_NO_WORKER				10016
#define DB_UNLOAD_ERR_NO_TASK				10017
#define	DB_UNLOAD_ERR_NO_SYMTAB				10018
#define DB_NO_TASK_ERROR					10019



//worker ���� �ε��� ���õ� ����
#define FILE_FAILED_TO_OPEN				1000
#define LEXER_ERROR						1001
#define TOKEN_STREAM_ERROR				1002
#define PARSER_ERROR_OUT_OF_MEM			1003
#define RUNNING_TASK_EXIST				1004

//�Ľ� �� �ø�ƽ �˻�
#define PARSER_SYNTAX_ERROR				2001  //antlr���� �߰ߵǴ� ��������
#define PARSER_NO_TREE_PARSER			2002 

class DBEH : public ErrorHandler
{
private:
	int errorCode;

public:
	DBEH(){
		errorCode = 0;
	}

	DBEH(bool isError, int code){
		errorCode = code;
		m_isError = false;
	}
  
	virtual ~DBEH() {}

	void print(){
		if(m_isError)printf("[ERR]");
		else printf("[MSG]");

		if(errorCode == DB_WORKER_ALREADY_EXIST_ERR)			
			printf("(DB:ADD,WORKER) %s\n", errorCode, "�̹� �ش��̸��� Task�� ������.");
		if(errorCode == DB_WORKER_ALREADY_EXIST_SKIP)			
			printf("(DB:ADD,WORKER) %s\n", errorCode, "�̹� �ش��̸��� Task�� �����Ͽ� �ǳʶ�.");
		if(errorCode == DB_WORKER_ALREADY_EXIST_OVERWRITE)			
			printf("(DB:ADD,WORKER) %s\n", errorCode, "�̹� �ش��̸��� Task�� �����Ͽ� ���.");		
		
		if(errorCode == DB_SYMTAB_ALREADY_EXIST_ERR)
			printf("(DB:ADD,SYM_TAB)(%d) %s\n", errorCode, "�̹� ��ϵ� �ɹ����̺��� ������");
		if(errorCode == DB_SYMTAB_ALREADY_EXIST_SKIP)
			printf("(DB:ADD,SYM_TAB)(%d) %s\n", errorCode, "�̹� ��ϵ� �ɹ����̺��� �����Ͽ� �ǳʶ�");
		if(errorCode == DB_SYMTAB_ALREADY_EXIST_OVERWRITE)
			printf("(DB:ADD,SYM_TAB)(%d) %s\n", errorCode, "�̹� ��ϵ� �ɹ����̺��� �����Ͽ� ���");

		if(errorCode == DB_TASK_ALREADY_EXIST_ERR)
			printf("(DB:ADD,TASK)(%d) %s\n", errorCode, "�̹� ��ϵ� behavior�� ������");
		if(errorCode == DB_TASK_ALREADY_EXIST_SKIP)
			printf("(DB:ADD,TASK)(%d) %s\n", errorCode, "�̹� ��ϵ� behavior�� �����Ͽ� �ǳʶ�");
		if(errorCode == DB_TASK_ALREADY_EXIST_OVERWRITE)
			printf("(DB:ADD,TASK)(%d) %s\n", errorCode, "�̹� ��ϵ� behavior�� �����Ͽ� ���");

		if(errorCode == DB_UPDATE_ERR_NO_EXE_WORKER)
			printf("(DB:UPDATE,WORKER)(%d) %s\n", errorCode, "������Ʈ�� ��� ���� Task�� �������� ����");
		if(errorCode == DB_STOP_ERR_NO_EXE_WORKER)
			printf("(DB:STOP,WORKER)(%d) %s\n", errorCode, "���� ��� ���� Task�� �������� ����");
		if(errorCode == DB_UNLOAD_ERR_NO_WORKER)
			printf("(DB:UNLOAD,WORKER)(%d) %s\n", errorCode, "������ Task�� �������� ����");
		if(errorCode == DB_UNLOAD_ERR_NO_SYMTAB)
			printf("(DB:UNLOAD,WORKER)(%d) %s\n", errorCode, "������ Task�� �ɺ����̺��� �������� ����");
		if(errorCode == DB_NO_TASK_ERROR)
			printf("(DB:FIND,TASK)(%d) %s\n", errorCode, "���࿡ �ʿ��� behavior�� �������� ����");

	}

	void printError(std::string text) {
		printf("[ERR] SYMBOL Cecking : %s\n",text.c_str());
	}

	void printMsg(std::string text){	
		printf("[MSG] SYMBOL Cecking : %s\n",text.c_str());
	}
}; 

class SemanticsEH : public ErrorHandler
{
public:
	SemanticsEH() { 	 
	}

	virtual ~SemanticsEH() {}

	void printError(std::string text) {	
		printf("[ERR] Task sentax Cecking : %s\n",text.c_str());
	}

	void printMsg(std::string text){
		printf("[MSG] Task sentax Cecking : %s\n",text.c_str());
	}
}; 

class RuntimeEH : public ErrorHandler
{
private:
	int errorCode;

public:
	RuntimeEH() { 	 
	  errorCode = 0;
	}

	virtual ~RuntimeEH() {}

	void printError() {	
		if(errorCode == RTMERR_GET_VARIABLE_VALUE)
			printf("[ERR] (RUNNIGN:%d) %s\n",errorCode, "���� ȹ���ϴµ� �����Ͽ����ϴ�.");
	}

	void printError(std::string text) {	
		printf("[ERR] Task running error : %s\n", text.c_str());
	}

	void printMsg(std::string text){
		printf("[MSG] Task running message : %s\n",text.c_str());
	}

	void setErrorCode(int code){
		errorCode = code;
	}

	int getErrorCode(){
		return errorCode;
	}
}; 