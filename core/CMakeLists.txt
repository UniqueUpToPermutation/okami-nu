add_library(okami-core src/okami.cpp)

target_include_directories(okami-core PUBLIC include)

target_link_libraries(okami-core PUBLIC EnTT::EnTT)