# include the entry directory of this library
include_directories(../util)

aux_source_directory(simple_test_src SIMPLE_TEST_SRC)

add_executable(simple_test ${SIMPLE_TEST_SRC})
TARGET_LINK_LIBRARIES(simple_test pthread)
