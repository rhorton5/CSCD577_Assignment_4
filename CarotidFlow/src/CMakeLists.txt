
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(CarotidFlow)

find_package(VTK COMPONENTS 
  vtkCommonColor
  vtkCommonCore
  vtkCommonDataModel
  vtkFiltersCore
  vtkFiltersFlowPaths
  vtkFiltersModeling
  vtkFiltersSources
  vtkIOLegacy

  vtkRenderingAnnotation
  vtkRenderingLabel

  vtkInteractionStyle
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingOpenGL2 QUIET)
if (NOT VTK_FOUND)
  message("Skipping CarotidFlow: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(CarotidFlow MACOSX_BUNDLE CarotidFlow.cxx )
  target_link_libraries(CarotidFlow PRIVATE ${VTK_LIBRARIES})
else ()
  # include all components
  add_executable(CarotidFlow MACOSX_BUNDLE CarotidFlow.cxx )
  target_link_libraries(CarotidFlow PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS CarotidFlow
    MODULES ${VTK_LIBRARIES}
    )
endif () 
