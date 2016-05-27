set(LINKAGE_TYPE STATIC)
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wpedantic \
-Werror -pipe -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -fno-omit-frame-pointer \
-fno-optimize-sibling-calls -g -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} \
-O3 -march=native -mtune=native -fno-ident")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} \
-Wl,--gc-sections,--strip-all,--build-id=none")
