set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
	dbus_manager.hpp
	dbus_manager.cpp
	main.cpp
	main_window.hpp
	main_window.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(dbus REQUIRED IMPORTED_TARGET dbus-1)

add_executable(
	${PROJECT_NAME}
	${PROJECT_SOURCES}
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	PkgConfig::GTKMM
	PkgConfig::dbus
)
