// in this file you can find the funtions i use exept the main function and the two signal handling functions

#include "mysh.h"

void saveCommand(bool is_pipe, vector<string> *parsedArguments, vector<myFiles> *parsedFiles, myCommands *parsedCommand, vector<myCommands> *parsedCommands)
{
    parsedCommand->isPipe = is_pipe;
    parsedCommand->isBack = 0;
    if (parsedArguments->back() == "&") // if last element of the vector parsedArguments is '&'
    {
        parsedCommand->isBack = 1;
        parsedArguments->pop_back();
    }
    parsedCommand->p_arguments = *parsedArguments;
    parsedCommand->p_files = *parsedFiles;
    parsedCommands->push_back(*parsedCommand);
    parsedCommand->p_arguments.clear();
    parsedCommand->p_files.clear();
    parsedArguments->clear();
    parsedFiles->clear();
}
bool isMatch(string s, string p)
{ // it finds whether string s matches string p or not ( string p contains wild chars)
    // dry run this sample case on paper , if unable to understand what soln does
    //  p = "a*bc" s = "abcbc"
    int sIdx = 0, pIdx = 0, lastWildcardIdx = -1, sBacktrackIdx = -1, nextToWildcardIdx = -1;
    while (sIdx < s.size())
    {
        if (pIdx < p.size() && (p[pIdx] == '?' || p[pIdx] == s[sIdx]))
        {
            // chars match
            ++sIdx;
            ++pIdx;
        }
        else if (pIdx < p.size() && p[pIdx] == '*')
        {
            // wildcard, so chars match - store index.
            lastWildcardIdx = pIdx;
            nextToWildcardIdx = ++pIdx;
            sBacktrackIdx = sIdx;

            // storing the pidx+1 as from there I want to match the remaining pattern
        }
        else if (lastWildcardIdx == -1)
        {
            // no match, and no wildcard has been found.
            return false;
        }
        else
        {
            // backtrack - no match, but a previous wildcard was found.
            pIdx = nextToWildcardIdx;
            sIdx = ++sBacktrackIdx;
            // backtrack string from previousbacktrackidx + 1 index to see if then new pidx and sidx have same chars, if that is the case that means wildcard can absorb the chars in b/w and still further we can run the algo, if at later stage it fails we can backtrack
        }
    }
    for (int i = pIdx; i < p.size(); i++)
    {
        if (p[i] != '*')
            return false;
    }
    return true;
    // true if every remaining char in p is wildcard
}