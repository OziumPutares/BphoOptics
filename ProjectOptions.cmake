include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(BphoOptics_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(BphoOptics_setup_options)
  option(BphoOptics_ENABLE_HARDENING "Enable hardening" ON)
  option(BphoOptics_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    BphoOptics_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    BphoOptics_ENABLE_HARDENING
    OFF)

  BphoOptics_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR BphoOptics_PACKAGING_MAINTAINER_MODE)
    option(BphoOptics_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(BphoOptics_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(BphoOptics_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(BphoOptics_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(BphoOptics_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(BphoOptics_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(BphoOptics_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(BphoOptics_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(BphoOptics_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(BphoOptics_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(BphoOptics_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(BphoOptics_ENABLE_PCH "Enable precompiled headers" OFF)
    option(BphoOptics_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(BphoOptics_ENABLE_IPO "Enable IPO/LTO" ON)
    option(BphoOptics_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(BphoOptics_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(BphoOptics_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(BphoOptics_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(BphoOptics_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(BphoOptics_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(BphoOptics_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(BphoOptics_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(BphoOptics_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(BphoOptics_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(BphoOptics_ENABLE_PCH "Enable precompiled headers" OFF)
    option(BphoOptics_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      BphoOptics_ENABLE_IPO
      BphoOptics_WARNINGS_AS_ERRORS
      BphoOptics_ENABLE_USER_LINKER
      BphoOptics_ENABLE_SANITIZER_ADDRESS
      BphoOptics_ENABLE_SANITIZER_LEAK
      BphoOptics_ENABLE_SANITIZER_UNDEFINED
      BphoOptics_ENABLE_SANITIZER_THREAD
      BphoOptics_ENABLE_SANITIZER_MEMORY
      BphoOptics_ENABLE_UNITY_BUILD
      BphoOptics_ENABLE_CLANG_TIDY
      BphoOptics_ENABLE_CPPCHECK
      BphoOptics_ENABLE_COVERAGE
      BphoOptics_ENABLE_PCH
      BphoOptics_ENABLE_CACHE)
  endif()

  BphoOptics_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (BphoOptics_ENABLE_SANITIZER_ADDRESS OR BphoOptics_ENABLE_SANITIZER_THREAD OR BphoOptics_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(BphoOptics_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(BphoOptics_global_options)
  if(BphoOptics_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    BphoOptics_enable_ipo()
  endif()

  BphoOptics_supports_sanitizers()

  if(BphoOptics_ENABLE_HARDENING AND BphoOptics_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR BphoOptics_ENABLE_SANITIZER_UNDEFINED
       OR BphoOptics_ENABLE_SANITIZER_ADDRESS
       OR BphoOptics_ENABLE_SANITIZER_THREAD
       OR BphoOptics_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${BphoOptics_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${BphoOptics_ENABLE_SANITIZER_UNDEFINED}")
    BphoOptics_enable_hardening(BphoOptics_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(BphoOptics_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(BphoOptics_warnings INTERFACE)
  add_library(BphoOptics_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  BphoOptics_set_project_warnings(
    BphoOptics_warnings
    ${BphoOptics_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(BphoOptics_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    BphoOptics_configure_linker(BphoOptics_options)
  endif()

  include(cmake/Sanitizers.cmake)
  BphoOptics_enable_sanitizers(
    BphoOptics_options
    ${BphoOptics_ENABLE_SANITIZER_ADDRESS}
    ${BphoOptics_ENABLE_SANITIZER_LEAK}
    ${BphoOptics_ENABLE_SANITIZER_UNDEFINED}
    ${BphoOptics_ENABLE_SANITIZER_THREAD}
    ${BphoOptics_ENABLE_SANITIZER_MEMORY})

  set_target_properties(BphoOptics_options PROPERTIES UNITY_BUILD ${BphoOptics_ENABLE_UNITY_BUILD})

  if(BphoOptics_ENABLE_PCH)
    target_precompile_headers(
      BphoOptics_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(BphoOptics_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    BphoOptics_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(BphoOptics_ENABLE_CLANG_TIDY)
    BphoOptics_enable_clang_tidy(BphoOptics_options ${BphoOptics_WARNINGS_AS_ERRORS})
  endif()

  if(BphoOptics_ENABLE_CPPCHECK)
    BphoOptics_enable_cppcheck(${BphoOptics_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(BphoOptics_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    BphoOptics_enable_coverage(BphoOptics_options)
  endif()

  if(BphoOptics_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(BphoOptics_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(BphoOptics_ENABLE_HARDENING AND NOT BphoOptics_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR BphoOptics_ENABLE_SANITIZER_UNDEFINED
       OR BphoOptics_ENABLE_SANITIZER_ADDRESS
       OR BphoOptics_ENABLE_SANITIZER_THREAD
       OR BphoOptics_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    BphoOptics_enable_hardening(BphoOptics_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
