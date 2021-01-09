#ifndef DEFINES_HPP
#define DEFINES_HPP

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    #define PATH_DELIMITER string("\\")
    #define LINE_DELIMITER string("\r\n")
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    #define PATH_DELIMITER string("/")
    #define LINE_DELIMITER string("\n")
#endif

#define FILE_TYPE_DELIMITER string(".")

#endif // DEFINES_HPP
