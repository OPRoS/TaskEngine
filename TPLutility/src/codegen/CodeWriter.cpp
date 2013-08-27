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
	//m_fsys�� �ܺο��� �����ȴ�.
}

void CodeWriter::setHomeDir(std::string homeDir)
{
	m_homeDir = homeDir;
}

int CodeWriter::initStyle(std::string stylefile)
{	
	const unsigned int BUFFERSIZE = 300;
	char buf[BUFFERSIZE];

	// ���� �ʱ�ȭ 
    memset(buf, BUFFERSIZE, 0);
	char delim[] = "\n";
    char *ptr;
	bool flag = false;

	//std::string stylefileFullname = m_homeDir + "/" + stylefile;
	std::string stylefileFullname = stylefile;
	m_file_style = fopen(stylefileFullname.c_str(), "r");
	if(m_file_style == NULL) return -1;

	//���������� ���پ� �����鼭
    while(fgets(buf, BUFFERSIZE, m_file_style) != NULL) {     
		// ���ڿ� �и��Ͽ� ����ϱ�
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
				//��������� ���
			}
			else if(flag){			
				std::vector<std::string> sline = Estring::split(line, "@");
				std::string key = Estring::trim(sline[0]);
				std::string elem = sline[1];
				if(key!="" && elem != "")

					//����� ���
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
					
					//����
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