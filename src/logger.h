#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>

class Logger
{
public:
    enum class LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

private:
    LogLevel m_message_level { };
    std::ofstream m_file { };
    std::string m_file_path { };
    std::mutex m_log_mutex { };

    Logger();
    Logger(const Logger&) = delete;
    ~Logger();

    Logger& operator=(const Logger&) = delete;

    static Logger& m_getInstance();
    std::string_view m_getLogLevelString(LogLevel log_level);
    std::string m_getTimeStamp();
    std::string_view m_getMessageColor(LogLevel message_level);
    std::string_view m_getResetColor();

    static std::string m_formatMessage(const std::string& message);
    template <typename T, typename... Args>
    static std::string m_formatMessage(const std::string&, T&& value, Args&&... arguments);
    bool m_isFileOutputEnabled();

    void m_logMessage(LogLevel message_level, const std::string& message);

public:
    static void setLevel(LogLevel value);

    static void enableFileOutput(const std::string& file_path = "log.txt");

    template <typename... Args>
    static void logDefaultMessage(LogLevel message_level, const std::string& message, Args&&... arguments);
    template <typename... Args>
    static void logContextMessage(LogLevel message_level, const std::string& file_name, int line_number,
                                  const std::string& message, Args&&... arguments);
};

template <typename T, typename... Args>
std::string Logger::m_formatMessage(const std::string& message, T&& value, Args&&... arguments)
{
    std::ostringstream output_string { };
    size_t placeholder_pos = message.find("{}");
    if (placeholder_pos != std::string::npos)
    {
        output_string << message.substr(0, placeholder_pos) << std::forward<T>(value) <<
                         m_formatMessage(message.substr(placeholder_pos + 2), std::forward<Args>(arguments)...);
    }
    else
    {
        output_string << message;
    }
    return output_string.str();
}

template <typename... Args>
void Logger::logDefaultMessage(LogLevel message_level, const std::string& message, Args&&... arguments)
{
    std::ostringstream formatted_message { };
    formatted_message << m_formatMessage(message, std::forward<Args>(arguments)...);
    m_getInstance().m_logMessage(message_level, formatted_message.str());
}

template <typename... Args>
void Logger::logContextMessage(LogLevel message_level, const std::string& file_name, int line_number,
                               const std::string& message, Args&&... arguments)
{
    std::ostringstream formatted_message { };
    formatted_message << m_formatMessage(message, std::forward<Args>(arguments)...) <<
                         " at (" << file_name << ": " << line_number << ")-";
    m_getInstance().m_logMessage(message_level, formatted_message.str());
}

#ifdef DEBUG_BUILD
#define LOG_TRACE(message, ...) Logger::logDefaultMessage(Logger::LogLevel::TRACE, message, ##__VA_ARGS__)
#define LOG_DEBUG(message, ...) Logger::logDefaultMessage(Logger::LogLevel::DEBUG, message, ##__VA_ARGS__)
#define LOG_INFO(message, ...) Logger::logDefaultMessage(Logger::LogLevel::INFO, message, ##__VA_ARGS__)
#define LOG_WARNING(message, ...) Logger::logDefaultMessage(Logger::LogLevel::WARNING, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) Logger::logDefaultMessage(Logger::LogLevel::ERROR, message, ##__VA_ARGS__)
#define LOG_CRITICAL(message, ...) Logger::logDefaultMessage(Logger::LogLevel::CRITICAL, message, ##__VA_ARGS__)
#endif

#ifdef RELEASE_BUILD
#define LOG_TRACE(message, ...) // Removed for release
#define LOG_DEBUG(message, ...) // Removed for release
#define LOG_INFO(message, ...) Logger::logContextMessage(Logger::LogLevel::INFO, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define LOG_WARNING(message, ...) Logger::logContextMessage(Logger::LogLevel::WARNING, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) Logger::logContextMessage(Logger::LogLevel::ERROR, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define LOG_CRITICAL(message, ...) Logger::logContextMessage(Logger::LogLevel::CRITICAL, __FILE__, __LINE__, message, ##__VA_ARGS__)
#endif


#endif
