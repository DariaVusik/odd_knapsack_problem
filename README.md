# "Odd" Knapsack Problem

## Steps

1. Download the OR-Tools library from the official Google Developers website:
   [https://developers.google.com/optimization/install](https://developers.google.com/optimization/install)

**Note:** OR-Tools version 9.10.4067 is currently used, other versions may not be working

2. Unpack the downloaded files to a directory of your choice.

3. Generate the solution file using CMake, and specify the `CMAKE_PREFIX_PATH` to target the directory where you unpacked the OR-Tools library:  
```cmake -DCMAKE_PREFIX_PATH=/path/to/or-tools/directory .```  
This will generate the necessary build files for your platform.

4. Now you can build the OR-Tools library using the generated build files.

5. Run program specifying input and output files, e.g. for Windows:  
```./odd_knapsack_problem.exe /path/to/input.txt /path/to/output.txt ```

**Note:** This steps have been tested on Microsoft Visual Studio Community 2022 (64-bit) - Current Version 17.10.1.
