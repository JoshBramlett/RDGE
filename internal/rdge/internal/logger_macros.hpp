//! \headerfile <rdge/internal/logger_macros.hpp>
//! \author Josh Bramlett
//! \version 0.0.3
//! \date 05/13/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/application.hpp>
#include <rdge/util/logger.hpp>

#ifndef DLOG
    #ifdef RDGE_DEBUG
        #define DLOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Debug, msg, __FILE_NAME__, __LINE__); \
            RDGE::WriteToConsole(RDGE::LogLevel::Debug, msg, __FILE_NAME__, __LINE__); \
        } while (false)
    #else
        #define DLOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Debug, msg); \
        } while (false)
    #endif
#endif

#ifndef ILOG
    #ifdef RDGE_DEBUG
        #define ILOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Info, msg, __FILE_NAME__, __LINE__); \
            RDGE::WriteToConsole(RDGE::LogLevel::Info, msg, __FILE_NAME__, __LINE__); \
        } while (false)
    #else
        #define ILOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Info, msg); \
        } while (false)
    #endif
#endif

#ifndef WLOG
    #ifdef RDGE_DEBUG
        #define WLOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Warning, msg, __FILE_NAME__, __LINE__); \
            RDGE::WriteToConsole(RDGE::LogLevel::Warning, msg, __FILE_NAME__, __LINE__); \
        } while (false)
    #else
        #define WLOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Warning, msg); \
        } while (false)
    #endif
#endif

#ifndef ELOG
    #ifdef RDGE_DEBUG
        #define ELOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Error, msg, __FILE_NAME__, __LINE__); \
            RDGE::WriteToConsole(RDGE::LogLevel::Error, msg, __FILE_NAME__, __LINE__); \
        } while (false)
    #else
        #define ELOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Error, msg); \
        } while (false)
    #endif
#endif

#ifndef FLOG
    #ifdef RDGE_DEBUG
        #define FLOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Fatal, msg, __FILE_NAME__, __LINE__); \
            RDGE::WriteToConsole(RDGE::LogLevel::Fatal, msg, __FILE_NAME__, __LINE__); \
        } while (false)
    #else
        #define FLOG(msg) do { \
            RDGE::WriteToLogFile(RDGE::LogLevel::Fatal, msg); \
        } while (false)
    #endif
#endif
