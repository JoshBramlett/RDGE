//! \headerfile <rdge/internal/logger_macros.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/16/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/util/logger.hpp>

#ifndef DLOG
    #ifdef RDGE_DEBUG
        #define DLOG(msg) do { \
            rdge::WriteToLogFile(rdge::LogLevel::DEBUG, msg, FILE_NAME, __LINE__); \
            rdge::WriteToConsole(rdge::LogLevel::DEBUG, msg, FILE_NAME, __LINE__); \
        } while (false)
    #else
        #define DLOG(x) do { (void) sizeof ((x)); } while (0)
    #endif
#endif

#ifndef DLOG_IF
    #define DLOG_IF(cond, msg) do { if (cond) { DLOG(msg); } } while (0)
#endif

#ifndef ILOG
    #ifdef RDGE_DEBUG
        #define ILOG(msg) do { \
            rdge::WriteToLogFile(rdge::LogLevel::INFO, msg, FILE_NAME, __LINE__); \
            rdge::WriteToConsole(rdge::LogLevel::INFO, msg, FILE_NAME, __LINE__); \
        } while (false)
    #else
        #define ILOG(msg) do { \
            rdge::WriteToLogFile(rdge::LogLevel::INFO, msg); \
        } while (false)
    #endif
#endif

#ifndef WLOG
    #ifdef RDGE_DEBUG
        #define WLOG(msg) do { \
            rdge::WriteToLogFile(rdge::LogLevel::WARNING, msg, FILE_NAME, __LINE__); \
            rdge::WriteToConsole(rdge::LogLevel::WARNING, msg, FILE_NAME, __LINE__); \
        } while (false)
    #else
        #define WLOG(msg) do { \
            rdge::WriteToLogFile(rdge::LogLevel::WARNING, msg); \
        } while (false)
    #endif
#endif

#ifndef ELOG
    #ifdef RDGE_DEBUG
        #define ELOG(msg) do { \
            rdge::WriteToLogFile(rdge::LogLevel::ERROR, msg, FILE_NAME, __LINE__); \
            rdge::WriteToConsole(rdge::LogLevel::ERROR, msg, FILE_NAME, __LINE__); \
        } while (false)
    #else
        #define ELOG(msg) do { \
            rdge::WriteToLogFile(rdge::LogLevel::ERROR, msg); \
        } while (false)
    #endif
#endif

#ifndef FLOG
    #ifdef RDGE_DEBUG
        #define FLOG(msg) do { \
            rdge::WriteToLogFile(rdge::LogLevel::FATAL, msg, FILE_NAME, __LINE__); \
            rdge::WriteToConsole(rdge::LogLevel::FATAL, msg, FILE_NAME, __LINE__); \
        } while (false)
    #else
        #define FLOG(msg) do { \
            rdge::WriteToLogFile(rdge::LogLevel::FATAL, msg); \
        } while (false)
    #endif
#endif
