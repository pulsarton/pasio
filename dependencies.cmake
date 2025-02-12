# required project dependencies here

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(asio CONFIG REQUIRED)
find_package(Pybind11 CONFIG REQUIRED)

message(STATUS "[pasio] Python3: ${Python3_EXECUTABLE}")