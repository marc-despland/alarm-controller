#ifndef _RUNNABLE_H
#define _RUNNABLE_H
#include "irunnable.h"
using namespace std;

class Runnable : public IRunnable{
	public:
		Runnable();
		virtual void start();
		virtual void stop();
		bool running();
	protected:
		bool flag;

};
#endif
