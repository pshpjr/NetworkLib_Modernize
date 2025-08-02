//
// Created by pshpj on 25. 2. 6.
//
#include "Init.h"

#include <CrashDump.h>
#include <iostream>
#include <Logger.h>
#include <cpptrace.hpp>

namespace psh
{
    void Init(int argc, char** argv)
    {
        logger::Init();
        utils::UseDump(L"./");

        psh::logger::Info("Logger And Dump Init!");
    }
}
