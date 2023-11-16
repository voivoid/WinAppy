find_program(ClangFormat clang-format)
if(ClangFormat)
  set(Targets WinAppy BasicWinApp)
  foreach(Target ${Targets})
    get_target_property(TargetSources ${Target} SOURCES)
    get_target_property(TargetDir ${Target} SOURCE_DIR)
    foreach(Src ${TargetSources})
      list(APPEND Sources "${TargetDir}/${Src}")
    endforeach()
  endforeach()

  list(FILTER Sources INCLUDE REGEX "^.*\.(h|cpp)\$")

  add_custom_target(clangformat
    COMMAND clang-format -style=file:${CMAKE_CURRENT_SOURCE_DIR}/.clang-format -i ${Sources}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
