#include "runnable.h"
#include <iostream>

Runnable::Runnable() {
	this->flag=false;
}

void Runnable::start() {
	this->flag=true;
}

void Runnable::stop() {
	this->flag=false;
}

bool Runnable::running() {
	return(this->flag);
}
