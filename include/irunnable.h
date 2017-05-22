#ifndef _IRUNNABLE_H
#define _IRUNNABLE_H
class IRunnable {
	public:
		virtual void start()=0;
		virtual void stop()=0;
		virtual bool running()=0;
};
#endif
