# 如果环境变量%msvcr140_ROOT%不等于F:\MyCppProjects\msvcr140_的话，需要先在cmd中运行mklink /d F:\MyCppProjects\msvcr140_ "%msvcr140_ROOT%"
set(VCPKG_LINKER_FLAGS "/LIBPATH:F:/MyCppProjects/msvcr140_/Release")