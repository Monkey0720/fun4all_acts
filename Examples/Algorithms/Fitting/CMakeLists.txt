add_library(
  ActsExamplesFitting SHARED
  src/FittingAlgorithm.cpp
  src/TrkrClusterFittingAlgorithmFitterFunction.cpp
  src/TrkrClusterFindingAlgorithmFinderFunction.cpp
  src/FittingAlgorithmFitterFunction.cpp)
target_include_directories(
  ActsExamplesFitting
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ActsExamplesFitting
  PUBLIC
    ActsCore
    ActsExamplesFramework ActsExamplesMagneticField
    Boost::program_options)

install(
  TARGETS ActsExamplesFitting
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})