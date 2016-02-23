#pragma once
#include "base/string_stream.h"
#include "base/exception.h"
#include <memory>
#include <string>

namespace hftbattle {

class Logger;
typedef Logger* LoggerId;

enum class LogLevel: int8_t {
  Debug,
  Info,
  Warning,
  Error,
  Fatal
};

class LogMessage {
  public:
    LogMessage(LoggerId logger, LogLevel level) :
      logger_(logger), level_(level) { }

    LogMessage(const LogMessage&) = delete;
    LogMessage& operator=(const LogMessage&) = delete;
    LogMessage(LogMessage&&) = default;
    LogMessage& operator=(LogMessage&&) = default;

    ~LogMessage();

    LoggerId logger() const { return logger_; }
    LogLevel level() const { return level_; }
    const char* text() { return text_.c_str(); }
    bool empty() const { return text_.empty(); }

    void reset() { text_.clear(); }

    template<typename T>
    LogMessage& operator<<(const T& value) {
      text_ << value;
      return *this;
    }

  private:
    LoggerId logger_;
    LogLevel level_;
    StringStream text_;
};

class Backend {
  public:
    Backend() = default;
    Backend(const Backend&) = delete;
    Backend& operator=(const Backend&) = delete;

    virtual void log(LogMessage* message) = 0;
    virtual void flush() = 0;
    virtual ~Backend() {}
};

class BackendHolder {
  public:
    BackendHolder(std::shared_ptr<Backend> backend)
      : backend_(std::move(backend)) {
    }

    BackendHolder(const BackendHolder&) = delete;
    BackendHolder& operator=(const BackendHolder&) = delete;

    void log(LogMessage* message) {
      backend_->log(message);
    }

    void flush() {
      backend_->flush();
    }

  private:
    std::shared_ptr<Backend> backend_;
};

class Logger {
  public:
    explicit Logger(const std::string& name);
    Logger(const std::string& name, std::shared_ptr<BackendHolder> backend);
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    const std::string& name() const { return name_; }
    LoggerId id() { return this; }
    LogLevel min_level() const { return min_level_; }
    void set_min_level(LogLevel min_level) { min_level_ = min_level; }
    BackendHolder* backend() { return backend_.get(); }

    static LogLevel default_min_level() {
      return *default_min_level_ptr();
    }

    static void set_default_min_level(LogLevel level) {
      *default_min_level_ptr() = level;
    }

    static LoggerId anonymous();

  private:
    static LogLevel* default_min_level_ptr() {
      static LogLevel default_min_level = LogLevel::Debug;
      return &default_min_level;
    }

    std::string name_;
    LogLevel min_level_;
    std::shared_ptr<BackendHolder> backend_;
};

class LogStreamHolder {
  public:
};

LoggerId GetLoggerId(const std::string& name);
void ChangeAllLoggersLevel(const std::string& level_str);

struct SourceLocation {
    SourceLocation(const char* filename, int line_number, const char* function_name)
      : filename(filename), line_number(line_number), function_name(function_name) {}
    const std::string filename;
    const int line_number;
    const std::string function_name;
};

template<typename T>
StringStreamBase<T>& operator<<(StringStreamBase<T>& stream, const SourceLocation& source_location) {
  return stream << "@ " << source_location.filename <<
    " :" << source_location.line_number <<
    ":" << source_location.function_name << ":";
}

inline LoggerId getCurrentLoggerId() {
  static const LoggerId kCurrentLoggerId = Logger::anonymous();
  return kCurrentLoggerId;
}

class LoggableComponent {
  public:
    explicit LoggableComponent(const std::string& name);
    ~LoggableComponent();
    const std::string& logger_name() const { return getCurrentLoggerId()->name(); }

  protected:
    inline LoggerId getCurrentLoggerId() const {
      return kCurrentLoggerId;
    }

