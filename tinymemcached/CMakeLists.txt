project (tinymemcached)

set(INC_DIR ./)
include_directories(${INC_DIR})

aux_source_directory(. DIR_SRCS)

add_executable(tinymemcached ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} event)
