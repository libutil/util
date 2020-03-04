aux_source_directory(simple_test-src SIMPLE_TEST_SRC)

add_executable(simple_test ${SIMPLE_TEST_SRC})
target_link_libraries(simple_test pthread ${Boost_LIBRARIES})