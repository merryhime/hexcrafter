set(SRCS
    main.cpp
    )

include(CreateDirectoryGroups)
create_directory_groups(${SRCS})

add_executable(hexcrafter_tests ${SRCS})
target_link_libraries(hexcrafter_tests PRIVATE catch hexcrafter_lib)
target_include_directories(hexcrafter_tests PRIVATE . ../src)
target_compile_options(hexcrafter_tests PRIVATE ${DYNARMIC_CXX_FLAGS})

add_test(hexcrafter_tests hexcrafter_tests)
