#include "myexception.h"


MyException::MyException():exception() {
	this->target="";
}

MyException::MyException(string target):exception() {
	this->target=target;
}


const char * MyException::what() {
	return this->target.c_str();
}

