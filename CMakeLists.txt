cmake_minimum_required(VERSION 3.9)
project("jackal-utilities")

find_package(common-cmake REQUIRED)

# Set the default install location.
SetDefaultInstallPrefix("${CMAKE_SOURCE_DIR}/bin")

# Load the project version from a file 'VERSION'
SetProjectVersionFromFile(JKUTIL ${CMAKE_CURRENT_LIST_DIR}/meta/version/VERSION)

# Enable target groups for the Visual Studio IDE
EnableGroups()

# Enable Testing
EnableTests()

# Generate Documentation
add_subdirectory("${CMAKE_SOURCE_DIR}/meta/doxy")

# Main Library
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src/jkutil")

# Tests
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/jkutil_tests")

# Installs the project's targets
SetupExport("JKUtil")
InstallExport("JKUtil"
	NAMESPACE "JKUtil")