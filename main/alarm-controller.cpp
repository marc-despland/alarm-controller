#include "daemon.h"
#include "log.h"
#include <stdlib.h>

class AlarmDaemon:public Daemon {
	public:
		static void Initialize(string program, string version, string description) {
			if (Daemon::me==NULL) {
				Daemon::me=new AlarmDaemon(program, version, description);
			}
		}
		~AlarmDaemon() {
		}
	protected :
		AlarmDaemon(string program, string version, string description):Daemon(program, version, description) {
			try {
				this->parameters->add("target", "The target IP to monitor", true, "192.168.1.1");
				this->parameters->add("start", "The start script to use", true, "/etc/alarm-controller/start.sh");
				this->parameters->add("stop", "The stop script to use", true, "/etc/alarm-controller/stop.sh");
				
				Log::logger->log("MAIN",NOTICE) << "Adding port and pool parameters" << endl;
			} catch(ExistingParameterNameException &e ) {
				Log::logger->log("MAIN", EMERGENCY) << "Can't create one of the file parameters"<< endl;
			}			
		}

		void daemon(){
			Log::logger->log("MAIN",NOTICE) << "Child daemon started" << endl;
				
		}
		void terminate(){
			Log::logger->log("MAIN",NOTICE) << "Child daemon terminate" << endl;
		}
		
};




int main(int argc, char **argv) {
	Log::logger->setLevel(DEBUG);
	AlarmDaemon::Initialize(argv[0], "1.0.0", "Test program for class Daemon");
		//exit(0);

	try {
		AlarmDaemon::Start(argc, argv);
	} catch(ForkException &e) {
		Log::logger->log("MAIN",NOTICE) << "ForkException occurs" << endl;
	} catch(OptionsStopException &e) {
		Log::logger->log("MAIN",NOTICE) << "OptionsStopException occurs" << endl;
	} catch(UnknownOptionException &e) {
		Log::logger->log("MAIN",NOTICE) << "UnknownException occurs" << endl;
	} catch(CantCreateFileException &e) {
		Log::logger->log("MAIN",NOTICE) << "CantCreateFileException occurs" << endl;
	}
}
