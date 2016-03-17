//! \headerfile <rdge/util/logger.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/05/2015
//! \bug

#pragma once

#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <unordered_map>
#include <utility>

#include <rdge/types.hpp>
#include <rdge/util/threadsafe_queue.hpp>
#include <rdge/util/worker_thread.hpp>

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
    Fatal
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

//! \class Logger
//! \brief Base RDGE Logger class
//! \details Base class for all logger objects that will be stored
//!          and accessed through the \ref RDGE::Util::LoggerFactory.
//!          Instantiation of the base class will write to a null stream.
class Logger
{
public:
    //! \brief Logger ctor
    //! \details Initialize logger object
    //! \param [in] id Unique identifier used to reference the object
    //! \param [in] min_log_level Minimum logging threshold
    explicit Logger (
                     const std::string&   id,
                     RDGE::Util::LogLevel min_log_level = RDGE::Util::LogLevel::Fatal
                    );

    //! \brief Logger Copy ctor
    //! \details Non-copyable
    Logger (const Logger&) = delete;

    //! \brief Logger Move ctor
    //! \details Default-moveable
    Logger (Logger&& other) = default;

    //! \brief Logger Copy Assignment Operator
    //! \details Non-copyable
    Logger& operator= (const Logger&) = delete;

    //! \brief Logger Move Assignment Operator
    //! \details Default-moveable
    Logger& operator= (Logger&&) = default;

    //! \brief Logger dtor
    virtual ~Logger ();

    //! \brief Unique ID used to access via /ref RDGE::LoggerFactory
    //! \returns Logger UID
    virtual std::string Id (void) const final
    {
        return m_id;
    }

    //! \brief Whether the logger object is actively logging
    //! \returns True if logging is enabled
    virtual bool IsActive (void) const final
    {
        return m_active;
    }

    //! \brief Toggles logger on/off state
    virtual void ToggleActive (void) final
    {
        m_active = !m_active;
    }

    //! \brief Write message to the stream
    //! \details Only implemented in derived classes
    //! \param [in] level Log severity
    //! \param [in] message Message to write to stream
    //! \param [in] filename Name of file
    //! \param [in] line Line number
    virtual void Write (
                        RDGE::Util::LogLevel level,
                        const std::string&   message,
                        const std::string&   filename,
                        RDGE::UInt32         line
                       )
    {
        RDGE::Unused(level);
        RDGE::Unused(message);
        RDGE::Unused(filename);
        RDGE::Unused(line);
    }

protected:
    std::string          m_id;
    RDGE::Util::LogLevel m_minLogLevel;
    bool                 m_active;

private:
    std::ostream* m_stream;
};

//! \class ConsoleLogger
//! \brief Logger object for writing to the standard console
//! \details Currently defaults to std::cout
class ConsoleLogger final : public Logger
{
public:
    //! \brief ConsoleLogger ctor
    //! \details Initialize logger object
    //! \param [in] id Unique identifier used to reference the object
    //! \param [in] min_log_level Minimum logging threshold
    explicit ConsoleLogger (
                            const std::string&   id,
                            RDGE::Util::LogLevel min_log_level
                           );

    //! \brief ConsoleLogger Copy ctor
    //! \details Non-copyable
    ConsoleLogger (const ConsoleLogger&) = delete;

    //! \brief ConsoleLogger Move ctor
    //! \details Default-moveable
    ConsoleLogger (ConsoleLogger&&) = default;

    //! \brief ConsoleLogger Copy Assignment Operator
    //! \details Non-copyable
    ConsoleLogger& operator= (const ConsoleLogger&) = delete;

    //! \brief ConsoleLogger Move Assignment Operator
    //! \details Default-moveable
    ConsoleLogger& operator= (ConsoleLogger&&) = default;

    //! \brief ConsoleLogger dtor
    virtual ~ConsoleLogger () { }

    //! \brief Write message to the stream
    //! \details Message will be written to console.  File and line number can be
    //!          be optionally included, but both need to be valid to be logged.
    //! \param [in] level Log severity
    //! \param [in] message Message to write to stream
    //! \param [in] filename Name of file
    //! \param [in] line Line number
    virtual void Write (
                        RDGE::Util::LogLevel level,
                        const std::string&   message,
                        const std::string&   filename = "",
                        RDGE::UInt32         line     = 0
                       ) final override;

private:
    std::ostream* m_stream;
};

//! \class FileLogger
//! \brief Logger object for writing to a file
//! \details Logging performed on the invoked thread, or class can optionally
//!          spawn it's own logging thread.
class FileLogger final : public Logger
{
public:
    //! \typedef LogQueue
    //! \brief Threadsafe queue for handling log requests
    using LogQueue = RDGE::Util::ThreadsafeQueue<std::unique_ptr<RDGE::Util::LogInfo>>;
    //! \typedef LogWorker
    //! \brief Logging worker thread
    using LogWorker = std::unique_ptr<RDGE::Util::WorkerThread>;

