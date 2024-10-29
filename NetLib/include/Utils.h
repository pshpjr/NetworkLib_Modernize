﻿// Utils
// Created by: pshpjr
// Date: 2024-10-28
//
// Description: 
//

#ifndef UTILS_H
#define UTILS_H

//STL 헤더

//서드파티 헤더

//프로젝트 헤더

namespace psh::Utils
{
	void ASSERT_CRASH(bool condition, const char* message = nullptr);

	template<typename T>
	T& Singleton()
	{
		static T instance;
		return instance;
	}

}


#endif // UTILS_H