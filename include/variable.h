#ifndef _VARIABLE_H
#define _VARIABLE_H

using namespace std;
#include <string>

class Variable {
	public :
		Variable(string description, bool mandatory);
		Variable(string description, bool mandatory, string value);
		
		virtual ~Variable();
		
		string getDescription();
		
		void set(string value);
		
		
		bool isMandatory();
		bool isAssign();
		bool isDefine();
		
		string asString();
		int asInt();
		double asDouble();
		char * asChars();
		bool asBool();
		
	protected:
		string description;
		string value;
		bool mandatory;
		bool assign;		
};

#endif
