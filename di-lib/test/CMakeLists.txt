cmake_minimum_required(VERSION 3.10)

# Test for when autoregistration is disabled
project(di-test)
test_setup()
configure_test("cadf::di")

# Test for when autoregistration is enabled
project(di-test-autoregistration)
test_setup()
configure_test("cadf::di")
target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_BEAN_AUTOREGISTRATION)
