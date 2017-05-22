#include "fork.h"
#include <sys/wait.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <iostream>
#include "log.h"

map<pid_t, Fork *>	Fork::processlist;

void Fork::sigint(int sig) {
	Log::logger->log("FORK", DEBUG)<< "Kill detected by child " << endl;
	pid_t pid=getpid();
	if (Fork::processlist.count(pid)==1) {
		Log::logger->log("FORK", DEBUG) << "Stoping status child " << endl;
		Fork::processlist[pid]->terminate();
	}
}



void Fork::sigchld(int sig) {
	int status;
	pid_t pid;
	//wait for one child process ends
	pid=wait(&status);
	Log::logger->log("FORK", DEBUG) << "Received sigchld from process " <<pid <<endl;
	if (Fork::processlist.count(pid)==1) {
		Log::logger->log("FORK", DEBUG) << "We notify the parent that child "<< pid<< " is termintated" <<endl;
		Fork::processlist[pid]->terminated();
		Fork::processlist.erase(pid);
	}
}


Fork::Fork(string name) {
	this->childpid=-1;
	this->name=name;
}

Fork::~Fork() {
	sigaction(SIGCHLD,&(this->oldact), NULL);
}

void Fork::killChild() {
	if (this->childpid>0) {
		Log::logger->log("FORK", DEBUG) << this->name<<" - " << "Send SIGINT to child : " << this->childpid << endl;
		kill(this->childpid, SIGINT);
	} else {
		Log::logger->log("FORK", DEBUG) << this->name<<" - " << "Can't send SIGINT to child pid " << this->childpid << endl;
	}
}

 
void Fork::execute() throw (ForkException) {
	Log::logger->log("FORK", DEBUG) << this->name<<" - " << "Fork execute " << endl;
	if((this->childpid = fork()) == -1) throw ForkException();
	if(this->childpid == 0) {
		Log::logger->log("FORK", DEBUG) << this->name<<" - " << "Fork done, we are the child " << endl;
		struct sigaction eventSigInt;
		//Create the sigaction structure to handle SIGINT signal
		sigemptyset(&eventSigInt.sa_mask);
		eventSigInt.sa_flags=0;
		eventSigInt.sa_handler= Fork::sigint;
		//Start listening to SIGINT signal
		sigaction(SIGINT,&eventSigInt, NULL);
		pid_t pid=getpid();
		Fork::processlist[pid]=this;
		Log::logger->log("FORK", DEBUG) << this->name<<" - " << "We will execute the child action " <<pid << endl;
		this->child();
		exit(0);
	} else {
		Log::logger->log("FORK", DEBUG) << this->name<<" - " << "Fork done, we are the parent" << endl;

		struct sigaction eventSigChld;
		//Create the sigaction structure to handle SIGCHLD signal
		sigemptyset(&eventSigChld.sa_mask);
		eventSigChld.sa_flags=0;
		eventSigChld.sa_handler= Fork::sigchld;
		//Start listening to SIGCHLD signal
		sigaction(SIGCHLD,&eventSigChld, &(this->oldact));

		Fork::processlist[this->childpid]=this;
		Log::logger->log("FORK", DEBUG) << this->name<<" - " << "We will execute the parent action" << endl;
		this->parent();
	}
}
