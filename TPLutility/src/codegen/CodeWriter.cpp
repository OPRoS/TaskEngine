/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2011 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "codegen/CodeWriter.h"

CodeWriter::CodeWriter(OSFileSys* fsys)
{
	m_fsys = fsys;
}

CodeWriter:: ~CodeWriter()
{
	//m_fsys는 외부에서 삭제된다.
}

void CodeWriter::setHomeDir(std::string homeDir)
{
	m_homeDir = homeDir;
}

int CodeWriter::initStyle(std::string stylefile)
{	
	const unsigned int BUFFERSIZE = 300;
	char buf[BUFFERSIZE];

	// 버퍼 초기화 
    memset(buf, BUFFERSIZE, 0);
	char delim[] = "\n";
    char *ptr;
	bool flag = false;

	//std::string stylefileFullname = m_homeDir + "/" + stylefile;
	std::string stylefileFullname = stylefile;
	m_file_style = fopen(stylefileFullname.c_str(), "r");
	if(m_file_style == NULL) return -1;

	//패턴파일을 한줄씩 읽으면서
    while(fgets(buf, BUFFERSIZE, m_file_style) != NULL) {     
		// 문자열 분리하여 출력하기
		ptr = (char*)strtok(buf, delim);		
		
		if(ptr==NULL){
			//printf("\n");
		}
		else{
			//printf("%s\n", ptr);
			std::string line = Estring::trim(std::string(ptr));
			if(line=="$start_inline"){
				flag = true;
			}
			else if(line=="$end_inline")
				break;
			else if(line==""){
				//공백라인인 경우
			}
			else if(flag){			
				std::vector<std::string> sline = Estring::split(line, "@");
				std::string key = Estring::trim(sline[0]);
				std::string elem = sline[1];
				if(key!="" && elem != "")

					//블록인 경우
					if(Estring::trim(elem) == "$start_block"){										
						elem = "";
						std::string end = "$end_block";
						while(1){
							fgets(buf, BUFFERSIZE, m_file_style);
							ptr = (char*)strtok(buf, delim);
							if(ptr != NULL){
								if(end.compare( Estring::trim(ptr))==0) break;
								elem.append(ptr);							
							}
							elem.append("\n");
						}
					}
					
					//삽입
					m_ptable.insert(strpair(key, elem));
			}
		}
		memset(buf, BUFFERSIZE, 0);
    }	
	fclose(m_file_style);
	return 0;
}

std::string CodeWriter::getBlock(std::string key)
{
	m_it = m_ptable.find(key);	
	if(m_it != m_ptable.end()) {	
		return m_it->second;
	}
	return "";
}