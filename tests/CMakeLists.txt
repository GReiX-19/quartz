find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(test_tagreader test_tagreader.cpp)
target_link_libraries(test_tagreader PRIVATE Qt::Test backend)
target_compile_definitions(test_tagreader PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_test(NAME tagreader COMMAND test_tagreader)