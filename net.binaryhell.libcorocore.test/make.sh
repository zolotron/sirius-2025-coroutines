#!/bin/bash

gcc -std=c11 -I../net.binaryhell.libcorocore -DPLATFORM_LINUX -DARCH_X86_64 -c test.c -o test.o
gcc test.o ../net.binaryhell.libcorocore/cmake-build-debug/libcorocore.a -o test
