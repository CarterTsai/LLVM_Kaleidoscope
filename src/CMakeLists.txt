cmake_minimum_required(VERSION 2.8)
project(src)

find_package(LLVM)

include_directories( ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )

set(CMAKE_CXX_FLAGS "-O0 -ggdb -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O0 -ggdb")

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit native)

add_executable(main 
                    main.cc
                    error.cc
                    lexer.cc
                    parser.cc 
              )
target_link_libraries(main ${REQ_LLVM_LIBRARIES})
