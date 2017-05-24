#include "daemon.h"
#include "log.h"
#include "blueping.h"
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
				this->parameters->add("target", "The target Bluetooth ID to monitor", true, "xx:xx:xx:xx:xx");
				this->parameters->add("entry", "The script executed when the device entry the bluetooth reception area", true, "/opt/alarm-controller/scripts/entry.sh");
				this->parameters->add("leaving", "The script executed when the device leave the bluetooth reception area", true, "/opt/alarm-controller/scripts/leaving.sh");
				this->parameters->add("bluetooth", "Use bluetooth to ping the device, if false ICMP request will be used" , true, "true");
				
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
				int ping=-1;
				if (this->parameters->get("bluetooth")->asBool()) {
					ping=BluePing::ping(this->parameters->get("target")->asString());
				} else {
					ping=Ping::ping(this->parameters->get("target")->asString());
				}
				if (ping>=0) {
					if (ping==0) {
						Log::logger->log("MAIN",DEBUG) << "Can't find target" << endl;
						if (!this->stateon) {
							Log::logger->log("MAIN",NOTICE) << "Device leave the area" << endl;
							this->stateon=true;
							::system(this->parameters->get("leaving")->asChars());
						}
					} else {
						Log::logger->log("MAIN",DEBUG) << "Target find" << endl;
						if (this->stateon) {
							Log::logger->log("MAIN",NOTICE) << "Device enter the area" << endl;
							this->stateon=false;
							::system(this->parameters->get("entry")->asChars());
						}
					}
				}
				sleep(10);
			}		
		}
		void terminate(){
			Log::logger->log("MAIN",NOTICE) << "Child daemon terminate" << endl;
			this->monitor=false;
			::system(this->parameters->get("stop")->asChars());
		}		
};




int main(int argc, char **argv) {
	Log::logger->setLevel(DEBUG);
	AlarmDaemon::Initialize(argv[0], "1.0.0", "Check a device enter in the bluetooth area of the daemon");
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
