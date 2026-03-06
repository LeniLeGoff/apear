cmake_minimum_required(VERSION 3.0)
project(apear)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
# message(${SOURCES})
add_compile_options(-DEIGEN_DONT_PARALLELIZE)

add_library(apear SHARED ${SOURCES})
target_include_directories(apear PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    "/usr/include/eigen3"
)
target_link_libraries(apear tbb) #Boost::serialization Boost::filesystem)
target_compile_options(apear PRIVATE -Wall -Werror)




install(
  TARGETS apear
  EXPORT apearTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)


install(
  EXPORT apearTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/apear
  NAMESPACE apear::
  FILE apearTargets.cmake # Not sure if this is still needed
  )


include(CMakePackageConfigHelpers)
configure_package_config_file(
  "cmake/apearConfig.cmake.in"
  "apearConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/apear"
  )

write_basic_package_version_file(
  "apearConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY AnyNewerVersion
  )

### Install Config and ConfigVersion files
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/apearConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/apearConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/apear"
  )

