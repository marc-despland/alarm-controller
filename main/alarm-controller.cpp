#include "daemon.h"
#include "log.h"
#include "ping.h"
#include <stdlib.h>
#include <unistd.h>


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
		bool monitor;
		bool stateon;
		AlarmDaemon(string program, string version, string description):Daemon(program, version, description) {
			try {
				this->parameters->add("target", "The target IP to monitor", true, "192.168.1.1");
				this->parameters->add("start", "The start script to use", true, "/opt/alarm-controller/scripts/start.sh");
				this->parameters->add("stop", "The stop script to use", true, "/opt/alarm-controller/scripts/stop.sh");
				
				Log::logger->log("MAIN",NOTICE) << "Adding port and pool parameters" << endl;
			} catch(ExistingParameterNameException &e ) {
				Log::logger->log("MAIN", EMERGENCY) << "Can't create one of the file parameters"<< endl;
			}			
		}

		void daemon(){
			Log::logger->log("MAIN",NOTICE) << "Child daemon started" << endl;
			this->monitor=true;
			this->stateon=false;
			while (this->monitor) {
				int ping=Ping::ping(this->parameters->get("target")->asString());
				if (ping>=0) {
					if (ping==0) {
						Log::logger->log("MAIN",DEBUG) << "Can't find target" << endl;
						if (this->stateon) {
							Log::logger->log("MAIN",NOTICE) << "Stop the Alarm" << endl;
							this->stateon=false;
							::system(this->parameters->get("stop")->asChars());
						}
					} else {
						Log::logger->log("MAIN",DEBUG) << "Target find" << endl;
						if (!this->stateon) {
							Log::logger->log("MAIN",NOTICE) << "Start the Alarm" << endl;
							this->stateon=true;
							::system(this->parameters->get("start")->asChars());
						}
					}
				}
				sleep(10);
			}		
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
