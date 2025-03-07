#include "mysh.h"

pid_t global_pid = -1; // id of the currently running process

void controlC(int signum)
{
    signal(SIGINT, controlC); // if ^C, call function controlC
    kill(global_pid, SIGINT); // send ^C signal to the currently running process
    cout << endl;
}

void controlZ(int signum)
{
    signal(SIGTSTP, controlZ); // if ^Z, call function controlZ
    kill(global_pid, SIGTSTP); // send ^Z signal to the currently running process
    cout << endl;
}

int main(int argc, char *argv[])
{
    // SIGNAL HANDLING
    signal(SIGINT, controlC);  // if ^C, call function controlC
    signal(SIGTSTP, controlZ); // if ^Z, call function controlZ

    // ALIAS AND MYHISTORY INITIALIZATIONS
    vector<alias> aliases;
    vector<string> histories;
    int execHistory = -1;
    int execAlias = -1;

    while (INTERACTING)
    {
        bool isHistory = 0;
        bool isAlias = 0;

        string command;
        // FINDING THE COMMAND
        if (execAlias > -1) // if the flag: execAlias > -1 we have to take the command from the aliases array and not from the standard input
        {
            command = aliases[execAlias].aliasCommand;
            execAlias = -1;
        }
        else if (execHistory > -1) // same as alias handling
        {
            command = histories[histories.size() - execHistory];
            execHistory = -1;
        }
        else
        {
            cout << "in-mysh-now:>";
            getline(cin, command); // getting the whole line of the command
        }

        command.push_back(' '); // pushing a space at the back of the command. it will be useful when restore after parsing

        string argument;                   // string to restore currently an argument
        vector<string> parsedArguments;    // vector to restore all the command arguments we read
        myFiles parsedFile;                // struct to restore currently a file
        vector<myFiles> parsedFiles;       // vector to restore all the command files we read
        myCommands parsedCommand;          // we restore currently every argument with its files(if there are any)
        vector<myCommands> parsedCommands; // we restore the whole command line parsed

        int flag = ARG; // 0: if we're expecting an argument, 1: input file, 2: output file, 3: cat file
                        // PARSING BEGINS
        for (int i = 0; i < command.length(); i++)
        {
            if (command[i] == '"') // alias handling
            {
                isAlias = !isAlias;        // change alias flag whether we expect an alias name or not
                if (argument.length() > 0) // we have read the command between the " "
                {
                    parsedArguments.push_back(argument);
                    argument.clear();
                }
                continue;
            }
            else if (isAlias == 1) // now we're reading the alias name
            {
                argument.push_back(command[i]);
                continue;
            }
            else if (command[i] != ' ' && command[i] != '>' && command[i] != '<' && command[i] != ';' && command[i] != '|') // case we dont read a special character or an alias
            {
                if (flag == ARG)                    // we're expecting an argument
                    argument.push_back(command[i]); // store it temporarily until the whole argument is read
                else                                // we' re expecting a file(input/output/cat)
                {
                    parsedFile.myString.push_back(command[i]); // store it
                    parsedFile.myType = flag;                  // INPUT - OUTPUT - CAT
                }
            }
            else
            {
                if (flag == ARG && argument.length() > 0) // the argument is read
                {
                    parsedArguments.push_back(argument); // store it with the other arguments
                    argument.clear();                    // clear to be prepared to read the next argument
                }
                else if (parsedFile.myString.length() > 0) // the file is read
                {
                    parsedFiles.push_back(parsedFile);
                    parsedFile.myString.clear();
                    flag = ARG; // we're going to expect an argument again
                }

                if (command[i] == '<') // case of expecting an input file
                    flag = INPUT;      // input file

                else if (command[i] == '>')
                {
                    if (command[i + 1] == '>') // case of expecting an cat file
                    {
                        flag = CAT; // cat
                        i++;        // we know the next char, we can pass it
                    }
                    else               // case of expecting an output file
                        flag = OUTPUT; // output file
                }
            }

            if (command[i] == '|' || command[i] == ';') // in case of a pipe restore all the arguments and files read by now and prepare for the next ones after the pipe
                saveCommand(command[i] == '|', &parsedArguments, &parsedFiles, &parsedCommand, &parsedCommands);
        }
        // PARSING ENDS
        if (parsedArguments.size() > 0) // Final save
            saveCommand(0, &parsedArguments, &parsedFiles, &parsedCommand, &parsedCommands);

        if (parsedCommands.size() > 0) // personal addition to terminate mysh
            if (parsedCommands[0].p_arguments.size() > 0)
                if (parsedCommands[0].p_arguments[0] == "exit")
                    exit(1);

        // CREATE ALIAS COMMAND
        if (parsedCommands[0].p_arguments[0] == "createalias" && parsedCommands[0].p_arguments.size() == 3) // check if coomand's first arg is "createalias"  ++ size control
        {
            alias newAlias;                                           // to save temporarily the new alias
            newAlias.aliasName = parsedCommands[0].p_arguments[1];    // save its name
            newAlias.aliasCommand = parsedCommands[0].p_arguments[2]; // save its command
            aliases.push_back(newAlias);
            continue; // we dont want to exec anything, just to restore the new alias
        }
        // DESTROY ALIAS COMMAND
        else if (parsedCommands[0].p_arguments[0] == "destroyalias" && parsedCommands[0].p_arguments.size() == 2) // check if coomand's first arg is "destroyalias"  ++ size control
        {
            for (int i = 0; i < aliases.size(); i++)
                if (aliases[i].aliasName == parsedCommands[0].p_arguments[1]) // findimg the alias we want to destroy in the aliases array
                {
                    aliases.erase(aliases.begin() + i); // erase it and continue
                    continue;
                }
            continue; // we dont want to exec anything, just to destroy the given alias
        }

        // CD COMMAND
        if (parsedCommands[0].p_arguments[0] == "cd" && parsedCommands[0].p_arguments.size() == 2)
        {
            chdir(parsedCommands[0].p_arguments[1].c_str());
            continue;
        }

        // MY HISTORY COMMAND
        for (int i = 0; i < parsedCommands.size(); i++)
        {
            if (parsedCommands[i].p_arguments.size() > 0)                                                         // just a check
            {                                                                                                     // USER ASKS FOR THE PREVIOUS MAX 20 COMMANDS
                if (parsedCommands[i].p_arguments[0] == "myHistory" && parsedCommands[i].p_arguments.size() == 1) // check if coomand's first and only arg is "myHistory"
                {
                    for (int j = 0; j < histories.size(); j++) // print all the commands saved in the histories array
                    {
                        cout << "history"
                             << " " << histories.size() - j << " " << histories[j] << endl;
                        isHistory = 1; // keep the history flag up to date because we dont want to save the current command "myHistory"
                    }
                }                                                                                                      // USER ASKS FOR A CERTAIN COMMAND WITH ITS ID
                else if (parsedCommands[i].p_arguments[0] == "myHistory" && parsedCommands[i].p_arguments.size() == 2) // check if command's first arg is "myHistory"  and if the command size is 2 because the second arg is the history's id the user wants
                {
                    execHistory = atoi(parsedCommands[i].p_arguments[1].c_str()); // make the second commands arg a char* and then a integer in order to go to the histories array and be able to find it by its index
                    isHistory = 1;                                                // keep the history flag up to date because we dont want to save the current command "myHistory <history's id>"
                }
                else
                { // CHECKING IF THE FIRST ARGUMENT-COMMAND IS ONE OF THE CREATED ALIASES
                    for (int j = 0; j < aliases.size(); j++)
                        if (aliases[j].aliasName == parsedCommands[i].p_arguments[0] && parsedCommands[i].p_arguments.size() == 1) // if a command is asked by using an saved alias name
                        {                                                                                                          // we have to execute it next
                            execAlias = j;                                                                                         // keep up to date the alias flag
                            continue;                                                                                              // we found it, continue
                        }
                }
            }
        }

        if (execAlias > -1) // IF WE ARE ON ALIAS MODE - NO NEED FOR EXEC YET
            continue;
        // MYHISTORY HANDLING
        if (isHistory == 0) // We arent in MYHISTORY MODE - need to save the command
            histories.push_back(command);
        else
            continue; // IF WE ARE ON MYHISTORY MODE - NO NEED FOR EXEC YET

        if (histories.size() == 21) // if we have passed the 20 commands, erase the oldest one
            histories.erase(histories.begin());

        // WILD CHARS
        for (int i = 0; i < parsedCommands.size(); i++)
        {
            for (int j = 0; j < parsedCommands[i].p_arguments.size(); j++)
            {
                string wildArgument = parsedCommands[i].p_arguments[j]; // saves temporarily every arg
                bool isWild = 0;                                        // this flag shows if we matched a file

                if ((wildArgument.find('*') != string::npos) || (wildArgument.find('?') != string::npos)) // if this arg contains at least one of the wild chars
                {
                    parsedCommands[i].p_arguments.erase(parsedCommands[i].p_arguments.begin() + j); // erase it
                    DIR *d;
                    struct dirent *dir;
                    d = opendir("."); // take all the files of this directory
                    if (d)
                    {
                        while ((dir = readdir(d)) != NULL)
                        {
                            string fileName = dir->d_name;                                    // if a file's name matches with the file's name of the wild chars
                            bool to_be_insert_or_not_to_be = isMatch(fileName, wildArgument); // this function will show us

                            if (to_be_insert_or_not_to_be) // so we insert it as an arg in our command to be exec later
                            {
                                isWild = 1;
                                parsedCommands[i].p_arguments.insert(parsedCommands[i].p_arguments.begin() + j, fileName);
                                j++;
                            }
                        }
                        if (!isWild) // if we dont have a match, keep in the argument list the file with the wild chars
                        {
                            parsedCommands[i].p_arguments.insert(parsedCommands[i].p_arguments.begin() + j, wildArgument);
                        }
                        closedir(d);
                    }
                }
            }
        }

        int current_fd = -1; // current file descriptor

        for (int i = 0; i < parsedCommands.size(); i++) // for each command of the given commands in a sigle line
        {
            int fd[2];                    // store fd of the pipe
            if (parsedCommands[i].isPipe) // if we're in the left part of the pipe command ( <here> | ...)
            {
                if (pipe(fd) == -1) // pipe
                {
                    perror(" pipe "); // exit if error
                    exit(1);
                }
            }

            pid_t pid;
            if ((pid = fork()) < 0)      // fork
                perror(" fork failed "); // inform if error

            if (pid == 0) // child
            {
                if (parsedCommands[i].isPipe) // if child is in the left part of the pipe command ( <here> | ...)
                {
                    close(fd[READ]);                // close READ because we dont read from this part
                    dup2(fd[WRITE], STDOUT_FILENO); // duplicate pipes input to standard output
                    close(fd[WRITE]);               // close child's write port because we have duplicated it
                }

                if (i > 0) // if not first argument
                {
                    if (parsedCommands[i - 1].isPipe) // if the previous/left command is pipe
                    {
                        dup2(current_fd, STDIN_FILENO); // read from the pipe
                        close(current_fd);              // close childs read port because we have duplicated it
                    }
                }
                // redirections
                for (int j = 0; j < parsedCommands[i].p_files.size(); j++)
                {
                    int filedes, flags;

                    switch (parsedCommands[i].p_files[j].myType) // which type is our file (1->INPUT, 2->OUTPUT, 3-> CAT)
                    {
                    case 1:
                    case 2:
                        flags = O_CREAT | O_RDWR; // manage file's flags
                        break;
                    case 3:
                        flags = O_CREAT | O_RDWR | O_APPEND;
                    }

                    if ((filedes = open((parsedCommands[i].p_files[j].myString).c_str(), flags, 0644)) == -1) // open file from the command's array of files
                    {
                        perror("Cannot open redirection file"); // if error inform us
                        exit(1);
                    }

                    switch (parsedCommands[i].p_files[j].myType)
                    {
                    case 1:
                        dup2(filedes, STDIN_FILENO); // connect file with standard input
                        close(filedes);
                        break;
                    case 2:
                    case 3:
                        dup2(filedes, STDOUT_FILENO); // connect file with standard output
                        close(filedes);
                        break;
                    }
                    close(filedes);
                }
                // exec instructions
                char *buff[parsedCommands[i].p_arguments.size() + 1];
                for (int j = 0; j < parsedCommands[i].p_arguments.size(); j++)
                {
                    buff[j] = new char[parsedCommands[i].p_arguments[j].length() + 1];
                    strcpy(buff[j], parsedCommands[i].p_arguments[j].c_str());
                }

                buff[parsedCommands[i].p_arguments.size()] = NULL;
                // exec1
                int e = execv(buff[0], buff);

                string firstArgument = "/usr/bin/" + parsedCommands[i].p_arguments[0];

                buff[0] = new char[firstArgument.length() + 1];
                strcpy(buff[0], firstArgument.c_str());
                // exec2
                e = execv(buff[0], buff);

                firstArgument = "/bin/" + parsedCommands[i].p_arguments[0];

                buff[0] = new char[firstArgument.length() + 1];
                strcpy(buff[0], firstArgument.c_str());
                // exec3
                e = execv(buff[0], buff);

                if (e == -1)
                    perror("exec");
            }
            else // parent
            {
                if (i > 0)
                    if (parsedCommands[i - 1].isPipe) // if the previous/left command is pipe
                        close(current_fd);            // close parents read port

                if (parsedCommands[i].isPipe) // if we're in the left part of the pipe command ( <here> | ...) for the parent
                {
                    current_fd = fd[READ]; // store its file descriptor
                    close(fd[WRITE]);      // doesnt did write, it only reads
                }
                else if (!parsedCommands[i].isBack) // if its not background
                {
                    global_pid = pid;
                    waitpid(pid, NULL, 0); // wait for the process to end
                }
            }
        }
    }
    return 0;
}