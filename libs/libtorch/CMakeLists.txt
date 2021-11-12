cmake_minimum_required(VERSION 3.16)

project(libtorch LANGUAGES CXX)

function(ROBOCIN_LINK_LIBTORCH LIB_PATH)
  find_package(Torch PATHS ${LIB_PATH}) # NOT REQUIRED
  if (${Torch_FOUND})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
    ROBOCIN_LINK_AND_INCLUDE_DIR(${PROJECT_NAME} ${LIB_PATH}/include)
    target_compile_definitions(${PROJECT_NAME} PUBLIC HAS_LIBTORCH)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${TORCH_LIBRARIES})
  endif ()
endfunction()

add_library(${PROJECT_NAME} STATIC include/libtorch/libtorch.h)

ROBOCIN_LINK_LIBTORCH(/opt/libtorch)