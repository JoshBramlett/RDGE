#include <rdge/util/logger.hpp>
#include <rdge/application.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <ostream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cerrno>
#include <utility>

namespace RDGE {
namespace Util {

using namespace std::chrono;

namespace {
    // Select Graphic Rendition Codes (used for console formatting)
    // https://en.wikipedia.org/wiki/ANSI_escape_code
    enum class SGRCode : RDGE::UInt16
    {
        Reset             = 0,
        Bold              = 1,
        Underline         = 4,
        SlowBlink         = 5,
        BoldOff           = 22,
        UnderlineOff      = 24,
        SlowBlinkOff      = 25,
        BlackText         = 30,
        RedText           = 31,
        GreenText         = 32,
        YellowText        = 33,
        BlueText          = 34,
        MagentaText       = 35,
        CyanText          = 36,
        WhiteText         = 37,
        DefaultText       = 39,
        BlackBackground   = 40,
        RedBackground     = 41,
        GreenBackground   = 42,
        YellowBackground  = 43,
        BlueBackground    = 44,
        MagentaBackground = 45,
        CyanBackground    = 46,
        WhiteBackground   = 47,
        DefaultBackground = 49
    };

    std::ostream& operator<< (std::ostream& os, SGRCode code)
    {
        return os << "\033[" << static_cast<RDGE::UInt16>(code) << "m";
    }

    std::ostream& operator<< (std::ostream& os, LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Debug:
            return os << "[DEBUG]";
        case LogLevel::Info:
            return os << "[INFO]";
        case LogLevel::Warning:
            return os << "[WARN]";
        case LogLevel::Error:
            return os << "[ERROR]";
        case LogLevel::Fatal:
            return os << "[FATAL]";
        case LogLevel::Custom:
            return os << "[CUSTOM]";
        default:
            break;
        }

        return os << "[UNKNOWN]";
    }

    struct log_timestamp
    {
        bool include_milliseconds = false;
        bool use_gmt              = false;
    };

