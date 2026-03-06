#ifndef ASSERT_H_
#define ASSERT_H_

#include "../debug.h"

#define assert(x) \
	if(!(x)) { \
		panic("assert@" __FILE__ ":%d: " #x "\n", __LINE__); \
	}

#endif	/* ASSERT_H_ */
