cmake_minimum_required(VERSION 3.16)

project(bzard LANGUAGES CXX)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 5)
set(VERSION_PATCH 0)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

add_definitions(-DIQ_VERSION_MAJOR=${VERSION_MAJOR})
add_definitions(-DIQ_VERSION_MINOR=${VERSION_MINOR})
add_definitions(-DIQ_VERSION_PATCH=${VERSION_PATCH})
add_definitions(-DIQ_VERSION=${VERSION})
add_definitions(-DIQ_APP_NAME=${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# sudo apt install libpipewire-0.3-dev libudev-dev qt6-declarative-dev libqt6xdg-dev
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Widgets)
find_package(Qt6 REQUIRED COMPONENTS DBus)
find_package(PkgConfig REQUIRED)
# find_package(Qt6 REQUIRED COMPONENTS Xdg)

# find_package(QT5XDG)

pkg_check_modules(LibUdev REQUIRED libudev)

aux_source_directory(. SRC_LIST)
file(GLOB HEADER_LIST "*.h")

qt_standard_project_setup(REQUIRES 6.5)


qt6_add_dbus_adaptor(SRC_LIST
     org.freedesktop.Notifications.xml
     iqdbusservice.h BzardDBusService
)

# if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything \
#         -Wno-exit-time-destructors -Wno-global-constructors \
#         -Wno-padded -Wno-c++98-compat")
#     set_source_files_properties(notificationsadaptor.cpp PROPERTIES COMPILE_FLAGS
#         "-Wno-sign-conversion -Wno-undefined-reinterpret-cast")
# else()
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
# endif()


set(RESOURCE_FILE qml.qrc)

add_executable(${PROJECT_NAME} ${SRC_LIST}  ${HEADER_LIST} ${RESOURCE_FILE})
target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Quick
    PRIVATE Qt6::DBus
    PRIVATE Qt6Xdg
    ${LibUdev_LIBRARIES}
)

# Include directories for headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PipeWire_INCLUDE_DIRS}
    ${LibUdev_INCLUDE_DIRS}
)

# qt_add_qml_module(${PROJECT_NAME}
#     URI ${PROJECT_NAME}
#     VERSION 1.0
#     QML_FILES
#         IQButton.qml
#         IQExtraNotifications.qml
#         BzardHistoryNotification.qml
#         IQNotificationBar.qml
#         IQNotification.qml
#         main.qml
#         IQExpirationBar.qml
#         IQFancyContainer.qml
#         BzardHistoryWindow.qml
#         IQNotificationContainer.qml
#         IQPopup.qml
# )
# target_link_libraries(${PROJECT_NAME} Qt5::Qml)
# target_link_libraries(${PROJECT_NAME} Qt5::Quick)
# target_link_libraries(${PROJECT_NAME} Qt5::DBus)
# target_link_libraries(${PROJECT_NAME} Qt6Xdg)

# if (X11)
#     add_definitions(-DIQ_X11=1)
#     add_subdirectory(X11-plugin)
#     target_link_libraries(${PROJECT_NAME} X11-plugin)
# endif() # X11

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/config.example" DESTINATION share/${PROJECT_NAME})
install(DIRECTORY "${CMAKE_SOURCE_DIR}/themes" DESTINATION share/${PROJECT_NAME})

### CPack

set(CPACK_GENERATOR "DEB")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "bzard")
set(CPACK_PACKAGE_VENDOR "Viktor Filinkov")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_STRIP_FILES TRUE)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME};bzard")
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${VERSION}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})

#6.5
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5network5 (>=5.5), libqt5dbus5 (>=5.5), libqt5gui5 (>=5.5), libqt5core5a (>=5.5), libqt5qml5 (>=5.5), 
libqt6quick6 (>=6.5),
libqt6xdg4 (>=4.1)")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Viktor Filinkov <x6prlie@gmail.com>")
# set(CPACK_DEBIAN_PACKAGE_SECTION "x11")

include(CPack)

