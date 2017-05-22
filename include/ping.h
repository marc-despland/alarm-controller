#ifndef _PING_H
#define _PING_H
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <string>


class Ping {

	public:
		static int ping(std::string target);
		
	protected:
		static uint16_t in_cksum(uint16_t *addr, unsigned len);
};

#endif
