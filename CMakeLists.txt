
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(PeekMedDICOMViewer)

find_package(VTK COMPONENTS
  vtkCommonColor
  vtkCommonCore
  vtkCommonDataModel
  vtkIOImage
  vtkIOXML
  vtkImagingCore
  vtkInteractionImage
  vtkInteractionStyle
  vtkInteractionWidgets
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingOpenGL2
  vtkRenderingVolume
  vtkRenderingVolumeOpenGL2 QUIET)
if (NOT VTK_FOUND)
  message("Skipping PeekMedDICOMViewer: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(PeekMedDICOMViewer MACOSX_BUNDLE PeekMedDICOMViewer.cxx )
  target_link_libraries(PeekMedDICOMViewer PRIVATE ${VTK_LIBRARIES})
else ()
  # include all components
  add_executable(PeekMedDICOMViewer MACOSX_BUNDLE PeekMedDICOMViewer.cxx )
  target_link_libraries(PeekMedDICOMViewer PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS PeekMedDICOMViewer
    MODULES ${VTK_LIBRARIES}
    )
endif ()
