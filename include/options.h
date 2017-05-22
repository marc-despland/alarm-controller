#ifndef _OPTIONS_H
#define _OPTIONS_H
using namespace std;
#include <exception>
#include <vector>
#include <map>
#include <string>
#include "option.h"

class OptionsStopException : public exception {
	const char* what();
};
class ExistingOptionException : public exception {
	const char* what();
};
class UnknownOptionException : public exception {
	const char* what();
};
class Options {
	public:
		Options(string program, string version, string description);
		void add(char shortopt, string longopt, string description, bool hasvalue, bool mandatory) throw(ExistingOptionException);
		Option * get(char shortopt) throw(UnknownOptionException);
		Option * get(string longopt) throw(UnknownOptionException);
		void parse(int argc, char **argv) throw(OptionsStopException);
	protected:
		void version();
		void help();
	protected:
		map<char, Option *> shortopt;
		map<string, Option *> longopt;
		string program;
		string vers;
		string description;

};

#endif
