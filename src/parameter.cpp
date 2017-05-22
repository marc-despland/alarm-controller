#include "parameter.h"


Parameter::Parameter(string name, string description, bool mandatory):Variable(description, mandatory) {
	this->name=name;
}
Parameter::Parameter(string name, string description, bool mandatory, string value):Variable(description, mandatory, value) {
	this->name=name;
}

string Parameter::getName() {
	return this->name;
}
