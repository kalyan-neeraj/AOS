## General Instructions: 
1. Make sure to run the codes on a linux machine.
2. Open the project folder 2024202013_A1 folder.
3. For instance if the project is in desktop folder
- open the bash and type `cd ./desktop/2024202013_A1` 
4. Before execution Compile the programs using:
     - question 1: `g++ 2024202013_A1_Q1.cpp`    (write it in the terminal)
     - question 2: `g++ 2024202013_A1_Q2.cpp`


## Question 1:
## Executing the file ##
1. To execute the compiled program in the terminal do the following
  - `./a.out <input_file> <flag 0 or 1> <start_index> <end_index>`
      (flag 0 no indices are required, flag 1 indices must be withing range of file and numeric)
   ## Example
  - For flag 0 : `./a.out input.txt 0`
      generates an output file in `0_input.txt` in Assignment1 directory 
  - For flag 1 " `./a,out input.txt 1 9 22`
       generates an output file in `1_input.txt` in Assignment1 directory 

## Overview:
1. Project takes an input file and reveres the content (or partially reverses the file) and writes the content to an output file.

# Function Descriptions and Flow
## Program Flow:
1. Arguments from the command line are extracted from `argv` in the `main`
2. Input file is read and an empty output file is created.
3. Depending on the flag (0 or 1), it calls `traversing_the_file`:
   - For flag 0: Reverses the entire file in chunks.
   - For flag 1: 
     a. Reverses the left half (before start index)
     b. Copies the middle chunk without reversal
     c. Reverses the right half (after end index)
4. If reversal is needed, `reverse_chunks` is called on each chunk before writing.
5. Progress is displayed using `displayProgress` throughout the process.
6. During execution, any error encountered is taken care by `displayError` and program execution is stopped.
7. The program closes files and exits after completion.


## Question 2:
## Executing the file ##
1. To execute the compiled program in the terminal do the following
  - `./a.out <new_file_path> <old_file_path> <directory_path>`
   ## Example
  - `./a.out ./Assignment1/1_input.txt input.txt Assignmen1`
      displays information regarding permissions of the files and directory and regarding if file content are reverse of each other
  -before execution ensure that the paths provided are valid and in new_file_path, file exists in the directory you have given in path.


## Assumptions:
1. We need to ensure proper file paths for new_file and old_file.
2. If we encounter any error in the filepaths, program is terminated.
3. If the directory doesnt exist in the given path, we  will  display all `No` for all permissions.
## Example:
 The new_path is `./Assignment1/0_input.txt` and directory is `./Assignment2`
 assuming that `Assignment2` directory doenst exist program will run normally, but will not display any permissions realted to the directory, will simply display all `No` in all permisisons 

## Overview:
1. Project deals with displaying permissions.
# Function Descriptions and Flow
## Classes:
1. `Error`
   - Stores error messages and provides a method to display errors
2. `Permissions`
   - Handles file/directory permissions
   - Methods:
     - `setAllPermissions`: Sets read, write, and execute permissions for user, group, and others
     - `displayPermissions`: Displays the permissions for a file or directory
3. `Directory_info`
   - Manages information about directories and files
   - Methods:
     - `checkfordir`: Checks if a given path is a directory
     - `displayInfo`: Displays information about file creation, content reversal, and size comparison.
## Program Flow:
1. `main` function takes command-line arguments for new file path, old file path, and directory path.
2. It opens both the new and old files using `getFileDescriptor`.
3. File sizes are obtained using `getfilesize`.
4. The program checks if both files have the same size.
5. If sizes are the same, it calls `isReversed` to check if the content of the new file is a reversed version of the old file.
6. A `Directory_info` object is created to store and display information about the files and directory.
7. The program then creates `Permissions` objects for the new file, old file, and directory.
8. Permissions for each object are displayed using the `displayPermissions` method.

