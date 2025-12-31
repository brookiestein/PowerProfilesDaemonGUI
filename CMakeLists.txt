cmake_minimum_required(VERSION 3.27)

project(
	PowerProfilesDaemonGUI
	VERSION 0.5.0
	LANGUAGES CXX
    DESCRIPTION "A basic GUI for the power-profiles-daemon utility."
)

set(PRETTY_NAME "Power Profiles Daemon GUI")

add_subdirectory(src)
