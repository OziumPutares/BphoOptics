include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(myproject_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET fmtlib::fmtlib)
    cpmaddpackage("gh:fmtlib/fmt#11.2.0")
  endif()

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.15.3
      GITHUB_REPOSITORY
      "gabime/spdlog"
      OPTIONS
      "SPDLOG_FMT_EXTERNAL ON")
  endif()

  if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.8.1")
  endif()

  if(NOT Freetype::Freetype)
    cpmaddpackage(
      NAME
      freetype
      GIT_REPOSITORY
      https://gitlab.freedesktop.org/freetype/freetype.git
      GIT_TAG
      VER-2-13-3
      VERSION
      2.13.3)

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
  if(NOT TARGET glm::glm)
    cpmaddpackage(
      NAME
      glm
      GITHUB_REPOSITORY
      g-truc/glm
      GIT_TAG
      1.0.1)
  endif()

endfunction()
