#pragma once

#include <string>
#include <fstream>
#include <dirent.h>
#include <regex>

class DirHelper {
public:
    typedef std::function<void(const std::string& line, const uint32_t lineno, const std::string& filepath)> LineCallbackFuncType;
    
    typedef std::function<void(const std::string& filepath)> OpenFailCallbackFuncType;
    
    void setOpenFailCallBackFunc(OpenFailCallbackFuncType& callback) {
        m_openfailcallback = callback;
    }
    
    void setLineCallBackFunc(LineCallbackFuncType& callback) {
        m_linecallback = callback;
    }
    
    // Invoke callback with each file path
    void scanDirectory(const std::string& path, const std::regex& fileRegex) {
        DIR *dir;
        struct dirent *ent;
        std::string err;
        
        if ((dir = opendir(path.c_str())) != NULL) {
            // scan all the files and directories within directory
            while ((ent = readdir(dir)) != NULL) {
                std::string filename(ent->d_name);
                
                if (std::regex_match(filename, fileRegex)) {
                    // concatenate the path and open file
                    std::string filepath = path + "/" + filename;
                    std::ifstream inputstream(filepath);
                    std::string line;
                    
                    if(inputstream) {
                        uint32_t lineno = 1;
                        
                        // Scan each line in the input stream
                        while (std::getline(inputstream, line)) {
                            if (m_linecallback)
                                m_linecallback(line, lineno, filepath);
                            lineno++;
                        }
                        
                        inputstream.close();
                    } else if (m_openfailcallback) {
                        m_openfailcallback(filepath);
                    }
                }
            }
            closedir(dir);
        } else if (m_openfailcallback) {
            m_openfailcallback(path);
        }
    }

private:
    LineCallbackFuncType m_linecallback = nullptr;
    OpenFailCallbackFuncType m_openfailcallback = nullptr;
};
