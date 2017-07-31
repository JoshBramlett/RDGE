//! \headerfile <rdge/util/logger.hpp>
//! \author Josh Bramlett
//! \version 0.0.3
//! \date 05/13/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/containers/threadsafe_queue.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <sstream>
#include <fstream>

#define LOG_S_BASE(_level) \
    ((rdge::LogLevel::_level) < rdge::GetMinLogLevel()) ? (void)0 : \
    rdge::detail::VoidStream() & \
    rdge::detail::LogStream((rdge::LogLevel::_level), FILE_NAME, __LINE__).Stream()

#define LOG_S_IF(_cond, _level) \
    (!(_cond)) ? (void)0 : LOG_S_BASE(_level)

#define DLOG() LOG_S_BASE(DEBUG)
#define ILOG() LOG_S_BASE(INFO)
#define WLOG() LOG_S_BASE(WARNING)
#define ELOG() LOG_S_BASE(ERROR)
#define FLOG() LOG_S_BASE(FATAL)
#define CLOG() LOG_S_BASE(CUSTOM)

#define DLOG_IF(_cond) LOG_S_IF(_cond, DEBUG)
#define ILOG_IF(_cond) LOG_S_IF(_cond, INFO)
#define WLOG_IF(_cond) LOG_S_IF(_cond, WARNING)
#define ELOG_IF(_cond) LOG_S_IF(_cond, ERROR)
#define FLOG_IF(_cond) LOG_S_IF(_cond, FATAL)
#define CLOG_IF(_cond) LOG_S_IF(_cond, CUSTOM)

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \enum LogLevel
//! \brief Log Level Severity
enum class LogLevel : uint8
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    CUSTOM
};


//! \namespace detail Internal
namespace detail {

//! \brief Global minimum log level
extern std::atomic<LogLevel> g_minLogLevel;

//! \class LogStream
//! \brief Used internally for logging macros
class LogStream
{
public:
    explicit LogStream (LogLevel, const char*, int32);
    ~LogStream (void) noexcept;

    std::ostringstream& Stream (void) { return m_stream; }

private:
    LogLevel            m_level;
    const char*         m_file;
    int32               m_line;
    std::ostringstream& m_stream;
};

//! \class LogStream
//! \brief Used internally for logging macros
//! \details Allows macros to avoid processing after conditional failure.
class VoidStream
{
public:
    void operator& (std::ostream&) const { }
};

} // namespace detail


//! \struct log_record
//! \brief Container for asynchronous logging
struct log_record
{
    LogLevel    level;
    const char* file;
    int32       line;
    std::string message;
};

//! \class RecordHandler
//! \brief Base class for handling log messages
//! \details Derived classes must implement the Log method which will
//!          be called when a logging message is ready to be processed.
class RecordHandler
{
public:
    RecordHandler (bool use_gmt = false, bool log_milli = false);
    virtual ~RecordHandler (void) noexcept = default;

    virtual void Log (const log_record&) = 0;

protected:
    enum StateFlags
    {
        USE_GMT          = 0x0001,
        LOG_MILLISECONDS = 0x0002
    };

    uint16 m_flags = 0;
};

//! \class ConsoleRecordHandler
//! \brief Writes log messages to the console
class ConsoleRecordHandler final : public RecordHandler
{
public:
    //! \brief ConsoleRecordHandler ctor
    //! \param [in] use_gmt True for GMT timestamps, false for local
    //! \param [in] log_milli Include milliseconds in the timestamp
    ConsoleRecordHandler (bool use_gmt = false, bool log_milli = false);

    //! \brief Process log record
    void Log (const log_record&) override;
};

//! \class FileRecordHandler
//! \brief Writes log messages to a file
class FileRecordHandler final : public RecordHandler
{
public:
    //! \brief FileRecordHandler ctor
    //! \details Opens the file stream
    //! \param [in] file File to write to
    //! \param [in] overwrite True to overwrite file contents, false to append
    //! \param [in] use_gmt True for GMT timestamps, false for local
    //! \param [in] log_milli Include milliseconds in the timestamp
    explicit FileRecordHandler (std::string file,
                                bool        overwrite = true,
                                bool        use_gmt = false,
                                bool        log_milli = false);

    //! \brief FileRecordHandler dtor
    ~FileRecordHandler (void) noexcept;

    //!@{ Copy/Move disabled
    FileRecordHandler (const FileRecordHandler&) = delete;
    FileRecordHandler& operator= (const FileRecordHandler&) = delete;
    FileRecordHandler (FileRecordHandler&&) = delete;
    FileRecordHandler& operator= (FileRecordHandler&&) = delete;
    //!@}

    //! \brief Process log record
    void Log (const log_record&) override;

private:
    std::string   m_file;
    std::ofstream m_stream;
};

// Logger is implemented using a single entry for logging, which passes
// the messages to a list of "handlers".  When a log entry is added, it
// is done so on a thread safe async queue before processing.
//
// Limitations:
//   - Cannot control individual handler behavior once added
//   - Slow.  Everything is lock protected.
//
// TODO
//
//   - Initialize is not currently configurable, and creates a console and
//     a (hard coded) file logger.  When updates are made to the game
//     settings logger initialization should be updated.
//   - Finish documentation.


void
InitializeLogger (void);

void
AddRecordHandler (std::unique_ptr<RecordHandler>&& handler);

inline LogLevel
GetMinLogLevel (void)
{
    return detail::g_minLogLevel.load(std::memory_order_relaxed);
}

inline void
SetMinLogLevel (LogLevel level)
{
    return detail::g_minLogLevel.store(level, std::memory_order_relaxed);
}


//! \class ScopeLogger
//! \brief Logs when created and destroyed
//! \details Used for profiling, a high resolution time point is recorded on
//!          object creation and destruction, and creates a log entry showing
//!          the execution time delta.
template <typename Duration = std::chrono::microseconds>
class ScopeLogger
{
public:
    using Clock = std::chrono::high_resolution_clock;

    //! \brief ScopeLogger ctor
    //! \details Records a starting time point.
    //! \param [in] identifier Used to identify the object
    explicit ScopeLogger (std::string identifier)
        : m_identifier(identifier)
        , m_start(std::chrono::time_point_cast<Duration>(Clock::now()))
    { }

    //! \brief ScopeLogger dtor
    //! \details Records an ending time point and logs the delta.
    ~ScopeLogger (void) noexcept
    {
        auto end = std::chrono::time_point_cast<Duration>(Clock::now());
        CLOG() << "ScopeLogger[" << m_identifier << "]"
               << " delta=" << std::chrono::duration_cast<Duration>(end - m_start).count();
    }

    //!@{ Copy/Move disabled
    ScopeLogger (const ScopeLogger&) = delete;
    ScopeLogger& operator= (const ScopeLogger&) = delete;
    ScopeLogger (ScopeLogger&&) = delete;
    ScopeLogger& operator= (ScopeLogger&&) = delete;
    //!@}

private:
    std::string m_identifier;
    std::chrono::time_point<std::chrono::high_resolution_clock, Duration> m_start;
};

} // namespace rdge
