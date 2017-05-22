//http://www.enderunix.org/docs/eng/daemon.php
#include "daemon.h"
#include "log.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

Daemon * Daemon::me=NULL;

Daemon::Action Daemon::strAction(string action) {
	Action tmp=UNKNOWN;
	if (action=="start") tmp=START;
	if (action=="stop") tmp=STOP;
	if (action=="reload") tmp=RELOAD;
	if (action=="create") tmp=CREATE;
	if (action=="configtest") tmp=CONFIGTEST;
	return tmp;
}
	

void Daemon::Start(int argc, char **argv) throw (DaemonNotInitializeException, ForkException,OptionsStopException, UnknownOptionException, CantCreateFileException) {
	if (Daemon::me==NULL) throw DaemonNotInitializeException();
	Daemon::me->start(argc, argv);
}

void Daemon::Signal(int sig){
	Log::logger->log("DAEMON", DEBUG) << "Daemon process receive a signal : "<<sig<< endl;
	switch(sig){
		case SIGHUP:
			/* rehash the server */
			// Have to see what to do with that
			// Report that user's terminal is disconnected. Signal used to report the termination of the controlling process.
			// Will probably use it to close an d reopen log to work with logrotate
			Log::logger->log("DAEMON", DEBUG) << "Daemon process receive a signal : "<<sig<< " Reload the daemon"<< endl;
			Daemon::me->reload();
			break;		
		case SIGTERM:
			/* finalize the server */
			Log::logger->log("DAEMON", DEBUG) << "Daemon process receive a signal : "<<sig<< " Kill the daemon"<< endl;
			Daemon::me->quit(0);
			break;		
	}	
}

Daemon::Daemon(string program, string version, string description):Runnable(), Fork("Daemon") {
	this->logfile=NULL;
	this->lock=-1;
	this->options=new Options(program,version, description);
	try {
		this->options->add('u', "usermod", "Do not start the program as a Daemon", false, false);
		this->options->add('f', "config", "The configuration file to use", true, true);
		this->options->add('a', "action", "start|stop|create|reload|configtest", true, true);
	} catch(ExistingOptionException &e ) {
		Log::logger->log("DAEMON", EMERGENCY) << "Can't create one of the command line option"<< endl;
	}
	this->parameters=new Parameters();
	try {
		this->parameters->add("log", "The log file to use to send log", true, "/var/log/"+program+".log");
		this->parameters->add("loglevel", "The loglevel to use", true, "NOTICE");
		this->parameters->add("pid", "The file to use to store the pid of the daemon", true, "/var/run/"+program+".pid");
		this->parameters->add("chdir", "Root folder for this daemon", true,"/dev/null");
	} catch(ExistingParameterNameException &e ) {
		Log::logger->log("DAEMON", EMERGENCY) << "Can't create one of the file parameter"<< endl;
	}
}

void Daemon::start(int argc, char **argv) throw (ForkException, OptionsStopException, UnknownOptionException, CantCreateFileException) {
	int dpid;
	Log::logger->log("DAEMON", DEBUG) << "start" << endl;
	this->options->parse(argc, argv);
	Log::logger->log("DAEMON", DEBUG) << "options parsed" << endl;
	this->parameters->setFileName(this->options->get("config")->asString());
	switch (Daemon::strAction(this->options->get("action")->asString())) {
		case CREATE :
			Log::logger->log("DAEMON", DEBUG) << "Creating parameter file" << endl;
			try {
				this->parameters->create();
			} catch(CantCreateFileException &e) {
				Log::logger->log("DAEMON", EMERGENCY) << "Can't create file " << this->options->get("config")->asString()<< endl;
			}
			//throw OptionsStopException();
		case CONFIGTEST :
			cout << "Loading config file to check it is well formed" << endl;
			try {
				this->parameters->parse();
			}catch(FileNotFoundException &e) {
				Log::logger->log("DAEMON", DEBUG) << "File Not Found Exception : " << e.what()<< endl;
				cout << "Can't open config file " << e.what()<< endl;
			}catch(InvalidConfigFileException &e) {
				Log::logger->log("DAEMON", EMERGENCY) << "Invalid configuration file " << this->options->get("config")->asString()<< endl;
			}
			//throw OptionsStopException();
		break;
		case START :
			try {
				Log::logger->log("DAEMON", DEBUG) << "start to parse parameter file" << endl;
				this->parameters->parse();
				try {
					this->logfile=new filebuf();
					Log::logger->log("DAEMON", DEBUG) << "preparing to create new logger" << endl;
					if (this->logfile->open(this->parameters->get("log")->asChars(), std::ios::out|std::ios::app)==NULL) {
						Log::logger->log("DAEMON", EMERGENCY) << "Can't create log file " << this->parameters->get("log")->asString()<< endl;
						throw CantCreateFileException();
					}
					Log::logger->log("DAEMON", DEBUG) << "Logfile created" << endl;
					Log::logger->log("DAEMON", DEBUG) << "Log Level will be "<< this->parameters->get("loglevel")->asString() << " : " <<Log::levelStr(this->parameters->get("loglevel")->asString()) << endl;
					Log::logger=new Log(this->logfile);
					Log::logger->setLevel(Log::levelStr(this->parameters->get("loglevel")->asString()));
					Log::logger->log("DAEMON", DEBUG) <<"Having new logger" << endl;
					//So if everything is ok, here we have read all config parameter
					//If we are here, it means that an action is requested
			
			
			
					//We will be able to start to prepare to run the daemon
					if (this->options->get("usermod")->isAssign()) {
						Log::logger->log("DAEMON", NOTICE) << "program is start in user space (not as a daemon)"<< endl;
						//we will start in user mod (not as a daemon)
						this->daemon();
					} else {
						//we start as a daemon : we have to fork;
						this->execute();
					}
			
				} catch (UnknownParameterNameException &e) {
					Log::logger->log("DAEMON", EMERGENCY) << "Not defined parameter " << endl;
				}
			}catch(FileNotFoundException &e) {
				Log::logger->log("DAEMON", EMERGENCY) << "Can't open file " << this->options->get("config")->asString()<< endl;
			}catch(InvalidConfigFileException &e) {
				Log::logger->log("DAEMON", EMERGENCY) << "Invalid configuration file " << this->options->get("config")->asString()<< endl;
			}
		break;
		case STOP:
			Log::logger->log("DAEMON", DEBUG) << "Action stop" << endl;
			try {
				this->parameters->parse();
				dpid=this->pid();
				if (dpid>0) kill(dpid,SIGTERM); 
			}catch(FileNotFoundException &e) {
				Log::logger->log("DAEMON", EMERGENCY) << "Can't open file " << this->options->get("config")->asString()<< endl;
			}catch(InvalidConfigFileException &e) {
				Log::logger->log("DAEMON", EMERGENCY) << "Invalid configuration file " << this->options->get("config")->asString()<< endl;
			}
		break;
		case RELOAD:
			Log::logger->log("DAEMON", DEBUG) << "Action Reload" << endl;
			try {
				this->parameters->parse();
				dpid=this->pid();
				if (dpid>0) kill(dpid,SIGHUP); 
			}catch(FileNotFoundException &e) {
				Log::logger->log("DAEMON", EMERGENCY) << "Can't open file " << this->options->get("config")->asString()<< endl;
			}catch(InvalidConfigFileException &e) {
				Log::logger->log("DAEMON", EMERGENCY) << "Invalid configuration file " << this->options->get("config")->asString()<< endl;
			}
		break;
		default:
			Log::logger->log("DAEMON", EMERGENCY) << "Don't understand action " << this->options->get("action")->asString()<< endl;
		break;
	}
}

