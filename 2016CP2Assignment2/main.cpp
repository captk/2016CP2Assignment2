/* 
 * Match: find word matches
 * Assignment 2, Computer Programming 2
 * Created: ...
 * Author: ...
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;
bool matchWhole(string, string);
bool matchPrefix(string, string);
bool matchSuffix(string, string);
bool matchAnywhere(string, string);
bool matchEmbedded(string, string);

int main(int argc, char* argv[]) {

    enum MODE {
        WHOLE, PREFIX, SUFFIX, ANYWHERE, EMBEDDED
    } mode = WHOLE;
    bool reverse_match = false;

    int c;
    while ((c = getopt(argc, argv, ":wpsaev")) != -1) {
        switch (c) {
            case 'w': // pattern matches whole word
                mode = WHOLE;
                break;
            case 'p': // pattern matches prefix
                mode = PREFIX;
                break;
            case 'a': // pattern matches anywhere
                mode = ANYWHERE;
                break;
            case 's': // pattern matches suffix
                mode = SUFFIX;
                break;
            case 'e': // pattern matches anywhere
                mode = EMBEDDED;
                break;
            case 'v': // reverse sense of match
                reverse_match = true;
                break;
        }
    }
    argc -= optind;
    argv += optind;

    string pattern = argv[0];

    string word;
    int matches = 0;

    //loops for each word
    while (cin >> word) {

        //divide and conquer - each mode has its own function.
        switch (mode) {
            case WHOLE:
                matchWhole(pattern, word);
                break;
            case PREFIX:
                matchPrefix(pattern, word);
                break;
            case ANYWHERE:
                matchAnywhere(pattern, word);
                break;
            case SUFFIX:
                matchSuffix(pattern, word);
                break;
            case EMBEDDED:
                matchEmbedded(pattern, word);
                break;
        }

    }
    return (matches == 0) ? 1 : 0;
}

/**
 * Prints with cout words that exactly match pattern
 * 
 * @param pattern
 * @param word
 * 
 * @return Notthing
 */
bool matchWhole(string pattern, string word) {
    if (pattern == word) {
        cout << word << endl;
        return true;
    }
    else return false;
}

/**
 * 
 * @param pattern
 * @param word
 */
bool matchPrefix(string pattern, string word) {

}

/**
 * 
 * @param pattern
 * @param word
 */
bool matchAnywhere(string pattern, string word) {

}

/**
 * 
 * @param pattern
 * @param word
 */
bool matchSuffix(string pattern, string word) {

}

/**
 * 
 * @param pattern
 * @param word
 */
bool matchEmbedded(string pattern, string word) {

}