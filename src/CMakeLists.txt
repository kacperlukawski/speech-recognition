# parse the subdirectory "speech"
add_subdirectory(speech)

# add the executable "Main"
add_executable(
    Main
        main.cpp
)

# connect "Main" executable with "Speech" library
target_link_libraries (Main LINK_PUBLIC Speech)