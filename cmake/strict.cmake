if (MSVC)
    # warning level 4
    add_compile_options( /W4 )
else()
    # lots of warnings
    add_compile_options( -Wall -Wextra -pedantic )
endif()
