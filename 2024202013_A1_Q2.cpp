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
#include <sys/mman.h>
#include <sys/types.h>
using namespace std;

ssize_t bufferSize = 1024 * 1024;
struct FileInfo {
    struct stat fileInfo;
    bool hasSameSize;
    bool reverseContent;
    bool isCreated;
};


class Error {
    public:
    string INPUT_PATH = " doesn't contain a file , provide a valid file path";
    string FORMAT_ERROR = "Error in the input given, provide the input in the following format <newfile_path> <oldfile_path> <directory_path>";
    string OPEN_ERROR = "Error in opening the file ";
    string DIFFERENT_ERROR = "Directory path given and new file parent Directory are different";

    void displayError(string msg) {
    write(1, msg.c_str(), msg.size());
    write(1, "\n", 1);
     _exit(1);
    }
};

class Permissions {
    public:
    struct stat object_info;
    bool read_per[3];
    bool write_per[3];
    bool execute_per[3];
    string obj_type;
    vector<string> permissions;

    Permissions(string& path, string& obj_type) : permissions(3, "") {
        permissions[0] = "User";
        permissions[1] = "Group";
        permissions[2] = "Others";
        this->obj_type = obj_type;
        for (int i = 0; i < 3; i++) {
            read_per[i] = false;
            write_per[i] = false;
            execute_per[i] = false;
        }
        setAllPermissions(path, obj_type);
    }

    void setAllPermissions(string& path, string& obj_type) {
        int permission_mode[3][3] = {
            {S_IRUSR, S_IWUSR, S_IXUSR}, 
            {S_IRGRP, S_IWGRP, S_IXGRP}, 
            {S_IROTH, S_IWOTH, S_IXOTH}
        };
        
        if (stat(path.c_str(), &object_info) == 0) {
            for (int i = 0; i < 3; i++) {
                read_per[i] = (object_info.st_mode & permission_mode[i][0]) != 0;
                write_per[i] = (object_info.st_mode & permission_mode[i][1]) != 0;
                execute_per[i] = (object_info.st_mode & permission_mode[i][2]) != 0;
            }
        } else {
            return;
        }
    }

    void displayPermissions() {
        string read_prompt = " has read permissions on " + obj_type + ": ";
        string write_prompt = " has write permissions on " + obj_type + ": ";
        string execute_prompt = " has execute permissions on " + obj_type + ": ";
        write(1, "\n", 1);
        for (int i = 0; i < 3; i++) {
                string read_str = permissions[i] + read_prompt + (read_per[i] ? "Yes" : "No") + "\n";
                string write_str = permissions[i] + write_prompt + (write_per[i] ? "Yes" : "No") + "\n";
                string execute_str = permissions[i] + execute_prompt + (execute_per[i] ? "Yes" : "No") + "\n";
                write(1, read_str.c_str(), read_str.size());
                write(1, write_str.c_str(), write_str.size());
                write(1, execute_str.c_str(), execute_str.size());
        }
    }
};

class Directory_info {
    public:
    FileInfo fileDetails;

    Directory_info() {
        fileDetails.hasSameSize = false;
        fileDetails.reverseContent = false;
        fileDetails.isCreated = false;
    }

    bool checkfordir(string path) {
        if (stat(path.c_str(), &fileDetails.fileInfo) == 0) {
            return (fileDetails.fileInfo.st_mode & S_IFDIR) != 0;
        }
        return false;
    }

    void displayInfo(string path) {
        string iscreated = checkfordir(path) ? "Yes":"No";
        string rev_content =  fileDetails.reverseContent ? "Yes" : "No";
        string size_content = fileDetails.hasSameSize ? "Yes" : "No";
        string  prompt1 = "Directory is created: " + iscreated + "\n";
        string prompt2 = "Whether file contents are reversed in newfile: " + rev_content + "\n";
        string prompt3 = "Both Files Sizes are Same : " +  size_content + "\n";

        write(1, prompt1.c_str(), prompt1.size());
        write(1, prompt2.c_str(), prompt2.size());
        write(1, prompt3.c_str(), prompt3.size());
    }
};

int getFileDescriptor(string path) {
    Error error;
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        path = "Path: " + path;
        error.displayError(path + error.INPUT_PATH);
    }
    return fd;
}


bool checkChunks(char inputBuffer[], char outputBuffer[], int n) {
     for (int i = 0; i < n; i++) {
        if (inputBuffer[i] != outputBuffer[n - i - 1]) {
            return false;
        }
    }
    return true;
}
bool isReversed(int old_file_fd, int new_file_fd, ssize_t fileSize) {
        ssize_t bytes_to_read = bufferSize;
        string percentage;
        int rem_chunk = (fileSize) % bufferSize;
        int err;
        char inputBuffer[bufferSize], outputBuffer[bufferSize];

        long long rev_start_offset = fileSize - bufferSize;
        long long start_offset = 0;
        while (start_offset < fileSize - bufferSize && rev_start_offset >=0) {
            
            //old file//
            lseek(old_file_fd, start_offset, SEEK_SET);
            read(old_file_fd, inputBuffer, bytes_to_read);

            //new file//
            lseek(new_file_fd, rev_start_offset, SEEK_SET);
            read(new_file_fd, outputBuffer, bytes_to_read);
            if (!checkChunks(inputBuffer, outputBuffer,bufferSize)) return false;
            start_offset += bufferSize; 
            rev_start_offset = rev_start_offset - bufferSize;
        }
        if (rem_chunk) {
            start_offset = fileSize - rem_chunk;
            rev_start_offset = 0;
            //old file//
             lseek(old_file_fd, start_offset, SEEK_SET);
            read(old_file_fd, inputBuffer, rem_chunk);


            //new file//
            lseek(new_file_fd, 0, SEEK_SET);
            read(new_file_fd, outputBuffer, rem_chunk);

            if (!checkChunks(inputBuffer, outputBuffer, rem_chunk)) return false;
        }

    return true;
}

ssize_t getfilesize(const std::string& input_file_path) {
    struct stat fileInfo;
    if (stat(input_file_path.c_str(), &fileInfo) == 0) {
        off_t fileSize = fileInfo.st_size;
        return fileSize;
    } else {
        string msg = "Error in getting the file size";
        Error error;
        error.displayError(msg);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    string directory_path;
    string old_file_path;
    string new_file_path;

    Error error;

    if (argc != 4 ) {
        error.displayError(error.FORMAT_ERROR);
    }
    new_file_path = argv[1];
    old_file_path = argv[2];
    directory_path = argv[3];

    char inputBuffer[bufferSize];
    char outputBuffer[bufferSize];
    int old_fd = getFileDescriptor(old_file_path);
    int new_fd = getFileDescriptor(new_file_path);
    ssize_t old_fileSize = getfilesize(old_file_path);
    ssize_t new_fileSize = getfilesize(new_file_path);
    bool hasSameSize = old_fileSize == new_fileSize;
    bool isreversed = hasSameSize ? isReversed(old_fd, new_fd, old_fileSize) : false;
    Directory_info dir_info;
    dir_info.fileDetails.hasSameSize = hasSameSize;
    dir_info.fileDetails.reverseContent = isreversed;
    dir_info.displayInfo(directory_path);

    //Permissions display//
    vector<string>obj_types = {"new file", "old file", "directory"};
    vector<string>paths = {new_file_path, old_file_path, directory_path };

    for (int i = 0; i < 3; i++) {
        Permissions obj_per(paths[i], obj_types[i]);
        obj_per.displayPermissions();
    }

    return 0;

}