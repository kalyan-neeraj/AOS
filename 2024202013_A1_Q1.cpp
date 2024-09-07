#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <cstring>
#include<iostream>
#include <string>
#include <vector> 
#include <cstdlib>
using namespace std;


//PATHS//
string DIR_PATH = "./Assignment1/";
//Error Msgs//
string NO_INDEX = "provide the start and end indices";
string FLAG_ERROR_0 = "Error in flag 0 format: <input_file> 0";
string FLAG_ERROR = "Error flag can only be 0 or 1";

string FORMAT = "provide the argument in proper format of <input_file> <flag 0 or 1> <start_index> <end_index>";
string WRONG_INDEX_1 = "start index should be more than than end index";
string WRONG_INDEX_2 = "indices cannot be negative";
string WRONG_INDEX_3 = "end index cannot be more than file size ";

string WRITE = "unable to write the file";
string READ = "unable to read the file";
string DIR = "unable to create the directory";
string INPUT_PATH = "Input file doesnt exist in the give Path: ";
string CREATE_FILE = "Error while file creation";

//Error displayer//
void displayError(string msg) {
    write(1, msg.c_str(), msg.size());
    write(1, "\n", 1);
     _exit(1);
}


string getFileName(string path) {
    ssize_t lastSlash = path.find_last_of('/');
    if (lastSlash == -1) return path;
    else return path.substr(lastSlash + 1, path.size());

}

void  create_directory() {
    ssize_t err = mkdir(DIR_PATH.c_str(), 0700);
    if (err == -1 && errno != EEXIST) {
        displayError(DIR);
    }
}

long long create_file(const char* path) {
    long long fd =  open(path,  O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd == -1) {
        displayError(CREATE_FILE);
    }
    return fd;
}

long long open_file(string path) {
    long long fileNo =  open(path.c_str(), O_RDONLY);
    if (fileNo == -1) {
        displayError(INPUT_PATH + path);
    }
    return fileNo;
}

ssize_t getfilesize(const std::string& input_file_path) {
    struct stat fileInfo;

    if (stat(input_file_path.c_str(), &fileInfo) == 0) {
        off_t fileSize = fileInfo.st_size;
        return fileSize;
    } else {
        string msg = "Error in getting the file size";
        write(1, msg.c_str(), msg.size());
        write(1, "\n", 1);
        _exit(1);
    }
}

long long convert_to_ssize(char* s) {
    long long n = strlen(s);
    if (s[0] == '-' )  {return -1;}
    long long value = 0;
    long long power_10 = 1;
    for(ssize_t i = n-1; i>=0; i--) {
        if (s[i] >= '0' && s[i] <= '9') {
            value += (s[i] - '0') * (power_10);
            power_10 *= 10;
        }
    }
    return value;
}

void displayProgress(ssize_t data_written, ssize_t fileSize, string& percentage ) {
    ssize_t  progress = (data_written  * 100) / fileSize;
    string cleaner_string(100, ' ');
    percentage   = to_string(progress) + "%";
    write(1, "\r", 1);
    write(1, cleaner_string.c_str(), 100);
    write(1, "\r", 1);
    write(1, percentage.c_str(), percentage.size());
}


void reverse_chunks(char* buffer, ssize_t chunk_size) {
    ssize_t i = 0;
    ssize_t j = chunk_size-1;
    ssize_t n = i - (i-j)/2;
    while(i <= n) {
        char temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
        i++;
        j--;
    }
}

ssize_t write_to_file_with_given_size(ssize_t fd, ssize_t writefile_fd ,ssize_t offSet, ssize_t writeOffset,char* buffer, ssize_t bytes_to_read, ssize_t rev) {
        lseek(fd, offSet, SEEK_SET);
        lseek(writefile_fd, writeOffset, SEEK_SET); 
         ssize_t curr_read = read(fd, buffer, bytes_to_read);
         if (curr_read == -1) {
            displayError(READ);
         }
         if (rev) reverse_chunks(buffer, curr_read);
         ssize_t write_fd =  write(writefile_fd, buffer, curr_read);
         if (write_fd == -1) {
            displayError(WRITE);
         }
         return write_fd;

}

