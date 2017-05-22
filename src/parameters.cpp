#include "parameters.h"
#include <iostream>    
#include <fstream>
#include <sstream>
#include "log.h"
#include "utils.h"

void Parameters::setFileName(string filename) {
	this->filename=filename;	
}	

Parameters::Parameters(string filename)  {
	this->filename=filename;	
}

Parameters::Parameters()  {
	this->filename="";	
}

void Parameters::create() throw (CantCreateFileException){
	std::filebuf fb;
	if (!fb.open (filename.c_str(),std::ios::out)) {
		throw CantCreateFileException();
	}
	std::ostream is(&fb);
	for (std::map<string, Parameter *>::iterator it=this->params.begin(); it!=this->params.end(); ++it) {
		Log::logger->log("CONFIG", DEBUG) << "Write parameter in file " << it->second->getName() << endl;
		is << "# " << it->second->getDescription() << endl;
		if (!it->second->isMandatory()) is <<"#";
		is << it->second->getName()<<"="<<it->second->asString()<<endl<<endl;
	}
	fb.close();
}

void Parameters::parse() throw (FileNotFoundException,InvalidConfigFileException) {
	std::filebuf fb;
	if (!fb.open (filename.c_str(),std::ios::in)) {
		throw FileNotFoundException(filename);
	}
	std::istream is(&fb);
	string line;
	//is>> std::noskipws;
	while (getline(is >> ws, line)) {
		if (line.size()>0 && line.at(0)!='#') {
			
			//line is not commented
			std::istringstream is_line(line);
			//is_line>> std::noskipws;
  			std::string key;
  			if( std::getline(is_line>>ws, key, '=') ) {
  				key=trim(key);
  				//we have found a key
  				std::string value;
  				
    				if( std::getline(is_line>>ws, value) ) {
    					value=trim(value);
    					//we also have a value
    					try {
    						this->get(key)->set(value);
    						    						Log::logger->log("CONFIG", INFO) << "Read parameter " << key << " with value " << value << endl;
    					} catch (UnknownParameterNameException &e) {
    						Log::logger->log("CONFIG", DEBUG) << "Read unknown parameter " << key << " with value " << value << endl;
    					}
    				} else {
    					Log::logger->log("CONFIG", DEBUG) << "Read parameter " << key << " without value " << endl;
    				}
  			} else {
    				Log::logger->log("CONFIG", DEBUG) << "Invalid parameter line " << line << endl;
    			}
		}
	}
	fb.close(); 
	bool valid=true;
	for (std::map<string, Parameter *>::iterator it=this->params.begin(); it!=this->params.end(); ++it) {
		Log::logger->log("CONFIG", DEBUG) << "checking parameter "  << it->first << " Mandatory:"<<this->params[it->first]->isMandatory()<<" Exists:"<<this->params[it->first]->isAssign()<<endl;
		if (!this->params[it->first]->isDefine()) {
			valid=false;
			Log::logger->log("CONFIG", ERROR) << "Missing mandatory parameter " << it->first<< " in file" << endl;
		}
	}
	if (!valid) {
		Log::logger->log("CONFIG", ERROR) << "Configuration file not valid" << endl;
		throw InvalidConfigFileException();
	}
	
}


void Parameters::add(string name, string description, bool mandatory) throw (ExistingParameterNameException){
	if (this->params.count(name)==1) throw ExistingParameterNameException();
	Parameter * param=new Parameter(name, description, mandatory);
	this->params.insert(pair<string,Parameter *>(name,param));
}
void Parameters::add(string name, string description, bool mandatory, string value) throw (ExistingParameterNameException){
	if (this->params.count(name)==1) throw ExistingParameterNameException();
	Parameter * param=new Parameter(name, description, mandatory, value);
	this->params.insert(pair<string,Parameter *>(name,param));
}


Parameter * Parameters::get(string name) throw (UnknownParameterNameException) {
	if (this->params.count(name)!=1) throw UnknownParameterNameException();
	return this->params[name];
}



