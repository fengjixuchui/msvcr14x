if(CMAKE_CL_64) 
link_directories("$ENV{msvcr14x_ROOT}/x64/Release")
link_directories("$ENV{msvcr14x_ROOT}/x64/Debug")
else()
link_directories("$ENV{msvcr14x_ROOT}/Release")
link_directories("$ENV{msvcr14x_ROOT}/Debug")
endif()