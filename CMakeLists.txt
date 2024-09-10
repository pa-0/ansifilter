### export CMAKE_PREFIX_PATH=/usr/local/Qt-6.5.3/

### Build and install ansifilter (lib, cli and gui).
### This cmake build file is NOT mandatory, you can still use the old makefile
### if preferred.
### The script tries to compile the GUI using Qt6, with Qt5 5.15 as fallback.

cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0167 NEW)

project(ansifilter-all)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compile options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -DNDEBUG -D_FILE_OFFSET_BITS=64")

# Directories
set(CORE_DIR src)
set(GUI_QT_DIR src/qt-gui)

set(INCLUDE_DIR src)


# Set installation directories
set(PREFIX /usr)
set(DATA_DIR ${PREFIX}/share)

set(BIN_DIR ${PREFIX}/bin)
set(LIB_DIR ${PREFIX}/lib)
set(MAN_DIR ${DATA_DIR}/man)
set(DOC_DIR ${DATA_DIR}/doc)

set(BASH_COMP_DIR ${DATA_DIR}/bash-completion/completions)
set(FISH_COMP_DIR ${DATA_DIR}/fish/vendor_completions.d)
set(ZSH_COMP_DIR ${DATA_DIR}/zsh/site-functions)

set(DESKTOP_APPS_DIR ${DATA_DIR}/applications)
set(DESKTOP_ICONS_DIR ${DATA_DIR}/icons/hicolor/256x256/apps)
set(GUI_FILES_DIR ${ansifilter_DATA_DIR}/gui_files)

# Source files
set(CORE_OBJECTS
    ${CORE_DIR}/stylecolour.cpp
    ${CORE_DIR}/stringtools.cpp
    ${CORE_DIR}/latexgenerator.cpp
    ${CORE_DIR}/texgenerator.cpp
    ${CORE_DIR}/rtfgenerator.cpp
    ${CORE_DIR}/htmlgenerator.cpp
    ${CORE_DIR}/svggenerator.cpp
    ${CORE_DIR}/codegenerator.cpp
    ${CORE_DIR}/pangogenerator.cpp
    ${CORE_DIR}/bbcodegenerator.cpp
    ${CORE_DIR}/elementstyle.cpp
    ${CORE_DIR}/preformatter.cpp
    ${CORE_DIR}/platform_fs.cpp
    ${CORE_DIR}/plaintextgenerator.cpp
)

set(CLI_OBJECTS
    ${CORE_DIR}/arg_parser.cpp
    ${CORE_DIR}/cmdlineoptions.cpp
    ${CORE_DIR}/main.cpp
)

set(GUI_OBJECTS
    ${GUI_QT_DIR}/main.cpp
    ${GUI_QT_DIR}/mydialog.cpp
)

# Library
add_library(ansifilter-lib STATIC ${CORE_OBJECTS})

