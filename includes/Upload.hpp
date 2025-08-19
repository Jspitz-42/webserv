#ifndef UPLOAD_HPP
# define UPLOAD_HPP

# include <string>

class Upload
{
public:
    static bool handleMultipartUpload(const std::string& content, 
                                    const std::string& boundary, 
                                    const std::string& uploadDir);

private:
    static std::string extractFilename(const std::string& contentDisposition);
    static bool saveFile(const std::string& filepath, const std::string& fileContent);
    static bool createDirectory(const std::string& path);
};

#endif
