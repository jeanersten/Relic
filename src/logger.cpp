#include "logger.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

Logger::Logger()
{
    m_message_level = LogLevel::TRACE;
}

Logger::~Logger()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}

Logger& Logger::m_getInstance()
{
    static Logger instance;
    return instance;
}

std::string_view Logger::m_getLogLevelString(LogLevel log_level)
{
    switch (log_level)
    {
        case LogLevel::TRACE: return "  TRACE  ";
        case LogLevel::DEBUG: return "  DEBUG  ";
        case LogLevel::INFO: return "   INFO  ";
        case LogLevel::WARNING: return " WARNING ";
        case LogLevel::ERROR: return "  ERROR  ";
        case LogLevel::CRITICAL: return "CRITICAL ";
        default: return " UNKNOWN ";
    }
}

std::string Logger::m_getTimeStamp()
{
    auto time_now { std::chrono::system_clock::now() };
    auto time_t_now { std::chrono::system_clock::to_time_t(time_now) };
    auto time_local { *std::localtime(&time_t_now) };

    std::ostringstream output_string { };
    output_string << std::put_time(&time_local, "%Y-%m-%d %H:%M:%S");
    return output_string.str();
}

std::string_view Logger::m_getMessageColor(LogLevel message_level)
{
    switch (message_level)
    {
        case LogLevel::TRACE: return "\033[90m"; // Light Gray
        case LogLevel::DEBUG: return "\033[34m"; // Blue
        case LogLevel::INFO: return "\033[32m"; // Green
        case LogLevel::WARNING: return "\033[33m"; // Yellow
        case LogLevel::ERROR: return "\033[31m"; // Red
        case LogLevel::CRITICAL: return "\033[35m"; // Magenta
        default: return "\033[30m";
    }
}

std::string_view Logger::m_getResetColor()
{
    return "\033[0m";
}

std::string Logger::m_formatMessage(const std::string& message)
{
    return message;
}

bool Logger::m_isFileOutputEnabled()
{
    if (!m_file_path.empty())
    {
        m_file.open(m_file_path, std::ios::app);
        if (!m_file.is_open())
        {
            throw std::ios_base::failure("Failed to open log file: " + m_file_path);
        }

        return true;
    }

    return false;
}

void Logger::m_logMessage(LogLevel message_level, const std::string& message)
{
    if (m_message_level <= message_level)
    {
        auto timestamp { m_getTimeStamp() };
        std::scoped_lock lock { m_log_mutex };

        std::ostringstream log_entry { };
        log_entry << timestamp << m_getMessageColor(message_level) << " [" <<
        m_getLogLevelString(message_level) << "]: " << message << 
        m_getResetColor() << '\n';

        std::cout << log_entry.str();

        if (m_file.is_open())
        {
            m_file << log_entry.str();
        }
        
        if (message_level == LogLevel::CRITICAL)
        {
            exit(-1);
        }
    }
}

void Logger::setLevel(LogLevel value)
{
    m_getInstance().m_message_level = value;
}

void Logger::enableFileOutput(const std::string& file_path)
{
    Logger& instance { m_getInstance() };
    instance.m_file_path = file_path;
    instance.m_isFileOutputEnabled();
}
