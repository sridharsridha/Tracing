cmake_minimum_required(VERSION 3.16)
project(Tracing VERSION 1.0.0
        DESCRIPTION "A C++ header only tracing implementation library"
        HOMEPAGE_URL "htts:://github.com/sridharsridha/Algorithms/tracing/"
        LANGUAGES CXX)

# define GNU standard installation directories
# note: ${CMAKE_INSTALL_INCLUDEDIR}, ${CMAKE_INSTALL_LIBDIR} and
# ${CMAKE_CURRENT_LIST_DIR} etc. are defined in GNUInstallDirs
include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(
        ${PROJECT_NAME}
        INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>/"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# set the C++ standard to the appropriate standard for using auto
target_compile_features(${PROJECT_NAME} INTERFACE
        cxx_std_17
        )

# Print the list of known compile features for this version of CMake
message("List of compile features: ${CMAKE_CXX_COMPILE_FEATURES}")

# specify the target to install (calculator library defined above)
# set the export name <name>-config (does not need to match target name)
install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-config)

# associate installed target files with the export, and then install the export
install(
        EXPORT ${PROJECT_NAME}-config # name of .cmake file
        NAMESPACE ${PROJECT_NAME}:: # set so clients must use calculator::calculator
        # to distinguish imported targets from internal ones
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}) # where the .cmake file will be installed

# copy include files to the install include directory
install(
        DIRECTORY ./
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})


