cmake_minimum_required(VERSION 3.0)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")


add_executable(cpg_test cpg_test.cpp ../src/nn.cpp)
target_include_directories(cpg_test PUBLIC ../include ../../include)
target_link_libraries(cpg_test "${TORCH_LIBRARIES}" apear)

add_executable(rbf_test rbf_test.cpp ../src/nn.cpp)
target_include_directories(rbf_test PUBLIC ../include)
target_link_libraries(rbf_test "${TORCH_LIBRARIES}")

add_executable(cpgrbf_test cpgrbf_test.cpp ../src/nn.cpp)
target_include_directories(cpgrbf_test PUBLIC ../include)
target_link_libraries(cpgrbf_test "${TORCH_LIBRARIES}")

add_executable(cpgrbfrnn_test cpgrbfrnn_test.cpp ../src/nn.cpp)
target_include_directories(cpgrbfrnn_test PUBLIC ../include)
target_link_libraries(cpgrbfrnn_test "${TORCH_LIBRARIES}")


# add_executable(pibb_test tests/pibb_test.cpp src/pi_bb.cpp src/pi_bb_elite.cpp)
# target_include_directories(pibb_test PUBLIC include ../EAFramework/include)
# target_link_libraries(pibb_test "${TORCH_LIBRARIES}" ARE)
