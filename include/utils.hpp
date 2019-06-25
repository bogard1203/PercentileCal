#pragma once

#include <stdlib.h>
#include <string>
#include <sstream>

namespace utils {
    // Below implementation is from https://stackoverflow.com/questions/2844817/how-do-i-check-if-a-c-string-is-an-int
    // The reasoning:
    // strtol will parse the string, stopping at the first character that cannot be considered part of an integer.
    // If you provide p (as I did above), it sets p right at this first non-integer character.
    // If p is not set to the end of the string (the 0 character), then there is a non-integer character in the string s, meaning s is not a correct integer.
    bool isInteger(const std::string& s, long& value) {
        // Check edge cases
        if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

        char * p;
        value = strtol(s.c_str(), &p, 10);
        
        return (*p == 0);
    }

    bool isFloat(const std::string& s, float& value) {
        // Check edge cases
        if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

        char * p;
        value = strtof(s.c_str(), &p);
        
        return (*p == 0);
    }

    bool isFloatAndBetween(const std::string& s, const float min, const float max, float& value) {
        if (isFloat(s, value)) {
            if (value > min && value < max)
                return true;
        }
        return false;
    }
    
    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    std::string getRegexString(std::regex_error& e) {
        std::string error_string = "Unknown.";
        switch(e.code()) {
            case std::regex_constants::error_collate:
                error_string = "The expression contained an invalid collating element name.";
                break;
            case std::regex_constants::error_ctype:
                error_string = "The expression contained an invalid character class name.";
                break;
            case std::regex_constants::error_escape:
                error_string = "The expression contained an invalid escaped character, or a trailing escape.";
                break;
            case std::regex_constants::error_backref:
                error_string = "The expression contained an invalid back reference.";
                break;
            case std::regex_constants::error_brack:
                error_string = "The expression contained mismatched brackets ([ and ]).";
                break;
            case std::regex_constants::error_paren:
                error_string = "The expression contained mismatched parentheses (( and )).";
                break;
            case std::regex_constants::error_brace:
                error_string = "The expression contained mismatched braces ({ and }).";
                break;
            case std::regex_constants::error_badbrace:
                error_string = "The expression contained an invalid range between braces ({ and }).";
                break;
            case std::regex_constants::error_range:
                error_string = "The expression contained an invalid character range.";
                break;
            case std::regex_constants::error_space:
                error_string = "There was insufficient memory to convert the expression into a finite state machine.";
                break;
            case std::regex_constants::error_badrepeat:
                error_string = "The expression contained a repeat specifier (one of *?+{) that was not preceded by a valid regular expression.";
                break;
            case std::regex_constants::error_complexity:
                error_string = "The complexity of an attempted match against a regular expression exceeded a pre-set level.";
                break;
            case std::regex_constants::error_stack:
                error_string = "There was insufficient memory.";
                break;
            default:
                break;
        }
        return error_string;
    }
}