    //! \brief FileLogger ctor
    //! \details Initialize file logger object
    //! \param [in] id Unique identifier used to reference the object
    //! \param [in] min_log_level Minimum logging threshold
    //! \param [in] file Full file path to write to
    //! \param [in] overwrite Flag to overwrite current file content
    explicit FileLogger (
                         const std::string&   id,
                         RDGE::Util::LogLevel min_log_level,
                         const std::string&   file,
                         bool                 overwrite = true
                        );

    //! \brief FileLogger Copy ctor
    //! \details Non-copyable
    FileLogger (const FileLogger&) = delete;

    //! \brief FileLogger Move ctor
    //! \details Default-moveable
    FileLogger (FileLogger&&) = default;

    //! \brief FileLogger Copy Assignment Operator
    //! \details Non-copyable
    FileLogger& operator= (const FileLogger&) = delete;

    //! \brief FileLogger Move Assignment Operator
    //! \details Default-moveable
    FileLogger& operator= (FileLogger&&) = default;

    //! \brief FileLogger dtor
    virtual ~FileLogger ();

    //! \brief Write message to the stream
    //! \details Message will be wrote to file.  File and line number can be
    //!          be optionally included, but both need to be valid to be logged.
    //! \param [in] level Log severity
    //! \param [in] message Message to write to stream
    //! \param [in] filename Name of file
    //! \param [in] line Line number
    virtual void Write (
                        RDGE::Util::LogLevel level,
                        const std::string&   message,
                        const std::string&   filename = "",
                        RDGE::UInt32         line     = 0
                       ) final override;

private:
    std::string    m_file;
    std::ofstream* m_stream;

    LogQueue         m_queue;
    LogWorker        m_worker;
    std::atomic_bool m_workerRunning;
};


// TODO: ScopeLogger
//class ScopeLogger : public Logger
//{
//public:
    //explicit ScopeLogger (std::ostream
                          //const std::string& id,
                          //RDGE::
                         //);
//};

//! \class LoggerFactory
//! \brief Provides lookup/storage for logger objects
//! \details Caller must be aware the factory is not static.  Ownership and scope
//!          must be considered.
class LoggerFactory final
{
public:
    //! \typedef LoggerMap
    //! \brief Factory container for \ref Logger derived objects
    using LoggerMap = std::unordered_map<std::string, std::shared_ptr<Logger>>;

    //! \brief LoggerFactory ctor
    //! \details Initialize logger factory object
    LoggerFactory (void) { }

    //! \brief LoggerFactory dtor
    ~LoggerFactory (void) { }

    //! \brief LoggerFactory Copy ctor
    //! \details Non-copyable
    LoggerFactory (const LoggerFactory&) = delete;

    //! \brief LoggerFactory Move ctor
    //! \details Move semantics performed on the collection
    LoggerFactory (LoggerFactory&& rhs) noexcept
    {
        for (const auto& iter : rhs.m_map)
        {
            m_map.emplace(std::make_pair(iter.first, std::move(iter.second)));
        }
    }

    //! \brief LoggerFactory Copy Assignment Operator
    //! \details Non-copyable
    LoggerFactory& operator= (const LoggerFactory&) = delete;

    //! \brief LoggerFactory Move Assignment Operator
    //! \details Move semantics performed on the collection
    LoggerFactory& operator= (LoggerFactory&& rhs) noexcept
    {
        if (this != &rhs)
        {
            for (const auto& iter : m_map)
            {
                m_map.emplace(std::make_pair(iter.first, std::move(iter.second)));
            }
        }

        return *this;
    }

    //! \brief Get logger object from UID
    //! \details If object cannot be found a generic null stream based
    //!          logger will be added to the collection and returned.
    //!          Objects should be casted when requested.  If cast is
    //!          not done, the appropriate methods may not be called.
    //!          For example:
    //!          \code
    //!          auto l = std::dynamic_pointer_cast<RDGE::ConsoleLogger>(factory->Get("cout"));
    //!          \endcode
    //! \param [in] id UID for the lookup
    //! \returns Logger (or class derived from) shared pointer
    //! \throws Add to collection failed
    std::shared_ptr<Logger> Get (const std::string& id = "default");

    //! \brief Add logger to our factory collection
    //! \details If object cannot be found a generic null stream based
    //!          logger will be added to the collection and returned.
    //! \param [in] logger Logger object
    //! \returns Logger (or class derived from) shared pointer
    //! \throws Add to collection failed
    void Add (std::shared_ptr<Logger> logger);

    //! \brief Remove logger from our factory collection
    //! \details A return value of false means either the item could
    //!          not be located or an exception was silently handled
    //!          trying to remove it from the collection
    //! \param [in] id UID for the lookup
    //! \returns Boolean success
    bool Delete (const std::string& id);

private:
    LoggerMap m_map;
};

} // namespace Util
} // namespace RDGE
