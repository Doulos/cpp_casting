# CPP casting

This directory contains code related to an article of the same title in the Doulos KnowHow section of the Doulos website. Doulos provides training on a variety of topics including C++.

Written by David C Black, Senior Member Technical Staff at Doulos.<br/>

[<img src="https://doulos.com/media/1009/doulos-logo-header.svg" width=80 style="vertical-align:bottom"/>**<big>/knowhow</big>**]( http://www.doulos.com/knowhow/) 

Search [Doulos KnowHow](https://www.doulos.com/knowhow) for the very latest information.

## How to build

You can likely just compile the cpp_casting.cpp file with your normal switches; however, we recommend using all warnings and pedantic options available to you and suppressing nothing. The accompanying CMakeList.txt and related cmake/*.cmake will do this rather simply.

Tools required:

To use cmake, you will of course need have cmake available. We assume version 3.21 or better. Go to https://cmake.org for more information.

Linux/macos commands to do the work:

```bash
# Configure and generate
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug

# Compile and link
cmake --build build/debug --config Debug
# You should see at least two warnings

# Execute the test
ctest --test-dir build/debug -C Debug -VV
```

Enjoy!

