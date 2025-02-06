//
// Created by pshpj on 25. 2. 6.
//
#include "Init.h"

#include <CrashDump.h>
#include <iostream>
#include <loguru.cpp>
#include <cpptrace.hpp>

namespace psh
{
    void Init(int argc, char** argv)
    {
        loguru::init(argc, argv);

        /*
         * loguru로 SIGSEGV 같은 거 못 잡음
         * breakpad쪽 콜백에 콜스택 찍게 해도 의미 없는 정보만 나옴
         */
        loguru::set_fatal_handler([](const loguru::Message& message)
        {
            auto callstack = cpptrace::generate_trace().to_string(true);;
            LOG_F(FATAL, "%s", callstack.c_str());
        });
        utils::UseDump(L"./");

        //add_file("error.log", loguru::Append, loguru::Verbosity_FATAL);
        //add_file("info.log",loguru::Append, loguru::Verbosity_INFO);

        LOG_F(INFO, "Logger And Dump Init!");
    }
}
