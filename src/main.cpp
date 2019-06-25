#include <stdio.h>
#include <vector>
#include <string>
#include <regex>
#include <iostream>

#include <utils.hpp>
#include <dirhelper.hpp>
#include <percentile.hpp>

// Refer to https://github.com/sengelha/streaming-percentiles
#include <stmpct/gk.hpp>

#define FILE_REGEX_KEYWORD  "FILE_REGEX"
#define LINE_REGEX_KEYWORD  "LINE_REGEX"
#define GROUPID_KEYWORD     "GROUPID"
#define PERCENTILE_KEYWORD  "PERCENTILE"

typedef struct expinfo_t {
    std::regex fileRegex;
    std::regex lineRegex;
    uint32_t groupID;
    std::vector<float> percentiles;
} ConfigInfo;

// Process configFilePath file to extract data for ConfigInfo
bool processConfig(const std::string configFilePath, ConfigInfo& config) {
    bool ret = true;
    std::string fileRegex = "";
    std::string lineRegex = "";
    std::string line;
    
    // Open config file
    std::ifstream inputstream(configFilePath);

    if (inputstream) {
        // Scan each line in the input stream
        while (std::getline(inputstream, line)) {
            // Skip comment and empty line
            if (line.size() > 0 && line[0] != '#') {
                size_t pos = line.find('=');
                
                // Handle edge cases such as "X" or "X "
                if (pos != std::string::npos && pos != (line.size() - 1)) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);

                    if (FILE_REGEX_KEYWORD == key) {
                        fileRegex = value;
                    } else if (LINE_REGEX_KEYWORD == key) {
                        lineRegex = value;
                    } else if (GROUPID_KEYWORD == key) {
                        long v = 0;
                        if (utils::isInteger(value, v)) {
                            config.groupID = v;
                        }
                    } else if (PERCENTILE_KEYWORD == key) {
                        float percentile = 0;
                        
                        // split value string to tokens by a delimiter of space
                        auto tokens = utils::split(value, ' ');
                        for (auto& token: tokens) {
                            if (utils::isFloatAndBetween(token, 0, 1, percentile))
                                config.percentiles.push_back(percentile);
                        }
                    }
                }
            }
        }
        
        inputstream.close();
    } else {
        std::cerr << "Could not open config file : "<< configFilePath << std::endl;
        ret = false;
    }
    
    if (ret) {
        // Check the availability of config values
        if (fileRegex.size() == 0) {
            std::cerr << "No " << FILE_REGEX_KEYWORD << " in config:" << configFilePath << std::endl;
            ret = false;
        } else if (lineRegex.size() == 0) {
            std::cerr << "No " << LINE_REGEX_KEYWORD << " in config:" << configFilePath << std::endl;
            ret = false;
        } else if (config.groupID == 0) {
            std::cerr << "No " << GROUPID_KEYWORD << " in config:" << configFilePath << std::endl;
            ret = false;
        } else if (config.percentiles.size() == 0) {
            std::cerr << "No " << PERCENTILE_KEYWORD << " in config:" << configFilePath << std::endl;
            ret = false;
        } else {
            // Check the correctness of config values
            try {
                config.fileRegex = std::regex(fileRegex, std::regex::ECMAScript);
            } catch (std::regex_error& e) {
                std::string errstring = utils::getRegexString(e);
                std::cerr << "File regex: " <<  fileRegex << std::endl << errstring << std::endl;
                ret = false;
            }
            
            try {
                config.lineRegex = std::regex(lineRegex, std::regex::ECMAScript);
            } catch (std::regex_error& e) {
                std::string errstring = utils::getRegexString(e);
                std::cerr << "Line regex: " <<  lineRegex << std::endl << errstring << std::endl;
                ret = false;
            }
        }
    }
    return ret;
}

int main (int argc, char *argv[]) {
    int ret = 0;
    float epsilon = 0;
    bool is_stmpct = false;
    ConfigInfo config;
    std::string config_path;
    std::string path;
    
    // arguments check
    if (argc != 3 && argc != 5) {
        std::cout << "Usage: " << argv[0] << " [-e epsilon] config path" << std::endl;
        std::cout << "Where: epsilon is between 0 and 1. If given, app uses stmpct algorithm" << std::endl;
        std::cout << "       config is the config file for setting file and line patterns" << std::endl;
        std::cout << "       path is the directory to search target files" << std::endl;
        std::cout << "Example:" << argv[0] << " test/config_1.txt test/unit_1" << std::endl;
        return 1;
    }
    
    if (argc == 5) {
        if (!strncmp(argv[1], "-e", 2) && !utils::isFloatAndBetween(argv[2], 0.f, 1.0f, epsilon)) {
            std::cout << "Expected epsilon should be between 0 and 1" << std::endl;
            return 1;
        }
        
        is_stmpct = true;
        config_path = argv[3];
        path = argv[4];
    } else {
        config_path = argv[1];
        path = argv[2];
    }
    
    if (!processConfig(config_path, config))
        ret = 1;

    if (ret == 0) {
        // TODO: Abstract the inteface from implemetations.
        // stmpct is third-party source.
        stmpct::gk<float> stmpct_imp(epsilon);
        percentile::Percentile<float> custom_imp;
        DirHelper dir_handler;
        
        // callback for file open failure
        DirHelper::OpenFailCallbackFuncType openfail_callback = [&] (const std::string& path) -> void {
            std::cerr << "Open Failed: " << path << std::endl;
        };
        
        // callback for line process
        DirHelper::LineCallbackFuncType line_callback = [&] (const std::string& line, const uint32_t lineno, const std::string& filepath) -> void {
            std::smatch result;
            
            if (std::regex_match(line, result, config.lineRegex)) {
                if (result.ready()) {
                    float value = 0;
                    
                    // The first sub_match element corresponds to the entire match, and the rest depends on
                    // the regex expression. http://www.cplusplus.com/reference/regex/match_results/
                    if (result.size() > 1 && result.size() >= config.groupID) {
                        if (utils::isFloat(result[config.groupID].str(), value)) {
                            if (is_stmpct)
                                stmpct_imp.insert(value);
                            else
                                custom_imp.insert(value);
                        } else
                            std::cerr << "line(" << lineno << ") Non-Number: " << line << std::endl;
                    } else
                        std::cerr << "line(" << lineno << ") No Groups: " << line << std::endl;
                } else {
                    std::cerr << "Match result is not ready. Unexpected result." << std::endl;
                    assert(false);
                }
            } else
               std::cerr << "line(" << lineno << ") Unmatched: " << line << std::endl;
        };
        
        // Set callbacks and start scanning files
        dir_handler.setOpenFailCallBackFunc(openfail_callback);
        dir_handler.setLineCallBackFunc(line_callback);
        dir_handler.scanDirectory(path, config.fileRegex);
        
        if (is_stmpct) {
            if (stmpct_imp.hasData()) {
                uint32_t perc = 0;
                float value = 0;

                for (auto& percentile: config.percentiles) {
                    perc = round(percentile * 100.0);
                    value = stmpct_imp.quantile(percentile);
                    std::cout << "Percentile " << perc << "% is " << value << std::endl;
                }
            }
        } else {
            if (custom_imp.hasData()) {
                uint32_t perc = 0;
                float value = 0;
                
                for (auto& percentile: config.percentiles) {
                    perc = round(percentile * 100.0);
                    value = custom_imp.quantile(percentile);
                    std::cout << "Percentile " << perc << "% is " << value << std::endl;
                }
            }
        }
    }
    return ret;
}
