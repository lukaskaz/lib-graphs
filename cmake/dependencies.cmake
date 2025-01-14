cmake_minimum_required(VERSION 3.10)

find_package(Threads REQUIRED)

include(ExternalProject)

set(source_dir "${CMAKE_BINARY_DIR}/libwebserver-src")
set(build_dir "${CMAKE_BINARY_DIR}/libwebserver-build")

EXTERNALPROJECT_ADD(
  libwebserver
  GIT_REPOSITORY    https://github.com/lukaskaz/lib-webserver.git
  GIT_TAG           main
  PATCH_COMMAND     ""
  PREFIX            libwebserver-workspace
  SOURCE_DIR        ${source_dir}
  BINARY_DIR        ${build_dir}
  CONFIGURE_COMMAND mkdir /${build_dir}/build &> /dev/null
  BUILD_COMMAND     cd ${build_dir}/build && cmake -D BUILD_SHARED_LIBS=ON
                    ${source_dir} && make
  UPDATE_COMMAND    ""
  INSTALL_COMMAND   "" 
  TEST_COMMAND      ""
)

include_directories(${source_dir}/inc)
link_directories(${build_dir}/build)
