#include <rdge/util/logger.hpp>
#include <rdge/application.hpp>
#include <rdge/util/exception.hpp>

#include <thread>
#include <iomanip>
#include <cerrno>
#include <utility>
#include <system_error>

using namespace std::chrono;

namespace rdge {

namespace {

// Select Graphic Rendition Codes (used for console formatting)
// https://en.wikipedia.org/wiki/ANSI_escape_code
enum class SGRCode : uint16
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
    return os << "\033[" << static_cast<uint16>(code) << "m";
}

std::ostream& operator<< (std::ostream& os, LogLevel level)
{
    switch (level)
    {
    case LogLevel::DEBUG:
        return os << "[DEBUG]";
    case LogLevel::INFO:
        return os << "[INFO]";
    case LogLevel::WARNING:
        return os << "[WARN]";
    case LogLevel::ERROR:
        return os << "[ERROR]";
    case LogLevel::FATAL:
        return os << "[FATAL]";
    case LogLevel::CUSTOM:
        return os << "[CUSTOM]";
    default:
        break;
    }

    return os << "[UNKNOWN]";
}

std::string
GetFormattedTimestamp (bool use_gmt, bool include_milliseconds)
{
    auto tp_now = system_clock::now();
    time_t now = system_clock::to_time_t(tp_now);
    struct tm t = (use_gmt) ? *gmtime(&now) : *localtime(&now);

    std::ostringstream ss;
    ss << (t.tm_year + 1900) << "/"
       << std::setfill('0')
       << std::setw(2) << (t.tm_mon + 1) << "/"
       << std::setw(2) << t.tm_mday << " "
       << std::setw(2) << t.tm_hour << ":"
       << std::setw(2) << t.tm_min << ":"
       << std::setw(2) << t.tm_sec;

    if (include_milliseconds)
    {
        auto epoch = tp_now.time_since_epoch();
        auto ms = duration_cast<milliseconds>(epoch).count() % 1000;

        ss << "." << std::setw(4) << ms;
    }

    return ss.str();
}

std::ostringstream&
GetThreadLocalStream (void)
{
    // TODO Perf: This could be modified to use a custom allocator, which would
    //      limit the number of dynamic allocations
    thread_local std::ostringstream ss;
    ss.str(std::string());
    ss.clear();
    return ss;
}

class AsyncLogHandler
{
public:
    AsyncLogHandler (void)
    {
        m_thread = std::thread([=](void) {
            m_running.store(true, std::memory_order_relaxed);
            using namespace std::chrono_literals;
            while (m_running.load(std::memory_order_relaxed))
            {
                log_record record;
                if (m_queue.wait_and_pop(record, 250ms))
                {
                    if (record.level < GetMinLogLevel())
                    {
                        continue;
                    }

                    if (m_running.load(std::memory_order_relaxed))
                    {
                        // TODO Perf: Remove lock and have consuming thread add/remove
                        //      handlers.  Would require making the queue consume an
                        //      abstract type and branch.
                        std::lock_guard<std::mutex> guard(m_handlerMutex);
                        for (auto& handler : m_handlers)
                        {
                            handler->Log(record);
                        }
                    }
                }
            }
        });
    }

    ~AsyncLogHandler (void) noexcept
    {
        m_running.store(false, std::memory_order_relaxed);

        try {
            m_thread.join(); // thread should always be joinable
        } catch (const std::system_error& ex) {
            std::cerr << "~AsyncLogHandler Exception caught:"
                      << " what=" << ex.what() << '\n';
        }
    }

    void AddHandler (std::unique_ptr<RecordHandler>&& handler)
    {
        std::lock_guard<std::mutex> guard(m_handlerMutex);
        m_handlers.emplace_back(std::move(handler));
    }

    void Log (log_record&& record)
    {
        if (record.level < GetMinLogLevel())
        {
            return;
        }

        m_queue.push(std::move(record));
    }

private:
    threadsafe_queue<log_record> m_queue;
    std::thread                  m_thread;
    std::atomic_bool             m_running = ATOMIC_VAR_INIT(false);

    std::mutex m_handlerMutex;
    std::vector<std::unique_ptr<RecordHandler>> m_handlers;
};

std::unique_ptr<AsyncLogHandler> s_logHandler;

} // anonymous namespace


