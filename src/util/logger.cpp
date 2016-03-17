#include <iomanip>
#include <ctime>
#include <cerrno>
#include <sstream>

#include <rdge/util/logger.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/onullstream>

namespace RDGE {
namespace Util {

namespace
{
    std::onullstream s_nullStream;

    std::ostream& operator<< (std::ostream& os, RDGE::Util::LogLevel level)
    {
        switch (level)
        {
            case RDGE::Util::LogLevel::Debug:
                return os << "[DEBUG]";
            case RDGE::Util::LogLevel::Info:
                return os << "[INFO]";
            case RDGE::Util::LogLevel::Warning:
                return os << "[WARN]";
            case RDGE::Util::LogLevel::Error:
                return os << "[ERROR]";
            case RDGE::Util::LogLevel::Fatal:
                return os << "[FATAL]";
        }

        return os;
    }

    // TODO: This needs work - keep the stream op overload,
    //       however the timestamp should be configurable.
    //         1)  timestmap format string
    //         2)  Deltas & nano/micro precision
    //
    //       Also, make sure whatever method we use for the timestamps
    //       we pull the header(s) in.
    struct Timestamp
    {
        bool use_gmt = true;
        bool include_milliseconds = false;
    };

    std::ostream& operator<< (std::ostream& os, Timestamp ts)
    {
        time_t now = time(NULL);
        struct tm t = (ts.use_gmt)
                        ? *gmtime(&now)
                        : *localtime(&now);

        return os << t.tm_year + 1900 << "/"
                  << std::setfill('0')
                  << std::setw(2) << t.tm_mon + 1 << "/"
                  << std::setw(2) << t.tm_mday << " "
                  << std::setw(2) << t.tm_hour << ":"
                  << std::setw(2) << t.tm_min << ":"
                  << std::setw(2) << t.tm_sec
                  << std::setfill(' ');
    }
}

//////////////////////////////////////////////////////////
//                   LoggerFactory
//////////////////////////////////////////////////////////

std::shared_ptr<Logger>
LoggerFactory::Get (const std::string& id)
{
    auto iter = m_map.find(id);
    if (iter == m_map.end())
    {
        auto emplaced = m_map.emplace(id, std::make_shared<Logger>(id));
        if (emplaced.second == false)
        {
            RDGE_THROW("Failed to add base Logger.  id=" + id);
        }

        iter = emplaced.first;
    }

    return iter->second;
}

void
LoggerFactory::Add (std::shared_ptr<Logger> logger)
{
    auto id = logger->Id();
    auto iter = m_map.find(id);
    if (iter == m_map.end())
    {
        auto emplaced = m_map.emplace(id, logger);
        if (emplaced.second == false)
        {
            RDGE_THROW("Failed to add Logger.  id=" + id);
        }
    }
    else
    {
        iter->second = logger;
    }
}

bool
LoggerFactory::Delete (const std::string& id)
{
    try
    {
        return (m_map.erase(id) == 1);
    }
    catch (std::exception& ex) { }

    return false;
}

//////////////////////////////////////////////////////////
//                   Logger (Base)
//////////////////////////////////////////////////////////

Logger::Logger (
                const std::string&   id,
                RDGE::Util::LogLevel min_log_level
               )
    : m_id(id)
    , m_minLogLevel(min_log_level)
    , m_active(true)
{
    m_stream = &s_nullStream;
}

Logger::~Logger (void)
{
    m_stream = nullptr;
}

//////////////////////////////////////////////////////////
//                  ConsoleLogger
//////////////////////////////////////////////////////////

ConsoleLogger::ConsoleLogger (
                              const std::string&   id,
                              RDGE::Util::LogLevel min_log_level
                             )
    : Logger(id, min_log_level)
{
    m_stream = &std::cout;
}

void
ConsoleLogger::Write (
                      RDGE::Util::LogLevel level,
                      const std::string&   message,
                      const std::string&   filename,
                      RDGE::UInt32         line
                     )
{
    if (m_active && m_minLogLevel >= level)
    {
        Timestamp ts;
        std::stringstream location;
        if (!filename.empty() && line > 0)
        {
            location << "(" << filename << ":" << line << ") ";
        }

        *m_stream << ts << " "
                  << level << " "
                  << location.str()
                  << message
                  << std::endl;

    }
}

//////////////////////////////////////////////////////////
//                    FileLogger
//////////////////////////////////////////////////////////

FileLogger::FileLogger (
                        const std::string&   id,
                        RDGE::Util::LogLevel min_log_level,
                        const std::string&   file,
                        bool                 overwrite
                       )
    : Logger(id, min_log_level)
    , m_file(file)
    , m_worker(nullptr)
    , m_workerRunning(false)
{
    auto mode = (overwrite) ? std::ios::trunc : std::ios::app;

    m_stream = new std::ofstream();
    m_stream->open(file, mode);
    if (m_stream->is_open() == false)
    {
        std::stringstream ss;
        ss << "File cannot open:"
           << " file=" << m_file
           << " code=" << errno
           << " msg=" << std::strerror(errno);

        RDGE_THROW(ss.str());
    }

    m_worker = std::make_unique<RDGE::Util::WorkerThread>([this]() {
        while (m_workerRunning)
        {
            std::unique_ptr<RDGE::Util::LogInfo> entry;
            if (m_queue.WaitAndPop(entry, 250) && m_workerRunning)
            {
                if (m_stream->is_open() && entry->level >= m_minLogLevel)
                {
                    Timestamp ts;
                    std::stringstream location;
                    if (!entry->filename.empty() && entry->line > 0)
                    {
                        location << "(" << entry->filename << ":" << entry->line << ") ";
                    }

                    *m_stream << ts << " "
                              << entry->level << " "
                              << location.str()
                              << entry->message
                              << std::endl;
                }
            }
        }
    }, [this]() { m_workerRunning = false; });

    m_workerRunning = true;
}

FileLogger::~FileLogger (void)
{
    if (m_stream->is_open())
    {
        m_stream->close();
    }
}

void
FileLogger::Write (
                   RDGE::Util::LogLevel level,
                   const std::string&   message,
                   const std::string&   filename,
                   RDGE::UInt32         line
                  )
{
    auto entry = RDGE::Util::LogInfo({
        level,
        message,
        filename,
        line
    });

    m_queue.Push(std::make_unique<RDGE::Util::LogInfo>(entry));
}

} // namespace Util
} // namespace RDGE