    const LoggerId kCurrentLoggerId;
};

std::shared_ptr<BackendHolder> GetDefaultBackend();
void ForwardLogsToFile(const std::string& filename);

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define VARNAME(name) CONCATENATE(name, __LINE__)

#define PRIVATE_LOG(logger, level) \
  for (bool _once = true; _once && level >= logger->min_level(); _once = false) \
    LogMessage(logger, level)

#define PRIVATE_LOG_IF(logger, level, condition) \
  for (bool _once = true; _once && level >= logger->min_level() && (condition); _once = false) \
    LogMessage(logger, level)

#define PRIVATE_LOG_IF_CHANGABLE(logger, level_true, level_false, condition) \
  for (struct { bool _once; LogLevel _level; } _data = {true, (condition) ? level_true : level_false}; \
      _data._once && _data._level >= logger->min_level(); \
      _data._once = false) \
    LogMessage(logger, _data._level)

#define PRIVATE_LOG_R(logger, level, statement, ok) \
  decltype(statement) VARNAME(__return_code__); \
  PRIVATE_LOG_IF(logger, level, ((ok) != (VARNAME(__return_code__) = (statement)))) << \
    "Invalid return code: " << VARNAME(__return_code__) << \
    " expected: " << (ok) << " for '" << (#statement) << " "

#define FATAL() throw Exception() << \
  SourceLocation(__FILE__, __LINE__, __FUNCTION__) << \
  "[" << getCurrentLoggerId()->name() << "] "

#define CHECK(condition) \
  for (bool _once = true; _once && !(condition); _once = false) \
    FATAL() << "Check failed: '" << (#condition) << "' "

#define CHECK_R(statement, ok) \
  decltype(statement) VARNAME(__return_code__); \
  for (bool _once = true; _once && ((ok) != (VARNAME(__return_code__) = (statement))); _once = false) \
    FATAL() << "Invalid return code: " << VARNAME(__return_code__) << \
    " expected: " << (ok) << " for '" << (#statement) << " "

#define PRIVATE_ALOG(level) PRIVATE_LOG(getCurrentLoggerId(), level)
#define PRIVATE_ALOG_IF(level, condition) PRIVATE_LOG_IF(getCurrentLoggerId(), level, (condition))

#define DEBUG() PRIVATE_LOG(getCurrentLoggerId(), LogLevel::Debug)
#define INFO() PRIVATE_LOG(getCurrentLoggerId(), LogLevel::Info)
#define WARNING() PRIVATE_LOG(getCurrentLoggerId(), LogLevel::Warning)
#define ERROR() PRIVATE_LOG(getCurrentLoggerId(), LogLevel::Error)

#define DEBUG_IF(condition) PRIVATE_LOG_IF(getCurrentLoggerId(), LogLevel::Debug, condition)
#define INFO_IF(condition) PRIVATE_LOG_IF(getCurrentLoggerId(), LogLevel::Info, condition)
#define WARNING_IF(condition) PRIVATE_LOG_IF(getCurrentLoggerId(), LogLevel::Warning, condition)
#define ERROR_IF(condition) PRIVATE_LOG_IF(getCurrentLoggerId(), LogLevel::Error, condition)

#define DEBUG_R(statement, ok) PRIVATE_LOG_R(getCurrentLoggerId(), LogLevel::Debug, (statement), ok)
#define INFO_R(statement, ok) PRIVATE_LOG_R(getCurrentLoggerId(), LogLevel::Info, (statement), ok)
#define WARNING_R(statement, ok) PRIVATE_LOG_R(getCurrentLoggerId(), LogLevel::Warning, (statement), ok)
#define ERROR_R(statement, ok) PRIVATE_LOG_R(getCurrentLoggerId(), LogLevel::Error, (statement), ok)

#define WARNING_IF_INFO(condition) PRIVATE_LOG_IF_CHANGABLE(getCurrentLoggerId(), LogLevel::Warning, LogLevel::Info, condition)
#define ERROR_IF_INFO(condition) PRIVATE_LOG_IF_CHANGABLE(getCurrentLoggerId(), LogLevel::Error, LogLevel::Info, condition)
#define ERROR_IF_WARNING(condition) PRIVATE_LOG_IF_CHANGABLE(getCurrentLoggerId(), LogLevel::Error, LogLevel::Warning, condition)

}  // namespace hftbattle
