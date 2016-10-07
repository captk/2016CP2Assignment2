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

//need this to convert strings into lowercase
#include <algorithm>

using namespace std;
bool matchWhole(string, string);
bool matchPrefix(string, string);
bool matchSuffix(string, string);
bool matchAnywhere(string, string);
bool matchEmbedded(string, string);
size_t matchWildcards(string, string, char);
char wildChar = '_';

int main(int argc, char* argv[]) {

    enum MODE {
        WHOLE, PREFIX, SUFFIX, ANYWHERE, EMBEDDED
    } mode = WHOLE;
    bool reverse_match = false;
    bool case_sensitive = true;

    int c;
    while ((c = getopt(argc, argv, ":wpsaevi")) != -1) {
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
            case 'i': // disregard case of letters
                case_sensitive = false;
                break;
        }
    }
    argc -= optind;
    argv += optind;

    string pattern = argv[0];

    //convert pattern to lowercase if -i is used
    if (!case_sensitive) {
        transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
    }

    string word, oldWord;
    int matches = 0;
    bool found = false;

    //loops for each word
    while (cin >> word) {
        
        oldWord = word;
        //convert word to lowercase if -i is used
        if (!case_sensitive) {
            transform(word.begin(), word.end(), word.begin(), ::tolower);
        }

        //divide and conquer - each mode has its own function.
        switch (mode) {
            case WHOLE:
                if (matchWhole(pattern, word)) {
                    found = true;
                    matches++;
                }
                break;
            case PREFIX:
                if (matchPrefix(pattern, word)) {
                    found = true;
                    matches++;
                }
                break;
            case ANYWHERE:
                if (matchAnywhere(pattern, word)) {
                    found = true;
                    matches++;
                }
                break;
            case SUFFIX:
                if (matchSuffix(pattern, word)) {
                    found = true;
                    matches++;
                }
                break;
            case EMBEDDED:
                if (matchEmbedded(pattern, word)) {
                    found = true;
                    matches++;
                }
                break;
        }

        if (reverse_match) {
            //cout << "reverse matching" << endl;
            if (found) matches--;
            else matches++;
            found = !found;
        }

        if (found) {
            cout << oldWord << endl;
        }

        found = false;

    }
    //cout << matches << endl;
    return (matches == 0) ? 1 : 0;
}

/**
 * Prints  words that exactly match pattern
 * 
 * @param pattern
 * @param word
 * 
 * @return Nothing
 */
bool matchWhole(string pattern, string word) {
    if (matchWildcards(pattern, word, wildChar) != string::npos
            && word.size() == pattern.size()) {
        return true;
    } else return false;
}

/**
 * 
 * @param pattern
 * @param word
 */
bool matchPrefix(string pattern, string word) {
    if (matchWildcards(pattern, word, wildChar) == 0) {
        return true;
    } else return false;
}

/**
 * 
 * @param pattern
 * @param word
 */
bool matchAnywhere(string pattern, string word) {
    if (matchWildcards(pattern, word, wildChar) != string::npos) {
        return true;
    } else return false;
}

/**
 * 
 * @param pattern
 * @param word
 */
bool matchSuffix(string pattern, string word) {

    int biggestIndex = word.size() - pattern.size();
    if (matchWildcards(pattern, word, wildChar) == biggestIndex && biggestIndex >= 0) {
        return true;
    } else return false;
}

/**
 * 
 * @param pattern
 * @param word
 */
bool matchEmbedded(string pattern, string word) {
    if (matchWildcards(word, pattern, wildChar) != string::npos) {
        return true;
    } else return false;
}

/**
 * Find matches when wildcards are used
 * @param pattern
 * @param word
 * @return starting index where match begins if match is found or string::npos
 * if no match is found.
 */
size_t matchWildcards(string pattern, string word, char wildcard){
    bool found = true;
    
    //if pattern is bigger than word, then pattern is not inside word
    if (pattern.size() > word.size()) return string::npos;
    
    //go through each 'sensible' letter of word and search for a match
    //'sensible' because we don't want to search the last index if patter is 
    //size 4, for example.
    for (int i = 0; i <= word.size() - pattern.size(); i++){
        //cout << "word: " << word.size() << " pattern: " << pattern.size() << endl;
        //go through each letter in pattern and check with corresponding letter
        //in word and set found to false if mismatch is found
        for (int j = 0; j < pattern.size(); j++){
            if(!(word.at(i+j) == pattern.at(j) || pattern.at(j) == wildcard
                    || word.at(i+j) == wildcard)){
                found = false;
            }
        }
        
        //return index where match is found
        if (found){
            return i;
        }
        
        //need to reset found!
        found = true;
    }
    //cout << "cannot find " << pattern << " in " << word << endl;
    return string::npos;
}