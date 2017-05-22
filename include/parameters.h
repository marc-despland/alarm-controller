#ifndef _PARAMETERS_H
#define _PARAMETERS_H
#include <map>
#include "myexception.h"
#include "parameter.h"

using namespace std;



class FileNotFoundException : public MyException {
	public:
		FileNotFoundException(string target):MyException(target) {};
};
class CantCreateFileException : public exception {
	const char* what();
};
class InvalidConfigFileException : public exception {
	const char* what();
};
class UnknownParameterNameException : public exception {
	const char* what();
};
class ExistingParameterNameException : public exception {
	const char* what() ;
};
class Parameters {
	public:
		Parameters(string filename) ;
		Parameters() ;
		void parse() throw (FileNotFoundException,InvalidConfigFileException);
		void add(string name, string description, bool mandatory)throw (ExistingParameterNameException);
		void add(string name, string description, bool mandatory,string value)throw (ExistingParameterNameException);
		Parameter * get(string name) throw (UnknownParameterNameException);
		void create()throw (CantCreateFileException);
		void setFileName(string filename) ;

	protected:
		map<string, Parameter *> params;
		string filename;
		
};

#endif
