#ifndef _OPTION_H
#define _OPTION_H
#include "variable.h"

class Option : public Variable {
	public:
		Option(char shortopt, string longopt, string description, bool hasvalue, bool mandatory);
		char getShortOption();
		string getLongOption();
		bool hasValue();
		string toString(bool description);
	
	protected:
		char shortoption;
		string longoption;
		bool hasvalue;
};

#endif
