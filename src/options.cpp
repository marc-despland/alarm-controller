#include "options.h"
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "log.h"

Options::Options(string program, string version, string description) {
	this->program=program;
	this->vers=version;
	this->description=description;
	try {
		this->add('h', string("help"), "Display this help message", false, false);
		this->add('v', string("version"), "Display software version", false, false);
	} catch(ExistingOptionException &e) {
		Log::logger->log("OPTION", EMERGENCY) << "Impossible error : can't create default option help or version" << endl;
	}
}


Option * Options::get(char shortopt) throw(UnknownOptionException) {
	if (this->shortopt.count(shortopt)!=1) throw UnknownOptionException();
	return this->shortopt[shortopt];
}
Option * Options::get(string longopt) throw(UnknownOptionException){
	if (this->longopt.count(longopt)!=1) throw UnknownOptionException();
	return this->longopt[longopt];
}


void Options::add(char shortopt, string longopt, string description, bool hasvalue, bool mandatory) throw(ExistingOptionException) {
	Log::logger->log("OPTION", DEBUG) << "Add option "<<shortopt << "/" <<longopt << endl;
	if (this->shortopt.count(shortopt)==1) throw ExistingOptionException();
	if (this->longopt.count(longopt)==1) throw ExistingOptionException();
	Option * opt=new Option(shortopt, longopt, description, hasvalue, mandatory);
	this->longopt.insert(pair<string, Option *>(longopt,opt));
	this->shortopt.insert(pair<char, Option *>(shortopt,opt));
}


void Options::version() {
	cout << this->program <<"\t version:"<<this->vers<<endl;
	cout << this->description<<endl;
}

void Options::help() {
	int count=0;
	cout << this->description<<endl;
	cout << "Usage : " << this->program <<"\t";
	for (std::map<string, Option *>::iterator it=this->longopt.begin(); it!=this->longopt.end(); ++it) {
		cout << it->second->toString(false) << "\t";
		count++;
		if (count>2) {
			cout << endl <<"\t\t\t";
			count=0;
		}
	}
	cout  << endl <<endl <<"Description:" <<endl;
	for (std::map<string, Option *>::iterator it=this->longopt.begin(); it!=this->longopt.end(); ++it) {
		cout << it->second->toString(true)<<endl;
	}

}


void Options::parse(int argc, char **argv) throw(OptionsStopException) {
	char * shortlist=(char *) malloc(sizeof(char)*((this->longopt.size())*2));
	struct option *longlist= (struct option *) malloc(sizeof(struct option)*(this->longopt.size()));
	int index=0;
	int opt;
	shortlist[0]=0;
	bool error=false;
	bzero(shortlist, sizeof(char)*((this->longopt.size())*2));	

	//first we create the parameter option for getopt
	int i=0;
	for (std::map<string, Option *>::iterator it=this->longopt.begin(); it!=this->longopt.end(); ++it) {
		shortlist[strlen(shortlist)]=it->second->getShortOption();
		longlist[i].name=strdup(it->second->getLongOption().c_str());
		if (it->second->hasValue()) {
			strcat(shortlist,":");
			longlist[i].has_arg=required_argument;
		} else {
			longlist[i].has_arg=no_argument;
		}
		longlist[i].flag=0;
		longlist[i].val=it->second->getShortOption();
		i++;
	}


	//then we parse the command line
	
	while ((opt=getopt_long(argc, argv, shortlist,longlist, &index))>0) {
		//printf("Read option:%c index=%d\n",opt,index);
		switch(opt) {
			case 'h':
				//Display help message
				this->help();
				throw OptionsStopException();
			break;
			case 'v':
				//Display the version
				this->version();
				throw OptionsStopException();
			break;
			case '?':
				//@printf("Unknown option\n");
			break;
			default:
				
				if (this->shortopt.count(opt)==1) {
					if (this->shortopt[opt]->hasValue()) {
						if (optarg==0) {
							error=true;
							Log::logger->log("OPTION", ERROR) << "Missing value for -" <<this->shortopt[opt]->toString(true)<< endl;
						} else {
							this->shortopt[opt]->set(optarg);
						}
					} else {
						this->shortopt[opt]->set("");
					}
				}
			break;
		}
	} //End while loop on getopt
	//Now check that every required option is set.
	for (std::map<string, Option *>::iterator it=this->longopt.begin(); it!=this->longopt.end(); ++it) {
		if (it->second->isMandatory()) {
			if (!it->second->isAssign()) {
				error=true;
				Log::logger->log("OPTION", ERROR) << "Missing required option -" <<it->second->toString(true)<< endl;
			}
		}
	}
	if (error) {
		this->help();
		throw OptionsStopException();
	}
}
