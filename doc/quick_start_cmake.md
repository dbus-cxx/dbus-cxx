Using CMake and find_package {#quick_start_cmake}
==

DBus-cxx is distributed with package files appropriate for use with CMake.  When creating a new project, your CMakeLists.txt may look something like this:

```
cmake_minimum_required(VERSION 3.10)

project(find-package-test)

set(CMAKE_CXX_STANDARD 17)

include( FindPkgConfig )
pkg_check_modules( sigc REQUIRED sigc++-3.0 )
find_package(dbus-cxx COMPONENTS qt REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Threads)
    
add_executable(qtcaller qt-caller.cpp)
target_link_libraries(qtcaller dbus-cxx::dbus-cxx-qt Qt5::Core Threads::Threads ${sigc_LIBRARIES})
target_include_directories(qtcaller PRIVATE ${sigc_INCLUDE_DIRS})
target_compile_definitions(qtcaller PRIVATE QT_NO_KEYWORDS)
```

The available components are: `dbus-cxx`, `qt`, `glib` depending on the exact dispatcher that you want to use.

