# parse the subdirectories
add_subdirectory(speech)

# add the executable "Main"
add_executable(
    Main
        main.cpp
)

# add the executable "Filters"
add_executable(
    Filters
        filters_test.cpp
)

# connect "Main" executable with "Speech" library
target_link_libraries(Main LINK_PUBLIC Speech)
target_link_libraries(Filters LINK_PUBLIC Speech)