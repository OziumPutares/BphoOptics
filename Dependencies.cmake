include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(myproject_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET fmtlib::fmtlib)
    cpmaddpackage("gh:fmtlib/fmt#9.1.0")
  endif()

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.11.0
      GITHUB_REPOSITORY
      "gabime/spdlog"
      OPTIONS
      "SPDLOG_FMT_EXTERNAL ON")
  endif()

  if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.3.2")
  endif()

  if(NOT TARGET CLI11::CLI11)
    cpmaddpackage("gh:CLIUtils/CLI11@2.3.2")
  endif()
  if(NOT Freetype::Freetype)
    cpmaddpackage(
      NAME
      freetype
      GIT_REPOSITORY
      https://github.com/aseprite/freetype2.git
      GIT_TAG
      VER-2-10-0
      VERSION
      2.10.0)

    if(freetype_ADDED)
      add_library(Freetype::Freetype ALIAS freetype)
    endif()
  endif()
  find_package(OpenGL REQUIRED)
  if(NOT imgui)
    # ImGui
    cpmaddpackage(
      NAME
      imgui
      VERSION
      1.91.1
      GITHUB_REPOSITORY
      ocornut/imgui
      DOWNLOAD_ONLY
      TRUE)

    # CMakeLists.txt from https://gist.githubusercontent.com/rokups/f771217b2d530d170db5cb1e08e9a8f4
    file(
      DOWNLOAD
      "https://gist.githubusercontent.com/rokups/f771217b2d530d170db5cb1e08e9a8f4/raw/4c2c14374ab878ca2f45daabfed4c156468e4e27/CMakeLists.txt"
      "${imgui_SOURCE_DIR}/CMakeLists.txt"
      EXPECTED_HASH SHA256=fd62f69364ce13a4f7633a9b50ae6672c466bcc44be60c69c45c0c6e225bb086)

    # Options
    set(IMGUI_EXAMPLES FALSE)
    set(IMGUI_DEMO FALSE)
    set(IMGUI_ENABLE_STDLIB_SUPPORT TRUE)
    # FreeType (https://github.com/cpm-cmake/CPM.cmake/wiki/More-Snippets#freetype)
    set(FREETYPE_FOUND TRUE)
    set(FREETYPE_INCLUDE_DIRS "")
    set(FREETYPE_LIBRARIES Freetype::Freetype)

    # Add subdirectory
    add_subdirectory(${imgui_SOURCE_DIR} EXCLUDE_FROM_ALL SYSTEM)
  endif()
  if(NOT TARGET tools::tools)
    cpmaddpackage("gh:lefticus/tools#update_build_system")
  endif()

endfunction()
