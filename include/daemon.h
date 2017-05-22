#ifndef _DAEMON_H
#define _DAEMON_H
#include "options.h"
#include "parameters.h"
#include "fork.h"
#include "runnable.h"

class DaemonNotInitializeException : public exception {
	const char* what();
};

class Daemon: virtual public Runnable, public Fork {

	public:
		enum Action :int {UNKNOWN=-1, START=0, STOP=1, RELOAD=2, CREATE=3, CONFIGTEST=4};
		static Daemon::Action strAction(string action);
		
		virtual ~Daemon();
		static void Start(int argc, char **argv) throw (DaemonNotInitializeException,ForkException,OptionsStopException, UnknownOptionException, CantCreateFileException);
		
	protected:
		int pid();
		static Daemon * me;
		Daemon(string program, string version, string description);
		void start(int argc, char **argv) throw (ForkException,OptionsStopException, UnknownOptionException, CantCreateFileException);
		virtual void daemon()=0;
		virtual void terminate()=0;
		void daemonize();
		void quit(int status);
		static void Signal(int sig);
		void reload();
		
		Options *options;
		Parameters * parameters;
		std::filebuf * logfile;
		int lock;

		void child();
		void parent();
		void terminated();
};

#endif