# CLI executable
add_executable(ansifilter ${CLI_OBJECTS})
target_link_libraries(ansifilter ansifilter-lib ${LUA_LIBRARIES} dl)
set_target_properties(ansifilter PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
target_include_directories(ansifilter-lib PRIVATE ${INCLUDE_DIR})
target_include_directories(ansifilter PRIVATE ${INCLUDE_DIR})

# Define custom directories
target_compile_definitions(ansifilter-lib PRIVATE HL_CONFIG_DIR="${CONF_DIR}" HL_DATA_DIR="${ansifilter_DATA_DIR}" HL_DOC_DIR="${ansifilter_DOC_DIR}")
target_compile_definitions(ansifilter PRIVATE HL_CONFIG_DIR="${CONF_DIR}" HL_DATA_DIR="${ansifilter_DATA_DIR}" HL_DOC_DIR="${ansifilter_DOC_DIR}")

# Install locations
install(DIRECTORY DESTINATION ${ansifilter_DATA_DIR})
install(DIRECTORY DESTINATION ${CONF_DIR})
install(DIRECTORY DESTINATION ${GUI_FILES_DIR})
install(DIRECTORY DESTINATION ${GUI_FILES_DIR}/ext)
install(DIRECTORY DESTINATION ${GUI_FILES_DIR}/l10n)

install(FILES README.adoc ChangeLog.adoc COPYING INSTALL DESTINATION ${DOC_DIR})
install(FILES man/ansifilter.1 DESTINATION ${MAN_DIR}/man1)
install(
    FILES sh-completion/ansifilter.bash
    RENAME ansifilter
    DESTINATION ${BASH_COMP_DIR}
)
install(FILES sh-completion/ansifilter.fish DESTINATION ${FISH_COMP_DIR})
install(
    FILES sh-completion/ansifilter.zsh
    RENAME _ansifilter
    DESTINATION ${ZSH_COMP_DIR}
)
install(FILES ansifilter.desktop DESTINATION ${DESKTOP_APPS_DIR} COMPONENT gui)
install(FILES src/qt-gui/ansifilter.xpm DESTINATION ${DESKTOP_ICONS_DIR} COMPONENT gui)

# Installation of binaries
install(TARGETS ansifilter DESTINATION ${BIN_DIR} )

# Messages for the user
add_custom_target(post-install
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    COMMAND ${CMAKE_COMMAND} -E echo "Type ansifilter --help or man ansifilter for instructions."
    COMMAND ${CMAKE_COMMAND} -E echo "Execute 'make install-gui' to install the ansifilter GUI."
    COMMAND ${CMAKE_COMMAND} -E echo "Do not hesitate to report problems. Unknown bugs are hard to fix."
)

# Uninstall target
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${ansifilter_DATA_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${ansifilter_DOC_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CONF_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove ${MAN_DIR}/man1/ansifilter.1.gz
    COMMAND ${CMAKE_COMMAND} -E remove ${BASH_COMP_DIR}/ansifilter
    COMMAND ${CMAKE_COMMAND} -E remove ${FISH_COMP_DIR}/ansifilter.fish
    COMMAND ${CMAKE_COMMAND} -E remove ${ZSH_COMP_DIR}/_ansifilter
    COMMAND ${CMAKE_COMMAND} -E remove ${BIN_DIR}/ansifilter
    COMMAND ${CMAKE_COMMAND} -E remove ${BIN_DIR}/ansifilter-gui
    COMMAND ${CMAKE_COMMAND} -E remove ${DESKTOP_APPS_DIR}/ansifilter.desktop
    COMMAND ${CMAKE_COMMAND} -E remove ${DESKTOP_ICONS_DIR}/ansifilter.xpm
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
)

# qt gui

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# Qt version detection and setup
find_package(Qt6 COMPONENTS Core Gui Widgets)

if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Gui Widgets)
endif()

# Add the sources
set(SOURCES
    ${GUI_QT_DIR}/main.cpp
    ${GUI_QT_DIR}/mydialog.cpp
)

# Add the headers
set(HEADERS
    ${GUI_QT_DIR}/mydialog.h
)

# Add the UI forms
set(FORMS
    ${GUI_QT_DIR}/ansifilter.ui
)

qt_wrap_ui(UIS_HDRS
    ${GUI_QT_DIR}/ansifilter.ui
)

# Add the resources
set(RESOURCES
    ${GUI_QT_DIR}/ansifilter.qrc
)

qt_add_resources(GUI_QRC_SOURCES ${GUI_QT_DIR}/ansifilter-gui.qrc)

# Create the executable
add_executable(ansifilter-gui ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES} ${UIS_HDRS})

target_link_libraries(ansifilter-gui Qt::Core Qt::Gui Qt::Widgets ansifilter-lib ${LUA_LIBRARIES})

set_target_properties(ansifilter-gui PROPERTIES AUTOMOC TRUE)
set_target_properties(ansifilter-gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(ansifilter-gui PRIVATE ${INCLUDE_DIR})

target_compile_definitions(ansifilter-gui PRIVATE HL_CONFIG_DIR="${CONF_DIR}" HL_DATA_DIR="${ansifilter_DATA_DIR}" HL_DOC_DIR="${ansifilter_DOC_DIR}"  -DQT)

install(TARGETS ansifilter-gui DESTINATION ${BIN_DIR} OPTIONAL COMPONENT gui)

# Platform-specific configurations
if (WIN32)
    #set(DESTDIR ../..)
    #include_directories(F:/libs/lua_5.4.0/ ../../include F:/libs/boost_1_82_0)
    #add_definitions(-DWIN32)
    #remove_definitions(-DNOCRYPT)
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STATIC")
    #target_link_libraries(ansifilter-gui -LF:/git/ansifilter/src -lansifilter)
    #if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    #    target_link_libraries(ansifilter-gui -LF:/libs/lua_5.4.0/x64 -llua)
    #else()
    #    target_link_libraries(ansifilter-gui -LF:/libs/lua_5.4.0/x32 -llua)
    #endif()
    #set_target_properties(ansifilter-gui PROPERTIES RESOURCE ansifilter-gui.rc)
    #add_custom_command(TARGET ansifilter-gui POST_BUILD COMMAND F:/upx393w/upx.exe --best --force F:/git/ansifilter/ansifilter-gui.exe)
elseif (APPLE)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 14.0)
    set(CMAKE_MACOSX_BUNDLE ON)
    set_target_properties(ansifilter-gui PROPERTIES MACOSX_BUNDLE_ICON_FILE ansifilter.icns)
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/ansifilter.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()
