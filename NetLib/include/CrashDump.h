// CrashDump
// Created by: pshpjr
// Date: 2024-10-29
//
// Description: 
//

#ifndef CRASHDUMP_H
#define CRASHDUMP_H

//STL 헤더
#include <memory>
//서드파티 헤더

//프로젝트 헤더
#include "types.h"
#include <string>

namespace psh::utils
{
	/// <summary>
	/// 이거 호출 이후에 프로그램이 크래시가 나면 dump 파일이 생성됩니다.
	///	한 번만 호출 가능
	/// </summary>
	/// <param name="dumpPath"></param>
	void UseDump(const std::wstring& dumpPath);
}


#endif // CRASHDUMP_H
