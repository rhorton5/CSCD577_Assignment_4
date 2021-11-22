
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(CarotidFlowLocal)

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
  vtkRenderingOpenGL2)

  include(${VTK_USE_FILE})
  add_executable(CarotidFlowLocal CarotidFlowLocal.cxx )
  target_link_libraries(CarotidFlowLocal ${VTK_LIBRARIES})
 