void Daemon::reload() {
	this->logfile->close();
	if (this->logfile->open(this->parameters->get("log")->asChars(), std::ios::out|std::ios::app)==NULL) {
		cout << "Can't create log file " << this->parameters->get("log")->asString()<< endl;
		this->quit(1);
	} 
	Log::logger=new Log(this->logfile);
	Log::logger->setLevel(Log::levelStr(this->parameters->get("loglevel")->asString()));
	Log::logger->log("DAEMON", NOTICE) << "Daemon has been reloaded (log file reopen)"<< endl;
}

Daemon::~Daemon() {
	delete this->options;
	delete this->parameters;
	if (this->logfile!=NULL) this->logfile->close();
}

void Daemon::child() {
	//We start to become a daemon
	Log::logger->log("DAEMON", DEBUG) << "Starting the daemon child process" << endl;
	setsid();
	int i;
	this->logfile->close();
	for (i=getdtablesize();i>=0;--i) close(i);
	
	if (this->logfile->open(this->parameters->get("log")->asChars(), std::ios::out|std::ios::app)==NULL) {
		cout << "Can't create log file " << this->parameters->get("log")->asString()<< endl;
		throw CantCreateFileException();
	}
	Log::logger=new Log(this->logfile);
	Log::logger->setLevel(Log::levelStr(this->parameters->get("loglevel")->asString()));
	Log::logger->log("DAEMON", DEBUG) <<"All file descriptor close ... could we log ?" << endl;

	i=open("/dev/null",O_RDWR); /* open stdin */
	dup(i); /* stdout */
	dup(i);
	umask(027);
	Log::logger->log("DAEMON", DEBUG) << "We are still able to log in file" << endl;
	try {
		chdir(this->parameters->get("chdir")->asChars());
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("DAEMON", EMERGENCY) << "Missing chdir parameter in config file " << endl;
	}
	try {
		this->lock=open(this->parameters->get("pid")->asChars(),O_RDWR|O_CREAT,0640);
		if (this->lock<0) {
			Log::logger->log("DAEMON", EMERGENCY) << "Can't open pid file : "<< this->parameters->get("pid")->asString()<< endl;
			this->quit(1);
		}
		if (lockf(this->lock,F_TLOCK,0)<0){
			Log::logger->log("DAEMON", EMERGENCY) << "Can't lock pid file : "<< this->parameters->get("pid")->asString()<< endl;
			this->quit(0);
		}
		/* only first instance continues */
		char str[20];
		sprintf(str,"%d\n",getpid());
		write(this->lock,str,strlen(str)); /* record pid to lockfile */
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("DAEMON", EMERGENCY) << "Missing pid parameter in config file " << endl;
	}

	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,Daemon::Signal); /* catch hangup signal */
	signal(SIGTERM,Daemon::Signal);

	this->daemon();
	
}

void Daemon::parent() {
	//As we are a Daemon, we don't care about parents
	Log::logger->close();
	exit(0);
}

void Daemon::quit(int status) {
	Log::logger->log("DAEMON", DEBUG) << "This is the end, our daemon will die!!!" << endl;
	this->stop();
	this->terminate();
	Log::logger->close();
	close(this->lock);
	exit(status);
}

int Daemon::pid() {
	int tmp=-1;
	std::fstream mylock(this->parameters->get("pid")->asChars(), std::ios_base::in);
	if (mylock.is_open()) {
		mylock >> tmp;
	} else {
		Log::logger->log("DAEMON", DEBUG) << "Can't open pid file " << this->parameters->get("pid")->asString()<< endl;
	}
			Log::logger->log("DAEMON", DEBUG) << "Daemon pid " << tmp<< endl;
	return tmp;
}

void Daemon::terminated() {

}