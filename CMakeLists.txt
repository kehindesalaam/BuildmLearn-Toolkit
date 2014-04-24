########################################################################################
#
# Copyright (c) 2012, BuildmLearn Contributors listed at http://buildmlearn.org/people/
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
# 
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
# 
# * Neither the name of the BuildmLearn nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
########################################################################################
#
# BuildmLearn Toolkit compilation script for CMake.
#
# Usage:
#   a) DEBUG build for testing.
#     cmake -DCMAKE_BUILD_TYPE=debug -DCMAKE_INSTALL_PREFIX=/usr -DUSE_QT_5=ON
#
#   b) RELEASE build for production use.
#     cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=/usr -DUSE_QT_5=ON
#
# Arguments:
#   "-DCMAKE_INSTALL_PREFIX=/usr"
#     Installation path, replace with something like "C:\rssguard" on Windows.
#
#   "-DUSE_QT_5=OFF"
#     Specifies which major Qt version to use. Qt 4 and Qt 5 are supported.
#     If "OFF" is passed as an argument, then Qt 4 is used. Default is "OFF".
#
# Other information:
#   - supports Windows, Linux, OS/2 (eComStation),
#   - Qt 4.7.3 and higher is required,
#   - resource compiler (windres.exe) is needed on MinGW,
#   - resource compiler (rc.exe) is needed on OS/2.
#
# Authors and contributors:
#   - BuildmLearn Contributors listed at http://buildmlearn.org/people/,
#
########################################################################################

# Setup basic variables.
project(buildmlearn-toolkit)

set(APP_NAME "BuildmLearn Toolkit")
set(APP_LOW_NAME "buildmlearn-toolkit")
set(APP_VERSION "2.0.1")
set(FILE_VERSION "2.0.1.0")
set(APP_AUTHOR "BuildmLearn Contributors")
set(APP_URL "http://buildmlearn.org/")
set(APP_URL_ISSUES "https://github.com/BuildmLearn/BuildmLearn-Toolkit/issues")
set(APP_URL_ISSUES_NEW "https://github.com/BuildmLearn/BuildmLearn-Toolkit/issues/new")
set(APP_EMAIL "rotter.martinos@gmail.com")
set(MINIMUM_QT_VERSION 4.7.3)
set(EXE_NAME ${APP_LOW_NAME})

# Options declaration.
option(USE_QT_5 "Use Qt 5 for building" OFF)

if(POLICY CMP0012)
  cmake_policy(SET CMP0012 NEW)
endif()

if(${USE_QT_5})
  cmake_minimum_required(VERSION 2.8.10)
else(${USE_QT_5})
  cmake_minimum_required(VERSION 2.8.0)
endif(${USE_QT_5})

message(STATUS "[${APP_LOW_NAME}] Welcome to ${APP_NAME} compilation process.")
message(STATUS "[${APP_LOW_NAME}] Compilation process begins right now.")

# Setup misc options.
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/resources/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Eliminate a warning when building in Windows that relates
# to static linking of Qt executables to qtmain.lib.
# This policy was introduced in CMake version 2.8.11.
# CMake version 2.8.11.2 warns when the policy is not set
# and uses OLD behavior.
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

# Select which Qt version to use.
if(${USE_QT_5})
  message(STATUS "[${APP_LOW_NAME}] Using Qt 5 library for building.")
  
  # Instruct CMake to run moc automatically when needed.
  set(CMAKE_AUTOMOC ON)
else(${USE_QT_5})
  message(STATUS "[${APP_LOW_NAME}] Using Qt 4 library for building.")
endif(${USE_QT_5})

# Obtain revision number if available.
message(STATUS "[${APP_LOW_NAME}] Obtaining revision number.")

if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
  find_package(Git)
  if(GIT_FOUND)
    # TODO: https://wiki.archlinux.org/index.php/VCS_PKGBUILD_Guidelines#Git
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_REVISION
    )
    string(REGEX REPLACE "\n" "" GIT_REVISION "${GIT_REVISION}")
    set(APP_REVISION ${GIT_REVISION})
  else(GIT_FOUND)
    set(APP_REVISION -)
  endif(GIT_FOUND)
else(EXISTS "${PROJECT_SOURCE_DIR}/.git")
  set(APP_REVISION -)
endif(EXISTS "${PROJECT_SOURCE_DIR}/.git")

message(STATUS "[${APP_LOW_NAME}] Resolved revision number: " ${APP_REVISION} ".")

