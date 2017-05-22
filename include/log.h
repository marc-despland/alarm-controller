#ifndef _LOG_H
#define _LOG_H
using namespace std;
#include <iostream>
#include <fstream>
#include <map>
enum LogLevel :int {NONE=-1, EMERGENCY=0, ALERT=1, CRITICAL=2, ERROR=3, WARNING=4, NOTICE=5, INFO=6, DEBUG=7};

class Log :public ostream {
	public :
		Log(ostream &out);
		Log(streambuf * sb);
		void setLevel(LogLevel level);
		Log &log(LogLevel level);
		void setLevel(string group, LogLevel level);
		Log &log(string group, LogLevel level);
		static Log* logger;
		static string strLevel(LogLevel level);
		static LogLevel levelStr(string str);
		void close();
	protected:
		LogLevel level;
		static Log cnull;
		streambuf * buffer;
		map<string, LogLevel>	groups;
		
};

#endif
