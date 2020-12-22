cmake_minimum_required(VERSION 3.12)
project(OrderMatchingEngine)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

add_executable(OrderMatchingEngine source/main.cpp source/User.cpp source/User.h source/Order.cpp source/Order.h source/Trade.cpp source/Trade.h source/TradeEngine.cpp source/TradeEngine.h test/test.cpp test/test.h test/testConcurrency.h test/testConcurrency.cpp)
