add_library(scf STATIC)
target_sources(scf PRIVATE table.c scf.c)
target_link_libraries(scf PRIVATE SDL3::SDL3)
target_include_directories(scf INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