# Configure desktop entry for Linux.
if(UNIX)
  message(STATUS "[${APP_LOW_NAME}] Generating desktop entry for Linux.")
  configure_file (
    ${PROJECT_SOURCE_DIR}/resources/desktop/${APP_LOW_NAME}.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/resources/desktop/${APP_LOW_NAME}.desktop
  )
  set(DESKTOP_ENTRY_PATH ${CMAKE_INSTALL_PREFIX}/share/applications)
endif(UNIX)

# Configure internal C++ defines.
configure_file (
  ${PROJECT_SOURCE_DIR}/src/definitions/definitions.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/definitions/definitions.h
)

# Define some useful DEBUG for, ehrm, debug build.
if(CMAKE_BUILD_TYPE STREQUAL "release" OR CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  message(STATUS "[${APP_LOW_NAME}] A release build (non-debug) is chosen. Debugging outputs are silently ignored.")
  set(CMAKE_BUILD_TYPE MinSizeRel)
  message(STATUS "[${APP_LOW_NAME}] Output executable file is optimized for minimum size.")
else(CMAKE_BUILD_TYPE STREQUAL "release" OR CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  message(STATUS "[${APP_LOW_NAME}] A debug build is chosen.")
  add_definitions(-DDEBUG)
  set(CMAKE_BUILD_TYPE Debug)

# Enable compiler warnings.
# See http://stackoverflow.com/questions/2368811/how-to-set-warning-level-in-cmake
message(STATUS "[${APP_LOW_NAME}] Enabling verbose makefile and full warning reports.")
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-pedantic -Wall -Wextra)
elseif(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
  #add_definitions(/W3)
endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

# Verbose compiling outputs.
set(CMAKE_VERBOSE_MAKEFILE ON)

endif(CMAKE_BUILD_TYPE STREQUAL "release" OR CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RELEASE")

# Force Qt to use string builders.
add_definitions(-DQT_USE_QSTRINGBUILDER)

# Unicode settings.
add_definitions(-DUNICODE -D_UNICODE)

if(${USE_QT_5})
  # Check for C++ 11 features availability.
  # See http://stackoverflow.com/questions/10984442/how-to-detect-c11-support-of-a-compiler-with-cmake
  if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION
    )

    if(NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
      message(FATAL_ERROR "[${APP_LOW_NAME}] Your C++ compiler does not support C++ 11.")
    else(NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
      add_definitions(-std=c++11)
    endif(NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))

  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_definitions(-std=c++11 -stdlib=libc++)
  elseif(${MSVC_VERSION} VERSION_LESS 1600)
    message(FATAL_ERROR "[${APP_LOW_NAME}] Your C++ compiler does not support C++ 11.")
  endif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
endif(${USE_QT_5})

# Setup libraries.
if(${USE_QT_5})
  find_package(Qt5 REQUIRED Sql Widgets Xml XmlPatterns Network LinguistTools)
else(${USE_QT_5})
  set(QT_MIN_VERSION ${MINIMUM_QT_VERSION})
  find_package(Qt4 REQUIRED QtCore QtGui QtSql QtNetwork QtXml QtXmlPatterns)
  include(${QT_USE_FILE})
endif(${USE_QT_5})

# Configure QStringBuilder behavior.
if(${USE_QT_5})
  message(STATUS "[${APP_LOW_NAME}] Enabling fast QString concatenation.")

  add_definitions(-DQT_USE_FAST_CONCATENATION)
  add_definitions(-DQT_USE_FAST_OPERATOR_PLUS)
elseif(NOT ${QTVERSION} VERSION_LESS 4.8.0)
  message(STATUS "[${APP_LOW_NAME}] Enabling fast QString concatenation.")

  add_definitions(-DQT_USE_FAST_CONCATENATION)
  add_definitions(-DQT_USE_FAST_OPERATOR_PLUS)
else(NOT ${QTVERSION} VERSION_LESS 4.8.0)
  message(STATUS "[${APP_LOW_NAME}] Disabling fast QString concatenation.")
endif(${USE_QT_5})

# APP source files.
set(APP_SOURCES
  ${APP_SOURCES}
  
  src/main.cpp

  src/gui/formmain.cpp
  src/gui/formnewproject.cpp

  src/miscellaneous/application.cpp
  src/miscellaneous/settings.cpp
  src/miscellaneous/debugging.cpp

  src/network-web/webfactory.cpp

  src/FlashcardTemplate.cpp
  src/InfoTemplate.cpp
  src/QuestionItem.cpp
  src/QuizTemplate.cpp
)

# APP headers.
set(APP_HEADERS
  ${APP_HEADERS}
  
  src/gui/formmain.h
  src/gui/formnewproject.h

  src/miscellaneous/application.h
  src/miscellaneous/settings.h
  src/miscellaneous/debugging.h

  src/network-web/webfactory.h

  src/FlashcardTemplate.h
  src/InfoTemplate.h
  src/QuestionItem.h
  src/QuizTemplate.h
)

# APP form files.
set(APP_FORMS
  src/gui/formmain.ui
)

set(APP_RESOURCES
  src/images.qrc
)

# APP translations.
set(APP_TRANSLATIONS
)

set(APP_TRANSLATIONS_WO_QT
)

set(APP_TEXT
  resources/text/LICENSE
)


# Wrap files, create moc files.
if(${USE_QT_5})
  qt5_wrap_ui(APP_UI ${APP_FORMS})
  qt5_add_resources(APP_RCC ${APP_RESOURCES})
  
  # Load translations.
  if(Qt5LinguistTools_FOUND)
    message(STATUS "[${APP_LOW_NAME}] Qt Linguist Tools found. Translations will get compiled.")
    qt5_add_translation(APP_QM ${APP_TRANSLATIONS})
  else(Qt5LinguistTools_FOUND)
    message(STATUS "[${APP_LOW_NAME}] Qt Linguist Tools NOT found. No compilation for translations.")
  endif(Qt5LinguistTools_FOUND)
else(${USE_QT_5})
  qt4_wrap_cpp(APP_MOC ${APP_HEADERS})
  qt4_wrap_ui(APP_UI ${APP_FORMS})
  qt4_add_resources(APP_RCC ${APP_RESOURCES})
  
  # Load translations.
  qt4_add_translation(APP_QM ${APP_TRANSLATIONS})
endif(${USE_QT_5})

# Include additional directory paths.
include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/src
)

# Compile the toolkit.
if(${USE_QT_5})
  add_executable(${EXE_NAME} WIN32 MACOSX_BUNDLE
    ${APP_SOURCES}
    ${APP_FORMS}
    ${APP_RCC}
    ${APP_QM}
  )

  if(WIN32)
    target_link_libraries(${EXE_NAME} Qt5::WinMain)
  endif(WIN32)
  
  # Use modules from Qt.
  qt5_use_modules(${EXE_NAME}
    Core
    Widgets
    Sql
    Network
    Xml
  )
# Setup compilation for Qt 4.
else(${USE_QT_5})
  add_executable(${EXE_NAME} WIN32 MACOSX_BUNDLE
    ${APP_SOURCES}
    ${APP_FORMS}
    ${APP_MOC}
    ${APP_RCC}
    ${APP_QM}
  )

  # Link modules from Qt.
  target_link_libraries(${EXE_NAME}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTMAIN_LIBRARY}
  )
endif(${USE_QT_5})

# Installation stage.
if(WIN32 OR OS2)
  message(STATUS "[${APP_LOW_NAME}] You will probably install on Windows or OS/2.")
  install(TARGETS ${EXE_NAME}
          RUNTIME DESTINATION ./)
elseif(APPLE)

elseif(UNIX)
  message(STATUS "[${APP_LOW_NAME}] You will probably install on Linux.")
  install(TARGETS ${EXE_NAME}
          RUNTIME DESTINATION bin)
endif(WIN32 OR OS2)

# Custom target for packaging.
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME ${APP_LOW_NAME})
set(CPACK_PACKAGE_VERSION ${APP_VERSION})
set(CPACK_IGNORE_FILES "\\\\.psd$;/CVS/;/\\\\.svn/;/\\\\.git/;\\\\.swp$;/CMakeLists.txt.user;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;\\\\.qm$;/build/;\\\\.diff$;.DS_Store'")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})

# Load packaging facilities.
include(CPack)

# make dist implementation.
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# make lupdate implementation.
if(${USE_QT_5})
  add_custom_target(lupdate
    ${Qt5Core_QMAKE_EXECUTABLE} -project -o ${CMAKE_CURRENT_BINARY_DIR}/${APP_LOW_NAME}.pro
      COMMAND ${Qt5_LUPDATE_EXECUTABLE} -no-obsolete -ts ${APP_TRANSLATIONS_WO_QT} -pro ${CMAKE_CURRENT_BINARY_DIR}/${APP_LOW_NAME}.pro
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
else(${USE_QT_5})
  add_custom_target(lupdate
    ${QT_QMAKE_EXECUTABLE} -project -o ${CMAKE_CURRENT_BINARY_DIR}/${APP_LOW_NAME}.pro
      COMMAND ${QT_LUPDATE_EXECUTABLE} -no-obsolete -ts ${APP_TRANSLATIONS_WO_QT} -pro ${CMAKE_CURRENT_BINARY_DIR}/${APP_LOW_NAME}.pro
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif(${USE_QT_5})