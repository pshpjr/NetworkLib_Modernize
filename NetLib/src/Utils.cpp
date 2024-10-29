#include "../include/Utils.h"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <ostream>


void psh::Utils::ASSERT_CRASH(bool condition, const char* message)
{
	if (!condition)
	{
		abort();
	}
}
