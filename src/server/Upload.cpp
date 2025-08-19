#include "Upload.hpp"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

bool Upload::handleMultipartUpload(const std::string& content, 
                                 const std::string& boundary, 
                                 const std::string& uploadDir)
{
    
    if (content.empty() || boundary.empty() || uploadDir.empty()) {
        return false;
    }

    if (!createDirectory(uploadDir)) {
        return false;
    }

    std::string delimiter = "--" + boundary;
    
    size_t start = content.find(delimiter);
    if (start == std::string::npos) {
        return false;
    }
    
    start += delimiter.length();
    if (start < content.length() && content[start] == '\r') start++;
    if (start < content.length() && content[start] == '\n') start++;

    size_t end = content.find(delimiter, start);
    if (end == std::string::npos) {
        std::string endDelimiter = delimiter + "--";
        end = content.find(endDelimiter, start);
        if (end == std::string::npos) {
            end = content.length();
        }
    }

    
    std::string part = content.substr(start, end - start);
    
    size_t headerEnd = part.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        headerEnd = part.find("\n\n");
        if (headerEnd == std::string::npos) {
            return false;
        }
        headerEnd += 2;
    } else {
        headerEnd += 4;
    }

    std::string headers = part.substr(0, headerEnd - 2);
    std::string fileData = part.substr(headerEnd);
    
    
    while (!fileData.empty() && 
           (fileData[fileData.length() - 1] == '\r' || fileData[fileData.length() - 1] == '\n')) {
        fileData.erase(fileData.length() - 1);
    }

    std::string filename = extractFilename(headers);

    
    if (!filename.empty() && !fileData.empty()) {
        std::string filepath = uploadDir + "/" + filename;
        bool result = saveFile(filepath, fileData);
        return result;
    }
    
    return false;
}

std::string Upload::extractFilename(const std::string& contentDisposition)
{
    size_t filenamePos = contentDisposition.find("filename=\"");
    if (filenamePos == std::string::npos)
        return "";
    
    filenamePos += 10; 
    size_t endPos = contentDisposition.find("\"", filenamePos);
    if (endPos == std::string::npos)
        return "";
    
    return contentDisposition.substr(filenamePos, endPos - filenamePos);
}

bool Upload::saveFile(const std::string& filepath, const std::string& fileContent)
{
    std::ofstream file(filepath.c_str(), std::ios::binary);
    if (!file.is_open())
        return false;
    
    file.write(fileContent.c_str(), fileContent.size());
    file.close();
    
    return file.good();
}

bool Upload::createDirectory(const std::string& path)
{
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    
    return mkdir(path.c_str(), 0755) == 0;
}