ssize_t traversing_the_file(ssize_t fd, ssize_t writefile_fd, ssize_t fileSize, ssize_t end, ssize_t bufferSize, ssize_t rev, char* buffer, ssize_t total_bytes_written) {
        ssize_t offSet = fileSize;
        ssize_t bytes_to_read = bufferSize;
        string percentage;
        ssize_t rem_chunk = (fileSize - end) % bufferSize;
        ssize_t err;

        for (ssize_t offSet = fileSize-bufferSize; offSet>=end; offSet-=bytes_to_read) {
            ssize_t data_written = write_to_file_with_given_size(fd, writefile_fd, offSet, total_bytes_written, buffer, bytes_to_read, rev);
                total_bytes_written += data_written;
                displayProgress(total_bytes_written, fileSize, percentage);
                err = write(1, "\r", 1);
                if (err == -1) {
                    displayError(WRITE);
                }
                err = write(1, percentage.c_str(), percentage.size());
                if (err == -1) {
                    displayError(WRITE);
                }
        }

        // if rem_chunk exists read the last chunk and write the file//
        if (rem_chunk > 0) {
            ssize_t data_written = write_to_file_with_given_size(fd, writefile_fd, end, total_bytes_written,buffer, rem_chunk, rev);
            total_bytes_written += data_written;
            displayProgress(total_bytes_written, fileSize, percentage);
        }
    return total_bytes_written;
}

int main( int argc, char *argv[]) {
    string input_file;
    ssize_t curr_chunk;
    long long start = 0;
    long long  end = 0;


    //bytes//
    ssize_t bufferSize = 1024 * 1024;
    char buffer[bufferSize];
    off_t fileSize;

    if (argv[2] == "1" && argc < 5) {
        displayError(NO_INDEX);
    } else if (argc < 3) {
        displayError(FORMAT);
    }


    //Get input file name from the input path//
    string input_file_path = argv[1];
    input_file = getFileName(input_file_path);


    ssize_t flag = convert_to_ssize(argv[2]);

    if (flag != 0 && flag != 1) {
        displayError(FLAG_ERROR);
    }

    if (!flag && argc > 3) {
        displayError(FLAG_ERROR_0);
    }

    if (flag) {
        if (argc != 5) displayError(FORMAT);
        start = convert_to_ssize(argv[3]);
        end = convert_to_ssize(argv[4]);
    }

    if (start < 0 || end< 0) {
        displayError(WRONG_INDEX_2);
    }

    if (start > end) {
        displayError(WRONG_INDEX_1);
    }

    //input file//
    ssize_t fd = open_file(input_file_path);
    fileSize = getfilesize(input_file_path);
    if (end > fileSize) {
        displayError(WRONG_INDEX_3);
    }

    if (end > fileSize) {
        displayError(WRONG_INDEX_3);
    }

    if (!fileSize) {
        string percentage = "100%";
        write(1, percentage.c_str(), percentage.size());
        _exit(1);
    }

    //file dir//
    create_directory();

    //output_file//
    string output_file = DIR_PATH + to_string(flag) + "_" + input_file;
    ssize_t writefile_fd = create_file(output_file.c_str());
    size_t total = 0;
    ssize_t total_bytes_written = 0;

    if (!flag) {
        //traversing file and reversing the chunks//
    total_bytes_written =   traversing_the_file(fd, writefile_fd, fileSize, 0, bufferSize, 1, buffer, 0);

        }
    else {
        //left half reversal//
            total_bytes_written =  traversing_the_file(fd, writefile_fd, start, 0, bufferSize, 1, buffer, total_bytes_written);
        //middle chunk reversal//
            ssize_t midChunk = abs(end - start)+1;
            ssize_t rem_chunk = midChunk % bufferSize;
            ssize_t bytes_to_read = bufferSize;
            string percentage;  
            ssize_t offSet;                                                                                                                                                                    
            for (offSet = start; offSet<=end-bytes_to_read + 1; offSet+=bytes_to_read) {
                ssize_t data_written = write_to_file_with_given_size(fd, writefile_fd, offSet, total_bytes_written,buffer, bytes_to_read, 0);
                total_bytes_written += data_written;
                displayProgress(total_bytes_written, fileSize, percentage);
                write(1, "\r", 1);
                write(1, percentage.c_str(), percentage.size());
            }
            if (rem_chunk > 0) {
            ssize_t data_written = write_to_file_with_given_size(fd, writefile_fd, offSet, total_bytes_written, buffer, rem_chunk, 0);
            total_bytes_written += data_written;
            displayProgress(total_bytes_written, fileSize, percentage);
            write(1, "\r", 1);
            write(1, percentage.c_str(), percentage.size());

            }
    //right half  reversal//
            traversing_the_file(fd, writefile_fd, fileSize, end+1, bufferSize, 1, buffer, total_bytes_written);                                                                                                                                                       
        }    
    write(1, "\n", 1);
    close(fd);
    close(writefile_fd);
    
}

