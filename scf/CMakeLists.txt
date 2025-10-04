add_library(scf STATIC)
target_sources(scf PRIVATE table.c lexer.c scf.c)
target_link_libraries(scf PRIVATE SDL3::SDL3)
target_include_directories(scf INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(table_test)
target_sources(table_test PRIVATE table_test.c)
target_link_libraries(table_test PRIVATE SDL3::SDL3 cmocka::cmocka scf)
add_test(NAME table_test COMMAND table_test)

add_executable(lexer_test)
target_sources(lexer_test PRIVATE lexer_test.c)
target_link_libraries(lexer_test PRIVATE SDL3::SDL3 cmocka::cmocka scf)
add_test(NAME lexer_test COMMAND lexer_test)
