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

class ErrorHandler
{
protected:
  /** a buffer for errors and debug messages */
  char messageBuffer[300];
  int errorCount;
  bool m_isError;

public:
	ErrorHandler(void)
	{
		errorCount = 0;
		m_isError = true;
	}

	virtual ~ErrorHandler(void)
	{
	};
		
	virtual void printError(std::string) = 0;	
	virtual void printMsg(std::string) = 0;
	

	/** 
	* Formats a error message and calls the printError() function.
	* @param format Format string as used by printf defined in stdio.h.
	* @param ... See printf in stdio.h.
	*/
	void error(const char* format, ...);

	/** 
	* Formats a message and calls the printMessage() function 
	* @param format Format string as used by printf defined in stdio.h.
	* @param ... See printf in stdio.h.
	*/
	void message(const char* format, ...);

	void printErrorCount() {	
	  printf("[ERR] Total Errors : %d\n", errorCount);
	}

	int hasError_(){
		return errorCount;
	};

	void addErrorCount(){
		errorCount++;
	};

	
};


class DataEH : public ErrorHandler
{
private:
	char* m_text;

public:
	DataEH(char* text) { 
		m_text = text;
	}

	virtual ~DataEH() {}

	void printError(std::string text) {	
		printf("[ERR] Irrelevant value : %s\n",m_text);
	}

	void printMsg(std::string text){
		printf("[MSG] Irrelevant value : %s\n",m_text);
	}
}; 









