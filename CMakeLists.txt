cmake_minimum_required(VERSION 3.0)

project(iq-notifier)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 1)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

add_definitions(-DIQ_VERSION_MAJOR=${VERSION_MAJOR})
add_definitions(-DIQ_VERSION_MINOR=${VERSION_MINOR})
add_definitions(-DIQ_VERSION_PATCH=${VERSION_PATCH})
add_definitions(-DIQ_VERSION=${VERSION})
add_definitions(-DIQ_APP_NAME=${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5DBus)
find_package(QT5XDG)

include_directories(./GSL/include)

aux_source_directory(. SRC_LIST)

qt5_add_dbus_adaptor(SRC_LIST
    org.freedesktop.Notifications.xml
    iqdbusservice.h IQDBusService
)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything \
        -Wno-exit-time-destructors -Wno-global-constructors \
        -Wno-padded -Wno-c++98-compat")
    set_source_files_properties(notificationsadaptor.cpp PROPERTIES COMPILE_FLAGS
        "-Wno-sign-conversion -Wno-undefined-reinterpret-cast")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
endif()

set(RESOURCE_FILE qml.qrc)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${RESOURCE_FILE})

target_link_libraries(${PROJECT_NAME} Qt5::Qml)
target_link_libraries(${PROJECT_NAME} Qt5::Quick)
target_link_libraries(${PROJECT_NAME} Qt5::DBus)
target_link_libraries(${PROJECT_NAME} Qt5Xdg)

if (X11)
    add_definitions(-DIQ_X11=1)
    add_subdirectory(X11-plugin)
    target_link_libraries(${PROJECT_NAME} X11-plugin)
endif() # X11

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/config.example" DESTINATION share/${PROJECT_NAME})
install(DIRECTORY "${CMAKE_SOURCE_DIR}/themes" DESTINATION share/${PROJECT_NAME})

### CPack

set(CPACK_GENERATOR "DEB")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "IQ Notifier")
set(CPACK_PACKAGE_VENDOR "Viktor Filinkov")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_STRIP_FILES TRUE)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME};IQ Notifier")
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${VERSION}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5network5 (>=5.5), libqt5dbus5 (>=5.5), libqt5gui5 (>=5.5), libqt5core5a (>=5.5), libqt5qml5 (>=5.5), libqt5quick5 (>=5.5), libqt5xdg1 (>=1.3)")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Viktor Filinkov <bruteforce@sigil.tk>")
set(CPACK_DEBIAN_PACKAGE_SECTION "x11")

include(CPack)

