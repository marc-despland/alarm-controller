#include "option.h"
#include <string.h>
#include <stdlib.h>
#include <sstream>

Option::Option(char shortopt, string longopt, string description, bool hasvalue, bool mandatory):Variable(description, mandatory) {
	this->shortoption=shortopt;
	this->longoption=longopt;
	this->hasvalue=hasvalue;
}

char Option::getShortOption() {
	return this->shortoption;
}
string Option::getLongOption() {
	return this->longoption;
}
bool Option::hasValue() {
	return this->hasvalue;
}


string Option::toString(bool description) {
	stringstream tmp;
	if (!this->mandatory) tmp << "[";
	tmp << "-" << this->shortoption <<"|--" <<this->longoption;
	if (this->hasvalue) tmp << " <value>";
	if (!this->mandatory) tmp << "]";
	if (description) {
		if (!this->hasvalue) tmp << "\t";
		tmp <<"\t" << this->description;
	}
	return tmp.str();
}
