# required project dependencies here

find_program(UV NAMES uv uv.exe)
execute_process(COMMAND ${UV} python find --system OUTPUT_VARIABLE Python3_EXECUTABLE OUTPUT_STRIP_TRAILING_WHITESPACE)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(asio CONFIG REQUIRED)
find_package(Pybind11 CONFIG REQUIRED)

message(STATUS "[pasio] Python3: ${Python3_EXECUTABLE}")