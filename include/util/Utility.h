/*
 * Utility
 *
 *  Created on: 2009. 2. 9
 *      Author: jinseo
 */

#ifndef UTILITY_
#define UTILITY_

#include <iostream>
#include <string>
#include <sstream>
#include "util/ErrorHandler.h"
#include <math.h>
#include <time.h>

#if WIN32
#include <io.h>
#include <direct.h>
#include "util/MultiOS.h"
#else
#include <unistd.h>
#endif

static unsigned int sizeInfoLength = 8;

static int isAccessible(std::string path)
{
	#if WIN32
		#define CHECKACCESS _access
		//#define CHECKRIGHTS 0
	#else
		#define CHECKACCESS access
		//#define CHECKRIGHTS F_OK
	#endif

	return CHECKACCESS( path.c_str(), 0 );
	//#undef CHECKACCESS
	//#undef CHECKRIGHTS
}


static std::string getCurPath()
{
	//	const char *appdata = std::getenv ( "CWD" );
	//	std::string fullpathName(appdata);

	char curPath[200];
	getcwd(curPath, 200);
	std::string fullpathName(curPath);
	return fullpathName;
}

/*
char*를 int로 변환: atoi
char*를 long으로 변환: atol, strtol
char*를 double로 변환: atof, strtod
*/

/*
inline std::string double2str(double x)
{
	std::ostringstream o;
	if (!(o << x))
		throw DataEH("double2str");
	return o.str();
}

inline double str2double(const std::string& str)
 {
	std::istringstream i(str);
	double x;
	if (!(i >> x))
		throw DataEH("str2double");
	return x;
 }
*/

static float str2float(const std::string& valueStr)
 {
	std::istringstream i(valueStr);
	float x;
	if (!(i >> x))
		throw DataEH("str2float");
	return x;
 }

static int str2int(const std::string& valueStr)
 {
	std::istringstream i(valueStr);
	int x;
	if (!(i >> x))
		throw DataEH("str2int");
	return x;
 }

static bool str2bool(const std::string& valueStr)
 {
	 if(valueStr.compare("true") ==0)
		return true;
	 if(valueStr.compare("false")==0)
		return  false;

	 throw DataEH("str2bool");
 }

static std::string int2str(int x)
{
	std::ostringstream o;
	if (!(o << x))
		throw DataEH("int2str");
	return o.str();
}

static std::string int2strFP(int x, int fp)
{
	std::ostringstream o;
	if (!(o << x))
		throw DataEH("int2str");
	
	std::string trans =  o.str();
	std::string r("");
	for(unsigned int i=0; i<sizeInfoLength-trans.size(); i++){
		r.append("0");
	}
	r.append(trans);

	return r;
}

static std::string float2str(float x)
{
	std::ostringstream o;
	if (!(o << x))
		throw DataEH("float2str");
	return o.str();
}

static std::string bool2str(bool x)
{
	if(x) return std::string("true");
	else return std::string("false");
}

static int randomE(int size) 
{
	 return rand() % size;
}

static double randomF(double min, double max) 
{
	double	X =  2 * ((double) rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
	
	//return X;

	//printf("--%f\n", X);
	//0~30에서 gaussianRandom을 얻고자 할 때
	double mu = (max + min)/2.0 ;
	double a = (max-min)/2.0;	//1. (30-0)/2=5, -1부터 1까지는 2 
	double b = X*a;				//2. 시그마가 0.2가 나오면, 0.2*5=1
	double c = mu + b;			//3. (30-0)/2+1 = 16
	return c;
}

static int randomD(double min, double max) 
{
	double	X =  2 * ((double) rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
	
	//return X;

	//printf("--%f\n", X);
	//0~30에서 gaussianRandom을 얻고자 할 때
	double mu = (max + min)/2.0 ;
	double a = (max-min)/2.0;	//1. (30-0)/2=5, -1부터 1까지는 2 
	double b = X*a;				//2. 시그마가 0.2가 나오면, 0.2*5=1
	double c = mu + b;			//3. (30-0)/2+1 = 16
	return (int)c;
}

static double gaussianRandomF(double min, double max) 
{
	
	double v1, v2, s;

	do {
		v1 =  2 * ((double) rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
		v2 =  2 * ((double) rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
		s = v1 * v1 + v2 * v2;
	} while (s >= 1 || s == 0);

	s = sqrt( (-2 * log(s)) / s );
	double X = v1*s;
	//return X;

	//printf("--%f\n", X);
	//0~30에서 gaussianRandom을 얻고자 할 때
	double mu = (max + min)/2.0 ;
	double a = (max-min)/6.0;	//1. (30-0)/6=5
	double b = X*a;			//2. 시그마가 0.2가 나오면, 0.2*5=1
	double c = mu + b;	//3. (30-0)/2+1 = 16
	return c;
}
static int gaussianRandomD(double min, double max) 
{
	
	double v1, v2, s;

	do {
		v1 =  2 * ((double) rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
		v2 =  2 * ((double) rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
		s = v1 * v1 + v2 * v2;
	} while (s >= 1 || s == 0);

	s = sqrt( (-2 * log(s)) / s );
	double X = v1*s;
	double mu = (max + min)/2.0 ;
	double a = (max-min)/6.0;	//1. (30-0)/6=5
	double b = X*a;			//2. 시그마가 0.2가 나오면, 0.2*5=1
	double c = mu + b;	//3. (30-0)/2+1 = 16
	return (int)c;
}


/*
static float str2float(std::string str)
{
	float f;
	if(EOF == sscanf(str.c_str(), "%f", &f)){
		//error
	}
	return f;
}

static int str2int(std::string str)
{
	int i;
	if(EOF == sscanf(str.c_str(), "%d", &i)){
		//error
	}
	return i;
}
*/
/*
static bool	str2bool(std::string);
{
}

static char	str2char(std::string);
{
}

static short str2short(std::string);
{
}

static long	str2long(std::string);
{
}

static double str2double(std::string);
{
}
*/

/*
// 문자열 buf에서 제일 마지막에 나타나는 문자열 a를 제거
int delEchar(char *buf, char *a) {
    int ik = strlen(buf), ib=strlen(a);
    if(ib>0 && ik>=ib)                    // 삭제할여는 문자가 있어면
    {
        ib--; ik--;                          // 문자열의 배열의 끝 기억  (c에서 배열은 0부터 시작)
        int n, i=ik;
        while(i>=0){
			if(buf[i]==a[ib]){           // 문자열 buf에서 문자열 a의 마지막 문자를 찾으면 
				for(n=ib; buf[i] == a[n]; n--){ 
					if(n == 0){        // 같은 문자를 찾으면                         
						for(; buf[i+ib] !='\0'; i++){  // buf에서 문자 삭제                                  
                            buf[i] = buf[i+ib+1];
                        }
                        buf[ik-ib]='\0';
                        return 1;
                    }
                    i--; 
                }  
                continue;
            }
            i--;
        }  
    }
    return 0;
}

 // 문자열 buf에서 제일 처음에 나타나는 문자열 a를 제거
int delSchar(char *buf, char *a)
{
    int n=0, k=0;
    for(int i=0; buf[i]!='\0'; i++) 
    {
       if(buf[i]==a[n]) 
       { 
           k = i;
           while(buf[i]==a[n])  
           {
               if(a[++n]=='\0')
               {           
                   for(k; buf[k+n]!='\0'; k++)
                   {                   
                       buf[k] = buf[k+n];
                   }
                   buf[k]='\0';
               
                   return 1;
               }
               i++;
           } 
           n=0;
           i--;
       }
    } 
    return 0;
}

*/

#endif /* UTILITY_ */
