cmake_minimum_required(VERSION 3.0)

aux_source_directory(. CLIENT_LIST)

add_executable(ChatClient ${CLIENT_LIST})
target_link_libraries(ChatClient mysqlclient pthread)