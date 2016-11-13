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
#include <stdio.h>
#include <fstream>

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
bool jumbled_match = false;

int main(int argc, char* argv[]) {

    enum MODE {
        WHOLE, PREFIX, SUFFIX, ANYWHERE, EMBEDDED
    } mode = WHOLE;

    //flags that may change execution path.
    bool reverse_match = false;
    bool case_sensitive = true;
    bool length_constrained = false;
    bool reading_file = false;


    //variables to store arguments of the -n option.
    int strMin = 0, strMax = 0;

    int c;
    while ((c = getopt(argc, argv, ":wpsaevijn:")) != -1) {
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
            case 'j': // matches words with out of order letters
                jumbled_match = true;
            case 'n': // constrain length of matched words
            {
                length_constrained = true;

                stringstream ss;
                string strArr[2];

                //counter for how many arguments are found
                int counter = 0;

                //use stringstream to convert chars to int
                ss << optarg;

                while (getline(ss, strArr[counter], ',')) {
                    counter++;
                }

                //stream needs to be cleared!
                ss.clear();

                //logic to extract arguments and put them in strMin and strMax
                //when it makes sense.
                if (counter > 0) {
                    if (strArr[0] != "") {
                        ss << strArr[0];
                        ss >> strMin;
                    }
                }
                ss.clear();

                if (counter >= 1) {
                    if (strArr[1] != "") {
                        ss << strArr[1];
                        ss >> strMax;
                    }
                }
                break;
            }
            case ':'://when an option doesn't get a required operand
                fprintf(stderr, "Option -n requires an operand\n");
                fprintf(stderr, "Usage: match [-OPTION]... PATTERN [FILENAME]...\n");
                return 2;
            case '?'://when an option given does not exist
                fprintf(stderr, "Unrecognised option -%c\n", optopt);
                fprintf(stderr, "Usage: match [-OPTION]... PATTERN [FILENAME]...\n");
                return 2;
        }
    }

    argc -= optind;
    argv += optind;

    //abort when no pattern is given
    if (argc == 0) {
        fprintf(stderr, "Missing pattern\n");
        fprintf(stderr, "Usage: match [-OPTION]... PATTERN [FILENAME]...\n");
        return 2;
    }
    string pattern = argv[0];

    //convert pattern to lowercase if -i was an option
    if (!case_sensitive) {
        transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
    }

    //oldWord stores original version of word
    string word, oldWord;
    int matches = 0;
    bool found = false;
    bool wordsLeft = true;

    //if filename is given
    ifstream ifs;
    int numFiles = argc - 1;
    int currentFileNum = 1;
    if (argc > 1) {
        ifs.open(argv[currentFileNum]);
        if (ifs.fail()) {
            fprintf(stderr, "Can't open file %s\n", argv[1]);
            return 2;
        }
        currentFileNum++;
        reading_file = true;
    }

    //get the first word
    if (reading_file) {
        wordsLeft = (ifs >> word);
    } else wordsLeft = (cin >> word);

    //loops for each word
    while (wordsLeft) {

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
        if ((word.size() < strMin && strMin != 0)
                || (word.size() > strMax && strMax != 0)) {
            found = false;
        }

        if (found) {
            cout << oldWord << endl;
            matches++;
        }

        found = false;

        if (reading_file) {
            wordsLeft = (ifs >> word);
        } else wordsLeft = (cin >> word);

        //Move on to the next file when done
        if (reading_file && !wordsLeft) {
            ifs.close();
            if (currentFileNum <= numFiles) {
                ifs.open(argv[currentFileNum]);
                if (ifs.fail()) {
                    fprintf(stderr, "Can't open file %s\n", argv[1]);
                    return 2;
                }
                currentFileNum++;
                wordsLeft = (ifs >> word);
            }
        }
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
 * Matches word to anywhere inside of pattern. Specification is vague on this.
 * A hack was created to match the output for level 10.
 * 
 * @param pattern
 * @param word
 * 
 * @return true if word can be found anywhere inside of pattern. false otherwise.
 */
bool matchEmbedded(string pattern, string word) {
    //here is the hack. It performs destructive matching where pattern and word
    //will be reduced. If word is reduced to 0 characters, or the same number
    //as wildcard characters then there is a match
    if (jumbled_match) {
        int wildcards = 0;

        //first, collect all underscores
        for (int n = 0; n < pattern.size(); n++) {
            if (pattern.at(n) == wildChar) {
                pattern.erase(n, 1);
                n--;
                wildcards++;
            }
        }
        for (int i = 0; i < pattern.size(); i++) {

            for (int j = 0; j < word.size(); j++) {
                if (pattern.at(i) == word.at(j)) {
                    word.erase(j,1);
                    j = word.size();
                }
            }
        }
        
        if (word.size() == wildcards) return true;
        else return false;
    }
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
    int wildcards = 0;

    //if pattern is bigger than word, then pattern is not inside word
    if (pattern.size() > word.size()) return string::npos;

    //if jumbled_match flag is true, matching is different
    if (jumbled_match) {

        //first, collect all underscores
        for (int n = 0; n < pattern.size(); n++) {
            if (pattern.at(n) == wildcard) {
                pattern.erase(n, 1);
                n--;
                wildcards++;
            }
        }
    }

    // go through each 'sensible' letter of word and search for a match
    //'sensible' because we don't want to search the last index if pattern is 
    //size 4, for example.
    for (int i = 0; i <= word.size() - pattern.size() - wildcards; i++) {

        //if jumbled_match flag is set go through each letter in pattern and see
        //if it exists in the substring of word
        //this is a destructive match that will erase all found letters
        if (jumbled_match) {

            string tempPattern = pattern;
            string tempWord = word;

            for (int j = 0; j < pattern.size(); j++) {
                for (int k = 0; k < pattern.size() + wildcards; k++) {
                    if (pattern.at(j) == word.at(i + k)) {
                        word.erase(i + k, 1);
                        pattern.erase(j, 1);
                        j--;
                        k = pattern.size() + wildcards;
                    }
                }
            }
            if (pattern.size() > 0) found = false;
            pattern = tempPattern;
            word = tempWord;

        }//go through each letter in pattern and check with corresponding letter
            //in word and set found to false if mismatch is found
        else {
            for (int j = 0; j < pattern.size(); j++) {
                if (!(word.at(i + j) == pattern.at(j) || pattern.at(j) == wildcard
                        || word.at(i + j) == wildcard)) {
                    found = false;
                }
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