cmake_minimum_required(VERSION 3.12)
project(CompilationWinter2018)

set(CMAKE_CXX_STANDARD 14)

include_directories(HW2)

add_executable(CompilationWinter2018
        Gal_Nir_Wet_Sol/HW1-Flex-12947001522264903.zip
        "Gal_Nir_Wet_Sol/HW2-Top-downparsing-13226001524233616 (1).zip"
        "Gal_Nir_Wet_Sol/HW3-Bisonandsemanticchecks-29637001525876136 (1).zip"
        Gal_Nir_Wet_Sol/HW5-codegeneration-5011001528373022.zip
        HW1/better_tests/t1.in
        HW1/better_tests/t1.out
        HW1/better_tests/t1.res
        HW1/eitan/lex.yy.c
        HW1/maxtests/t1.in
        HW1/maxtests/t1.out
        HW1/maxtests/t1.res
        HW1/maxtests/t2.in
        HW1/maxtests/t2.out
        HW1/tests/t1.in
        HW1/tests/t1.out
        HW1/tests/t1.res
        HW1/tests/t2.in
        HW1/tests/t2.out
        HW1/tests/t2.res
        HW1/a.out
        HW1/hw1.l
        HW1/hw11.l
        HW1/lex.yy.c
        HW1/selfcheck-hw1
        HW2/HW2/Debug/HW2.tlog/CL.command.1.tlog
        HW2/HW2/Debug/HW2.tlog/CL.read.1.tlog
        HW2/HW2/Debug/HW2.tlog/CL.write.1.tlog
        HW2/HW2/Debug/HW2.tlog/HW2.lastbuildstate
        HW2/HW2/Debug/HW2.tlog/link.command.1.tlog
        HW2/HW2/Debug/HW2.tlog/link.read.1.tlog
        HW2/HW2/Debug/HW2.tlog/link.write.1.tlog
        HW2/HW2/Debug/HW2.log
        HW2/HW2/HW2.vcxproj
        HW2/HW2/HW2.vcxproj.filters
        HW2/HW2/HW2.vcxproj.user
        HW2/grammar.cpp
        HW2/grammar.h
        HW2/hw2-tests.zip
        HW2/hw2.cpp
        HW2/hw2.h
        HW2/HW2.sln
        HW2/lexer.lex
        HW2/main.cpp
        HW2/Makefile
        HW2/selfcheck-hw2
        HW2/tokens.h
        .gitignore
        README.md)
