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
#include <sstream>
#include <vector>

//need this to convert strings into lowercase
#include <algorithm>

using namespace std;

//a function for each match method
bool matchWhole(string, string);
bool matchPrefix(string, string);
bool matchSuffix(string, string);
bool matchAnywhere(string, string);
bool matchEmbedded(string, string);
size_t matchWildcards(string, string, char);

//wildcard character. May change scope or implementation later.
char wildChar = '_';

int main(int argc, char* argv[]) {

    enum MODE {
        WHOLE, PREFIX, SUFFIX, ANYWHERE, EMBEDDED
    } mode = WHOLE;
    
    //flags that may change execution path.
    bool reverse_match = false;
    bool case_sensitive = true;
    bool length_constrained = false;
    
    //variables to store arguments of the -n option.
    int strMin = 0, strMax = 0;

    int c;
    while ((c = getopt(argc, argv, ":wpsaevin:")) != -1) {
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
            case 'n': // constrain length of matched words
                
                length_constrained = true;
                
                stringstream ss;
                string strArr[2];
                
                //counter for how many arguments are found
                int counter = 0;
                
                //use stringstream to convert chars to int
                ss << optarg;
                
                while(getline(ss,strArr[counter],',')){
                    counter++;
                }
                
                //stream needs to be cleared!
                ss.clear();
                
                //logic to extract arguments and put them in strMin and strMax
                //when it makes sense.
                if (counter > 0){
                    if(strArr[0] != ""){
                        ss << strArr[0];
                        ss >> strMin;
                    }
                }
                ss.clear();
                
                if (counter >= 1){
                    if(strArr[1] != ""){
                        ss << strArr[1];
                        ss >> strMax;
                    }
                }
                

                break;
        }
    }
    
    argc -= optind;
    argv += optind;

    string pattern = argv[0];

    //convert pattern to lowercase if -i was an option
    if (!case_sensitive) {
        transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
    }

    //oldWord stores original version of word
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
                if (matchWhole(pattern, word)) found = true;
                break;
            case PREFIX:
                if (matchPrefix(pattern, word)) found = true;
                break;
            case ANYWHERE:
                if (matchAnywhere(pattern, word)) found = true;
                break;
            case SUFFIX:
                if (matchSuffix(pattern, word)) found = true;
                break;
            case EMBEDDED:
                if (matchEmbedded(pattern, word)) found = true;
                break;
        }

        //deal with -r here
        if (reverse_match) {
            found = !found;
        }

        //deal with -n here
        if ( (word.size() < strMin && strMin != 0) 
                || (word.size() > strMax && strMax != 0) ){
            found = false;
        }
        
        if (found) {
            cout << oldWord << endl;
            matches++;
        }

        found = false;

    }
    return (matches == 0) ? 1 : 0;
}

/**
 * Matches pattern to the whole word.
 * 
 * @param pattern
 * @param word
 * 
 * @return true if pattern matches the whole word. false if no match found.
 */
bool matchWhole(string pattern, string word) {
    if (matchWildcards(pattern, word, wildChar) != string::npos
            && word.size() == pattern.size()) {
        return true;
    } else return false;
}

/**
 * Matches patter to the start of the word
 * 
 * @param pattern
 * @param word
 * 
 * @return true if pattern matches to the start of word. false otherwise.
 */
bool matchPrefix(string pattern, string word) {
    if (matchWildcards(pattern, word, wildChar) == 0) {
        return true;
    } else return false;
}

/**
 * Matches pattern to anywhere in word.
 * 
 * @param pattern
 * @param word
 * 
 * @return true if pattern can be found anywhere in word. false otherwise.
 */
bool matchAnywhere(string pattern, string word) {
    if (matchWildcards(pattern, word, wildChar) != string::npos) {
        return true;
    } else return false;
}

/**
 * Matches pattern to the end of word.
 * 
 * @param pattern
 * @param word
 * 
 * @return true if pattern constitutes the end of the word. false otherwise.
 */
bool matchSuffix(string pattern, string word) {

    int biggestIndex = word.size() - pattern.size();
    if (matchWildcards(pattern, word, wildChar) == biggestIndex && biggestIndex >= 0) {
        return true;
    } else return false;
}

/**
 * Matches word to anywhere inside of pattern.
 * 
 * @param pattern
 * @param word
 * 
 * @return true if word can be found anywhere inside of pattern. false otherwise.
 */
bool matchEmbedded(string pattern, string word) {
    if (matchWildcards(word, pattern, wildChar) != string::npos) {
        return true;
    } else return false;
}

/**
 * Find matches when wildcards are used
 * 
 * @param pattern
 * @param word
 *
 * @return starting index where match begins if match is found or string::npos
 * if no match is found.
 */
size_t matchWildcards(string pattern, string word, char wildcard) {
    bool found = true;

    //if pattern is bigger than word, then pattern is not inside word
    if (pattern.size() > word.size()) return string::npos;

    //go through each 'sensible' letter of word and search for a match
    //'sensible' because we don't want to search the last index if pattern is 
    //size 4, for example.
    for (int i = 0; i <= word.size() - pattern.size(); i++) {

        //go through each letter in pattern and check with corresponding letter
        //in word and set found to false if mismatch is found
        for (int j = 0; j < pattern.size(); j++) {
            if (!(word.at(i + j) == pattern.at(j) || pattern.at(j) == wildcard
                    || word.at(i + j) == wildcard)) {
                found = false;
            }
        }

        //return index where match is found
        if (found) {
            return i;
        }

        //need to reset found!
        found = true;
    }

    //return not found if no match is found for each 'sensible' letter in word.
    return string::npos;
}