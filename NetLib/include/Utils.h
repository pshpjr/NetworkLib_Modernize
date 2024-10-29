// Utils
// Created by: pshpjr
// Date: 2024-10-28
//
// Description: 
//

#ifndef UTILS_H
#define UTILS_H

//STL 헤더
#include <cstdlib>
//서드파티 헤더

//프로젝트 헤더

namespace psh
{
	inline void AssertCrash(bool condition, const char* message = nullptr)
	{
#ifdef _DEBUG
		if (!condition)
		{
			abort();
		}
#endif
	}

	template<typename T>
	T& Singleton()
	{
		static T instance;
		return instance;
	}

}


#endif // UTILS_H