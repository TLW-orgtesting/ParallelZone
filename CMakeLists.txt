cmake_minimum_required(VERSION 3.14)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(parallelzone VERSION "${VERSION}" LANGUAGES CXX)

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cpp)

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_DOCS "Should we build the documentation?" OFF)
option(
    ONLY_BUILD_DOCS
    "If enabled and BUILD_DOCS is true no libraries will be built" OFF
)

option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

# Work out the project paths
set(project_inc_dir "${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}")
set(project_src_dir "${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}")

if("${BUILD_DOCS}")
    find_package(Doxygen REQUIRED)
    set(DOXYGEN_EXTRA_PACKAGES amsmath) # Extra LaTeX packages for documentation
    doxygen_add_docs(parallelzone_cxx_api "${project_inc_dir}")
    if("${ONLY_BUILD_DOCS}")
        return()
    endif()
endif()

#TODO: Make sure MADworld uses the Cereal we build
cpp_find_or_build_dependency(
    MADworld
    URL github.com/m-a-d-n-e-s-s/madness
    VERSION 58437dd272609651c546d57d1a7f0a4a3b0377bd
    BUILD_TARGET MADworld
    FIND_TARGET MADworld
    CMAKE_ARGS ENABLE_UNITTESTS=OFF
               MADNESS_BUILD_MADWORLD_ONLY=ON
               MADNESS_ENABLE_CEREAL=ON
               ENABLE_MKL=OFF
               ENABLE_ACML=OFF
               BUILD_TESTING=OFF
)

cpp_find_or_build_dependency(
    bphash
    URL github.com/ryanmrichard/BPHash
    BUILD_TARGET bphash
    FIND_TARGET bphash::bphash
    BRANCH master
    CMAKE_ARGS BUILD_TESTING=OFF
)
# Note that hashes may differ with different compilers when typeid is used.
# Comment below to disable using typeid for hashes. 
target_compile_definitions(bphash PUBLIC BPHASH_USE_TYPEID)

cpp_find_or_build_dependency(
    cereal
    URL github.com/USCiLab/cereal
    BUILD_TARGET cereal
    FIND_TARGET cereal::cereal
    CMAKE_ARGS JUST_INSTALL_CEREAL=ON
)

cpp_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${project_src_dir}"
    INCLUDE_DIR "${project_inc_dir}"
    DEPENDS MADworld bphash::bphash cereal::cereal

)

include(nwx_python_mods)
cppyy_make_python_package(MPI PACKAGE parallelzone NAMESPACES parallelzone )

if("${BUILD_TESTING}")
    cpp_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
		VERSION v2.13.3
    )
    cpp_add_tests(
        test_parallelzone
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/tests"
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/parallelzone"
        DEPENDS Catch2::Catch2 MADworld parallelzone cereal::cereal
    )
endif()

