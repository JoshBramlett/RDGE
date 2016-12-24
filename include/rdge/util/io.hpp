//! \headerfile <rdge/util/io.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 03/22/2016

#pragma once

#include <rdge/core.hpp>

#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace util {

//! \brief Read and return contents of a text file
//! \param [in] filepath Full path of the file to read
//! \returns Contents of the text file
//! \throws File cannot be found or read
inline std::string read_text_file (const char* filepath)
{
    // TODO: Make smart, should check std::string::max_size to make
    //       sure that the file is not too big.  Also, look into
    //       how to handle adverse behavior from our C functions
    // TODO: Look into fopen_s and the other extended functions
    // TODO: Actually throw exceptions
    // TODO: The "rt" below may not be needed, the docs say it's
    //       treated as a text file by default and may be library
    //       implementation specific.
    // http://www.cplusplus.com/reference/cstdio/fopen/
    FILE* file = fopen(filepath, "rt");
    if (!file)
    {
        return "";
    }

    fseek(file, 0, SEEK_END);
    unsigned long length = ftell(file);
    char* data = new char[length + 1];
    memset(data, 0, length + 1);

    fseek(file, 0, SEEK_SET);
    fread(data, 1, length, file);
    fclose(file);

    std::string result { data };
    delete[] data;

    return result;
}

} // namespace util
} // namespace rdge
