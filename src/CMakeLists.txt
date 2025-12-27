set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(RESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/resources.xml")
set(RESOURCE_SRC "${CMAKE_CURRENT_BINARY_DIR}/resources.c")

add_custom_command(
	OUTPUT ${RESOURCE_SRC}
	COMMAND glib-compile-resources
	--generate-source
	--target=${RESOURCE_SRC}
	--sourcedir=${CMAKE_SOURCE_DIR}
	${RESOURCE_XML}
	DEPENDS ${RESOURCE_XML}
)

set(PROJECT_SOURCES
	dbus_manager.hpp
	dbus_manager.cpp
	main.cpp
	main_window.hpp
	main_window.cpp
	${RESOURCE_SRC}
)

set(DESKTOP_FILE assets/com.github.brookiestein.powerprofilesdaemongui.desktop)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(dbus REQUIRED IMPORTED_TARGET dbus-1)

execute_process(
    COMMAND sed -i "s|Exec=.*|Exec=${CMAKE_INSTALL_PREFIX}/bin/powerprofilesgui|g" ${DESKTOP_FILE}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

configure_file(config.hpp.in config.hpp)

add_executable(
	powerprofilesgui
	${PROJECT_SOURCES}
)

target_link_libraries(
	powerprofilesgui
	PRIVATE
	PkgConfig::GTKMM
	PkgConfig::dbus
)

include(GNUInstallDirs)

install(TARGETS powerprofilesgui
	BUNDLE DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
	${CMAKE_SOURCE_DIR}/${DESKTOP_FILE}
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications
)

install(DIRECTORY
	${CMAKE_SOURCE_DIR}/assets/icons/hicolor
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons
)
