// MY HEAD FILE

#include <iostream> //all the needed libraries
#include <vector>
#include <string>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>

#define INTERACTING 1 // showing that the shell is alive and  we re reading or executing

#define ARG 0    // expecting args while reading commands and parsing
#define INPUT 1  // expecting input file while reading commands and parsing
#define OUTPUT 2 // expecting output while reading commands and parsing
#define CAT 3    // expecting cat file (no cat args) while reading commands and parsing

#define READ 0
#define WRITE 1

using namespace std;
typedef struct myFiles_ myFiles;
struct myFiles_ // struct to keep files from the command line
{
    int myType; // 1: if it is an input file, 2: output, 3: cat
    string myString;
};

typedef struct myCommands_ myCommands; // struct to keep seperately the arguments (cat, sort, ls,..) and the files of a command
struct myCommands_
{ // useful iin case of a pipe
    vector<string> p_arguments;
    vector<myFiles> p_files;
    bool isPipe; // 1: if we see the character '|' and 0: if not
    bool isBack;
};

typedef struct aliases_ alias; // struct to keep and alias (Keeping together its name and its command)
struct aliases_
{
    string aliasName;    // save its name - after the "createalias" word
    string aliasCommand; // save the commad between the " "
};

// FUNCTIONS
bool isMatch(string s, string p);
void saveCommand(bool is_pipe, vector<string> *parsedArguments, vector<myFiles> *parsedFiles, myCommands *parsedCommand, vector<myCommands> *parsedCommands);
