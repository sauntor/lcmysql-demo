#!/bin/bash
pwd=$(cd `dirname $0`; pwd)
[[ -f $pwd/Makefile ]] && make -f $pwd/Makefile clean
rm -rf $pwd/Makefile 
rm -rf $pwd/.ycm_extra_conf.py
rm -rf $pwd/cmake_install.cmake
rm -rf $pwd/CMakeFiles
rm -rf $pwd/CMakeCache.txt
rm -rf $pwd/lcmysql-demo.kdev4
rm -rf $pwd/compile_commands.json
rm -rf $pwd/build
rm -rf $pwd/.kdev4
