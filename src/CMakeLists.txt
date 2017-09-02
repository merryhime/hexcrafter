set(SRCS
    lexer.cpp
    match.cpp
    )

set(HEADERS
    container_util.hpp
    generator.hpp
    lexer.hpp
    match.hpp
    peekable_generator.hpp
    source_location.hpp
    token.hpp
    )

include(CreateDirectoryGroups)
create_directory_groups(${SRCS} ${HEADERS})

add_library(hexcrafter_lib ${SRCS} ${HEADERS})
target_include_directories(hexcrafter_lib PUBLIC .)
target_compile_options(hexcrafter_lib PRIVATE ${HEXCRAFTER_CXX_FLAGS})
target_link_libraries(hexcrafter_lib PUBLIC gsl)

add_executable(hexcrafter main.cpp)
target_compile_options(hexcrafter PRIVATE ${HEXCRAFTER_CXX_FLAGS})
target_link_libraries(hexcrafter PRIVATE hexcrafter_lib)