    std::ostream& operator<< (std::ostream& os, log_timestamp ts)
    {
        auto chrono_now = system_clock::now();
        time_t now = system_clock::to_time_t(chrono_now);
        struct tm t = (ts.use_gmt) ? *gmtime(&now) : *localtime(&now);

        auto epoch = chrono_now.time_since_epoch();
        auto ms = duration_cast<milliseconds>(epoch).count() % 1000;

        os << t.tm_year + 1900 << "/"
           << std::setfill('0')
           << std::setw(2) << t.tm_mon + 1 << "/"
           << std::setw(2) << t.tm_mday << " "
           << std::setw(2) << t.tm_hour << ":"
           << std::setw(2) << t.tm_min << ":"
           << std::setw(2) << t.tm_sec;

        if (ts.include_milliseconds)
        {
            os << "." << std::setw(4) << ms;
        }

        return os << std::setfill(' ');
    }
}

//////////////////////////////////////////////////////////
//                  ConsoleLogger
//////////////////////////////////////////////////////////

ConsoleLogger::ConsoleLogger (
                              LogLevel min_level,
                              bool     include_ms,
                              bool     use_gmt
                             )
    : m_minLogLevel(min_level)
    , m_includeMilliseconds(include_ms)
    , m_useGMT(use_gmt)
{ }

void
ConsoleLogger::Write (
                      RDGE::Util::LogLevel level,
                      const std::string&   message,
                      const std::string&   filename,
                      RDGE::UInt32         line
                     )
{
    if (level >= m_minLogLevel)
    {
        auto out = (level >= LogLevel::Error) ? &std::cerr : &std::cout;

        log_timestamp ts { m_includeMilliseconds, m_useGMT };
        std::stringstream location;
        if (!filename.empty() && line > 0)
        {
            location << "(" << filename << ":" << line << ") ";
        }

        switch (level)
        {
        case LogLevel::Debug:
            *out << SGRCode::WhiteText << ts << " "
                 << SGRCode::CyanText << level << " "
                 << SGRCode::YellowText << location.str()
                 << SGRCode::Reset << message
                 << std::endl;
            break;
        case LogLevel::Info:
            *out << SGRCode::WhiteText << ts << " "
                 << SGRCode::BlueText << level << " "
                 << SGRCode::YellowText << location.str()
                 << SGRCode::Reset << message
                 << std::endl;
            break;
        case LogLevel::Warning:
            *out << SGRCode::WhiteText << ts << " "
                 << SGRCode::MagentaText << level << " "
                 << SGRCode::YellowText << location.str()
                 << SGRCode::Reset << message
                 << std::endl;
            break;
        case LogLevel::Error:
            *out << SGRCode::WhiteText << ts << " "
                 << SGRCode::Bold << SGRCode::RedText << level << SGRCode::BoldOff << " "
                 << SGRCode::YellowText << location.str()
                 << SGRCode::Reset << message
                 << std::endl;
            break;
        case LogLevel::Fatal:
            *out << SGRCode::WhiteText << ts << " "
                 << SGRCode::Bold << SGRCode::RedBackground << level << SGRCode::Reset << " "
                 << SGRCode::YellowText << location.str()
                 << SGRCode::Reset << message
                 << std::endl;
            break;
        case LogLevel::Custom:
            *out << SGRCode::WhiteText << ts << " "
                 << SGRCode::Bold << SGRCode::GreenBackground << level << SGRCode::Reset << " "
                 << SGRCode::YellowText << location.str()
                 << SGRCode::Reset << message
                 << std::endl;
            break;
        }
    }
}

//////////////////////////////////////////////////////////
//                    FileLogger
//////////////////////////////////////////////////////////

FileLogger::FileLogger (
                        std::string file,
                        LogLevel    min_level,
                        bool        overwrite,
                        bool        include_ms,
                        bool        use_gmt
                       )
    : m_file(std::move(file))
    , m_minLogLevel(min_level)
    , m_includeMilliseconds(include_ms)
    , m_useGMT(use_gmt)
    , m_active(true)
    , m_stream(nullptr)
    , m_worker(nullptr)
    , m_workerRunning(false)
{
    auto mode = (overwrite) ? std::ofstream::trunc : std::ofstream::app;

    m_stream = new std::ofstream();
    m_stream->open(m_file, std::ofstream::out | mode);
    if (m_stream->is_open() == false)
    {
        std::stringstream ss;
        ss << "File cannot open:"
           << " file=" << m_file
           << " code=" << errno
           << " msg=" << std::strerror(errno);

        RDGE_THROW(ss.str());
    }

    m_worker = std::make_unique<WorkerThread>([this]() {
        while (m_workerRunning)
        {
            std::unique_ptr<LogInfo> entry;
            if (m_queue.WaitAndPop(entry, 250) && m_workerRunning)
            {
                if (m_stream->is_open() && m_active && entry->level >= m_minLogLevel)
                {
                    log_timestamp ts { m_includeMilliseconds, m_useGMT };
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

                    m_stream->flush();
                }
            }
        }
    }, [this]() { m_workerRunning = false; });

    m_workerRunning = true;
}

FileLogger::FileLogger (FileLogger&& rhs) noexcept
    : m_file(std::move(rhs.m_file))
    , m_minLogLevel(rhs.m_minLogLevel)
    , m_includeMilliseconds(rhs.m_includeMilliseconds)
    , m_useGMT(rhs.m_useGMT)
    , m_stream(rhs.m_stream)
    , m_queue(std::move(rhs.m_queue))
    , m_worker(std::move(rhs.m_worker))
{
    m_active.store(rhs.m_active);
    m_workerRunning.store(rhs.m_workerRunning);

    rhs.m_stream = nullptr;
}

FileLogger&
FileLogger::operator= (FileLogger&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_worker->Stop();

        if (m_stream->is_open())
        {
            m_stream->close();
        }

        delete m_stream;

        m_file = std::move(rhs.m_file);
        m_minLogLevel = rhs.m_minLogLevel;
        m_includeMilliseconds = rhs.m_includeMilliseconds;
        m_useGMT = rhs.m_useGMT;
        m_active.store(rhs.m_active);
        m_stream = rhs.m_stream;
        m_queue = std::move(rhs.m_queue);
        m_worker = std::move(rhs.m_worker);
        m_workerRunning.store(rhs.m_workerRunning);

        rhs.m_stream = nullptr;
    }

    return *this;
}

FileLogger::~FileLogger (void)
{
    m_worker->Stop();

    if (m_stream->is_open())
    {
        m_stream->close();
    }

    delete m_stream;
}

void
FileLogger::SetActive (bool active)
{
    m_active = active;
}

void
FileLogger::Write (
                   RDGE::Util::LogLevel level,
                   const std::string&   message,
                   const std::string&   filename,
                   RDGE::UInt32         line
                  )
{
    m_queue.Push(std::make_unique<LogInfo>(LogInfo { level, message, filename, line }));
}

//////////////////////////////////////////////////////////
//                    ScopeLogger
//////////////////////////////////////////////////////////

ScopeLogger::ScopeLogger (
                          std::string        identifier,
                          const std::string& function_name,
                          const std::string& filename
                         )
    : m_identifier(std::move(identifier))
{
    std::stringstream ss;
    ss << "ScopeLogger[" << m_identifier << "] START";

    if (!function_name.empty())
    {
        ss << " function_name=" << function_name;
    }

    if (!filename.empty())
    {
        ss << " filename=" << filename;
    }

    RDGE::WriteToConsole(LogLevel::Custom, ss.str());

    // for more accurate results, the last thing must be to get the timestamp
    m_startPoint = time_point_cast<Duration>(HiResClock::now());
}

ScopeLogger::~ScopeLogger (void)
{
    auto stop = HiResClock::now();
    auto delta = duration_cast<Duration>(stop - m_startPoint).count();

    std::stringstream ss;
    ss << "ScopeLogger[" << m_identifier << "] STOP delta=" << delta << "μs";

    RDGE::WriteToConsole(LogLevel::Custom, ss.str());
}

} // namespace Util
} // namespace RDGE