namespace detail {

#ifdef RDGE_DEBUG
std::atomic<LogLevel> g_minLogLevel = ATOMIC_VAR_INIT(LogLevel::DEBUG);
#else
std::atomic<LogLevel> g_minLogLevel = ATOMIC_VAR_INIT(LogLevel::WARNING);
#endif

LogStream::LogStream (LogLevel level, const char* file, int32 line)
    : m_level(level)
    , m_file(file)
    , m_line(line)
    , m_stream(GetThreadLocalStream())
{ }

LogStream::~LogStream (void) noexcept
{
    if (s_logHandler)
    {
        s_logHandler->Log(log_record { m_level, m_file, m_line, m_stream.str() });
    }
}

} // namespace detail


//////////////////////////////////////////////////////////
//                   RecordHandler
//////////////////////////////////////////////////////////

RecordHandler::RecordHandler (bool use_gmt, bool log_milli)
{
    if (use_gmt)
    {
        m_flags |= USE_GMT;
    }

    if (log_milli)
    {
        m_flags |= LOG_MILLISECONDS;
    }
}

//////////////////////////////////////////////////////////
//                 ConsoleRecordHandler
//////////////////////////////////////////////////////////

ConsoleRecordHandler::ConsoleRecordHandler (bool use_gmt, bool log_milli)
    : RecordHandler(use_gmt, log_milli)
{ }

void
ConsoleRecordHandler::Log (const log_record& record)
{
    auto& out = (record.level >= LogLevel::ERROR) ? std::cerr : std::cout;

    auto ts = GetFormattedTimestamp(m_flags & USE_GMT, m_flags & LOG_MILLISECONDS);
    out << SGRCode::WhiteText << ts << " ";

    switch (record.level)
    {
    case LogLevel::DEBUG:
        out << SGRCode::CyanText << record.level << " ";
        break;
    case LogLevel::INFO:
        out << SGRCode::BlueText << record.level << " ";
        break;
    case LogLevel::WARNING:
        out << SGRCode::MagentaText << record.level << " ";
        break;
    case LogLevel::ERROR:
        out << SGRCode::Bold << SGRCode::RedText << record.level << SGRCode::BoldOff << " ";
        break;
    case LogLevel::FATAL:
        out << SGRCode::Bold << SGRCode::RedBackground << record.level << SGRCode::Reset << " ";
        break;
    case LogLevel::CUSTOM:
        out << SGRCode::Bold << SGRCode::GreenBackground << record.level << SGRCode::Reset << " ";
        break;
    }

#ifdef RDGE_DEBUG
    if (record.file)
    {
        out << SGRCode::YellowText << "(" << record.file << ":" << record.line << ") ";
    }
#endif

    out << SGRCode::Reset << record.message << '\n';
    out.flush();
}

//////////////////////////////////////////////////////////
//                 FileRecordHandler
//////////////////////////////////////////////////////////

FileRecordHandler::FileRecordHandler (std::string file,
                                      bool overwrite,
                                      bool use_gmt,
                                      bool log_milli)
    : RecordHandler(use_gmt, log_milli)
    , m_file(std::move(file))
{
    auto mode = (overwrite) ? std::ofstream::trunc : std::ofstream::app;
    m_stream.open(m_file, std::ofstream::out | mode);
    if (!m_stream.is_open())
    {
        std::stringstream ss;
        ss << "File cannot open:"
           << " file=" << m_file
           << " code=" << errno
           << " msg=" << std::strerror(errno);

        RDGE_THROW(ss.str());
    }
}

FileRecordHandler::~FileRecordHandler (void) noexcept
{
    if (m_stream.is_open())
    {
        m_stream.close();
    }
}

void
FileRecordHandler::Log (const log_record& record)
{
    if (m_stream.is_open())
    {
        auto ts = GetFormattedTimestamp(m_flags & USE_GMT, m_flags & LOG_MILLISECONDS);
        m_stream << ts << " "
                 << record.level << " ";

#ifdef RDGE_DEBUG
        if (record.file)
        {
            m_stream << "(" << record.file << ":" << record.line << ") ";
        }
#endif

        m_stream << record.message << '\n';
        m_stream.flush();
    }
}

//////////////////////////////////////////////////////////
//                    Global Methods
//////////////////////////////////////////////////////////

void
InitializeLogger (void)
{
    if (!s_logHandler)
    {
        s_logHandler = std::make_unique<AsyncLogHandler>();

        s_logHandler->AddHandler(std::make_unique<ConsoleRecordHandler>());
        s_logHandler->AddHandler(std::make_unique<FileRecordHandler>("rdge.log"));
    }
}

void
AddRecordHandler (std::unique_ptr<RecordHandler>&& handler)
{
    if (s_logHandler)
    {
        s_logHandler->AddHandler(std::move(handler));
    }
}

} // namespace rdge

