//! \headerfile <rdge/util/logger.hpp>
//! \author Josh Bramlett
//! \version 0.0.3
//! \date 05/13/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/util/threadsafe_queue.hpp>
#include <rdge/util/worker_thread.hpp>

#include <string>
#include <memory>
#include <atomic>
#include <fstream>
#include <chrono>

//! \namespace RDGE: Rainbow Drop Game Engine
namespace RDGE {
namespace Util {

//! \enum LogLevel
//! \brief Log Level Severity
enum class LogLevel : RDGE::UInt8
{
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal,
    Custom
};

//! \struct LogInfo
//! \brief POD container holding info to log
//! \details Used for passing to logging worker threads.
struct LogInfo
{
    RDGE::Util::LogLevel level;
    std::string          message;
    std::string          filename;
    RDGE::UInt32         line;
};

//! \class ConsoleLogger
//! \brief Logger object for writing to the standard console
//! \details Fatal and Error messages will be written to std::cerr whereas all
//!          others will write to std::cout.  Messages are formatted appropriately
//!          to ensure ease of visibility and help detect important messages.
class ConsoleLogger
{
public:
    //! \brief ConsoleLogger ctor
    //! \details Initialize console logger object
    //! \param [in] min_level Minimum logging threshold
    //! \param [in] include_ms Include milliseconds in the timestamp
    //! \param [in] use_gmt Timestamps in GMT (false is local time)
    explicit ConsoleLogger (
                            LogLevel min_level  = LogLevel::Warning,
                            bool     include_ms = false,
                            bool     use_gmt    = false
                           );

    //! \brief ConsoleLogger Copy ctor
    //! \details Default-copyable
    ConsoleLogger (const ConsoleLogger&) = default;

    //! \brief ConsoleLogger Move ctor
    //! \details Default-moveable
    ConsoleLogger (ConsoleLogger&&) = default;

    //! \brief ConsoleLogger Copy Assignment Operator
    //! \details Default-copyable
    ConsoleLogger& operator= (const ConsoleLogger&) = default;

    //! \brief ConsoleLogger Move Assignment Operator
    //! \details Default-moveable
    ConsoleLogger& operator= (ConsoleLogger&&) = default;

    //! \brief ConsoleLogger dtor
    ~ConsoleLogger (void) { }

    //! \brief Write message to the stream
    //! \details Message will be written to console.  File and line number can be
    //!          be optionally included, but both need to be valid to be logged.
    //! \param [in] level Log severity
    //! \param [in] message Message to write to stream
    //! \param [in] filename Name of file
    //! \param [in] line Line number
    void Write (
                RDGE::Util::LogLevel level,
                const std::string&   message,
                const std::string&   filename = "",
                RDGE::UInt32         line     = 0
               );

private:
    LogLevel m_minLogLevel;
    bool     m_includeMilliseconds;
    bool     m_useGMT;
};

//! \class FileLogger
//! \brief Logger object for writing to a file
//! \details Each FileLogger instance will create it's own thread for
//!          processing.  The class is thread safe so messages can be
//!          written from any thread.
class FileLogger
{
public:
    //! \typedef LogQueue
    //! \brief Threadsafe queue for handling log requests
    using LogQueue = ThreadsafeQueue<std::unique_ptr<LogInfo>>;
    //! \typedef LogWorker
    //! \brief Logging worker thread
    using LogWorker = std::unique_ptr<WorkerThread>;

    //! \brief FileLogger ctor
    //! \details Initialize logger object
    //! \param [in] file File to write to
    //! \param [in] min_level Minimum logging threshold
    //! \param [in] overwrite Whether create a new file or append to existing
    //! \param [in] include_ms Include milliseconds in the timestamp
    //! \param [in] use_gmt Timestamps in GMT (false is local time)
    explicit FileLogger (
                         std::string file,
                         LogLevel    min_level  = LogLevel::Warning,
                         bool        overwrite  = true,
                         bool        include_ms = false,
                         bool        use_gmt    = false
                        );

    //! \brief FileLogger Copy ctor
    //! \details Non-copyable
    FileLogger (const FileLogger&) = delete;

    //! \brief FileLogger Move ctor
    //! \details Transfers ownership
    FileLogger (FileLogger&&) noexcept;

    //! \brief FileLogger Copy Assignment Operator
    //! \details Non-copyable
    FileLogger& operator= (const FileLogger&) = delete;

    //! \brief FileLogger Move Assignment Operator
    //! \details Transfers ownership
    FileLogger& operator= (FileLogger&&) noexcept;

    //! \brief FileLogger dtor
    ~FileLogger (void);

    //! \brief Whether the logger object is actively logging
    //! \returns True if logging is enabled
    bool IsActive (void) const { return m_active; }

    //! \brief Sets whether the logger object is active
    //! \details Activation/deactivation may not have an obvious immediate
    //!          effect as the thread writes at a set interval.  For example,
    //!          writing an entry and then deactivating the logger will not
    //!          log the message if the deactivate value was set prior to the
    //!          logger thread processing the message.
    //! \param [in] active True to log messages, false to drop all requests
    void SetActive (bool active);

    //! \brief Write message to the stream
    //! \details Only implemented in derived classes
    //! \param [in] level Log severity
    //! \param [in] message Message to write to stream
    //! \param [in] filename Name of file
    //! \param [in] line Line number
    void Write (
                LogLevel           level,
                const std::string& message,
                const std::string& filename = "",
                RDGE::UInt32       line     = 0
               );

private:
    std::string      m_file;
    LogLevel         m_minLogLevel;
    bool             m_includeMilliseconds;
    bool             m_useGMT;
    std::atomic_bool m_active;

    std::ofstream*   m_stream;
    LogQueue         m_queue;
    LogWorker        m_worker;
    std::atomic_bool m_workerRunning;
};

//! \class ScopeLogger
//! \brief Logs to std::cout when created and destroyed
//! \details RAII compliant object which implements a high frequency timer
//!          and logs the delta between instantiation and destruction.  It's
//!          intended purpose is for profiling.
class ScopeLogger
{
public:
    //! \brief ScopeLogger ctor
    //! \details Initialize logger object
    //! \param [in] identifier Used to identify the object
    //! \param [in] function_name Name of the function where the object resides
    //! \param [in] filename File where the object resides
    explicit ScopeLogger (
                          std::string        identifier,
                          const std::string& function_name = "",
                          const std::string& filename      = ""
                         );

    //! \brief ScopeLogger Copy ctor
    //! \details Non-copyable
    ScopeLogger (const ScopeLogger&) = delete;

    //! \brief ScopeLogger Move ctor
    //! \details Non-movable
    ScopeLogger (ScopeLogger&&) = delete;

    //! \brief ScopeLogger Copy Assignment Operator
    //! \details Non-copyable
    ScopeLogger& operator= (const ScopeLogger&) = delete;

    //! \brief ScopeLogger Move Assignment Operator
    //! \details Non-movable
    ScopeLogger& operator= (ScopeLogger&&) = delete;

    //! \brief ScopeLogger dtor
    ~ScopeLogger (void);

private:
    using HiResClock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::microseconds;

    std::string m_identifier;
    std::chrono::time_point<HiResClock, Duration> m_startPoint;
};


} // namespace Util

// Promote to RDGE namespace
using Util::FileLogger;
using Util::LogLevel;

} // namespace RDGE
