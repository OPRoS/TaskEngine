/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "ds/Estring.h"

Estring::Estring(char* str)
{
	m_str = str;
}

Estring:: ~Estring(void)
{
}

void Estring::setValue(char *str)
{
	m_str = str;
}

char* Estring::getValue()
{
	return m_str;
}

std::string Estring::toString(int value)
{
	std::stringstream out;
	out << value;
	return out.str();
}

/**
 *  str2에 copy
 */
void Estring::strcpy(Estring* str2)
{
	char* s2 = str2->getValue();

	while(*s2++=*m_str++);

	str2->setValue(s2);
}

int Estring::strlen()
{
	const char* sc;
	for(sc=m_str; *sc !='\0';++sc);
	return (sc-m_str);
}

int Estring::strcmp(Estring* str)
{
	const char* s = str->getValue();

	for( ;*m_str==*s; ++m_str, ++s)
		if(*m_str == '\0') return 0;

	return ((*(unsigned char*)m_str < *(unsigned char*)s)?-1:+1);
}

void Estring::strcat(Estring *str)
{
	char *s = str->getValue();
	while(*m_str)m_str++;
	while(*m_str++=*s);

}

void Estring::replaceAll(std::string& text, const std::string& find_token, const std::string& replace_token)
{
	if(find_token.compare(replace_token)==0)
		return;

    size_t i = 0;
    while ( (i = text.find(find_token)) != std::string::npos )
        text.replace(i, find_token.size(), replace_token);
}

void Estring::replaceAll(std::string* text, const std::string& find_token, const std::string& replace_token)
{
	if(find_token.compare(replace_token)==0)
		return;

    size_t i = 0;
    while ( (i = text->find(find_token)) != std::string::npos )
        text->replace(i, find_token.size(), replace_token);
}


std::string Estring::trim(const std::string source)
{
	/*
	std::string whitespaces="\n\t\v\f\r ";
    std::string result("");

    size_t begin = source.find_first_not_of(whitespaces);
    size_t end = source.find_last_not_of(whitespaces);
    if (begin != std::string::npos && end != std::string::npos)
        result = source.substr(begin, end - begin + 1);

    return result;
	*/
	std::string ret = source;
	const char* chars = "\n\t\v\f\r ";
	ret.erase(ret.find_last_not_of(chars)+1);
	ret.erase(0, ret.find_first_not_of(chars));
	return ret;

}


std::vector<std::string> Estring::split(const std::string &s, const std::string &delimiter)
{
  std::vector<std::string> newrow;
  if(s=="")return newrow;
  std::size_t begin, end;
  begin = 0;
  unsigned int dlen = delimiter.length();

  while(true){
    end = s.find(delimiter, begin);	
    if(end == std::string::npos){
      newrow.push_back(s.substr(begin, s.length() - begin + dlen));
      break;
    }
    newrow.push_back(s.substr(begin, end - begin));
    begin = end + dlen;
  }
  return(newrow);
}


std::vector<std::string> Estring::split(const std::string & s, char delimiter)
{
  std::vector<std::string> newrow;
  std::size_t bpos, epos;
  bpos = 0;
  while(true){
    epos = s.find_first_of(delimiter, bpos);
    if(epos == std::string::npos){
      newrow.push_back(s.substr(bpos, s.length() - bpos + 1));
      break;
    }
    newrow.push_back(s.substr(bpos, epos - bpos));
    bpos = epos + 1;
  }
  return(newrow);
}

std::wstring Estring::wcs2wstr(std::string const& str, std::locale const& loc = std::locale())
{
        typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
        codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
        std::mbstate_t state = std::mbstate_t();
        std::vector<wchar_t> buf(str.size() + 1);
        char const* in_next = str.c_str();
        wchar_t* out_next = &buf[0];
        std::codecvt_base::result r = codecvt.in(state,
                str.c_str(), str.c_str() + str.size(), in_next,
                &buf[0], &buf[0] + buf.size(), out_next);
        //if (r == std::codecvt_base::error)
        //        throw std::runtime_error("can't convert string to wstring");
        return std::wstring(&buf[0]);
}

std::string Estring::wcs2str(std::wstring const& str, std::locale const& loc = std::locale())
{
        typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
        codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
        std::mbstate_t state = std::mbstate_t();
        std::vector<char> buf((str.size() + 1) * codecvt.max_length());
        wchar_t const* in_next = str.c_str();
        char* out_next = &buf[0];
        std::codecvt_base::result r = codecvt.out(state,
                str.c_str(), str.c_str() + str.size(), in_next,
                &buf[0], &buf[0] + buf.size(), out_next);
        //if (r == std::codecvt_base::error)
        //        throw std::runtime_error("can't convert wstring to string");
        return std::string(&buf[0]);
}


/*
std::string으로 부터 char* 으로 변환

	1번) const char* fileName =(tok->getData()).c_str(); => 이것은 문제가 발생한다.

	2번) std::string str = tok->getData();
	     const char* fileName = str.c_str(); //ok
*/
/*
int main()
{
        // 전역 locale 설정
        std::locale::global(std::locale(""));

        std::string mbs1 = "abcdef가나다라";
        std::wstring wcs1 = L"abcdef가나다라";
		std::string mbs2 = Estring::wcs2str(wcs1);
        assert(mbs1 == mbs2);
		std::wstring wcs2 = Estring::wcs2wstr(mbs1);
        assert(wcs1 == wcs2);
}

공백제거 - string.replace(/ /g,"");
공백,엔터 제거 - string.replace(/(\n| )/g,""); OR string.replace(/(\r\n| )/g,"");


*/

