//
// Created by Sridhar N on 27/05/20.
//

#ifndef TRACING_TRACING_H
#define TRACING_TRACING_H

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#define MAX_NUM_TRACE_LEVELS 8
#define DEFAULT_TRACE_LEVEL "1"

#define TRACE0(data)                                                   \
  TRACE_INTERNAL(                                                      \
      sri::tracing::TraceSourceLocation(__FILE__, __func__, __LINE__), \
      __trace_handle(), data, sri::tracing::TraceLevel::level0)
#define TRACE1(data)                                                   \
  TRACE_INTERNAL(                                                      \
      sri::tracing::TraceSourceLocation(__FILE__, __func__, __LINE__), \
      __trace_handle(), data, sri::tracing::TraceLevel::level1)
#define TRACE2(data)                                                   \
  TRACE_INTERNAL(                                                      \
      sri::tracing::TraceSourceLocation(__FILE__, __func__, __LINE__), \
      __trace_handle(), data, sri::tracing::TraceLevel::level2)
#define TRACE3(data)                                                   \
  TRACE_INTERNAL(                                                      \
      sri::tracing::TraceSourceLocation(__FILE__, __func__, __LINE__), \
      __trace_handle(), data, sri::tracing::TraceLevel::level3)
#define TRACE4(data)                                                   \
  TRACE_INTERNAL(                                                      \
      sri::tracing::TraceSourceLocation(__FILE__, __func__, __LINE__), \
      __trace_handle(), data, sri::tracing::TraceLevel::level4)
#define TRACE5(data)                                                   \
  TRACE_INTERNAL(                                                      \
      sri::tracing::TraceSourceLocation(__FILE__, __func__, __LINE__), \
      __trace_handle(), data, sri::tracing::TraceLevel::level5)
#define TRACE6(data)                                                   \
  TRACE_INTERNAL(                                                      \
      sri::tracing::TraceSourceLocation(__FILE__, __func__, __LINE__), \
      __trace_handle(), data, sri::tracing::TraceLevel::level6)
#define TRACE7(data)                                                   \
  TRACE_INTERNAL(                                                      \
      sri::tracing::TraceSourceLocation(__FILE__, __func__, __LINE__), \
      __trace_handle(), data, sri::tracing::TraceLevel::level7)

#define TRACE_CONTEXT(name)                                            \
  static sri::tracing::TraceContextHandle &__trace_handle() noexcept;  \
  static sri::tracing::TraceContextHandle &__trace_handle() noexcept { \
    static sri::tracing::TraceContextHandle handle(name);              \
    return handle;                                                     \
  }

#define TRACE_FILTER(filters)                             \
  {                                                       \
    sri::tracing::TraceFilterManager::filtersIs(filters); \
    sri::tracing::TraceContextManager::reconcile();       \
  }

#define TRACE_WRITERS(...) \
  sri::tracing::TraceWriterManager::writersIs(__VA_ARGS__)

#define TRACE_FORMAT(global_format) \
  sri::tracing::TraceFormat::globalFormatIs(global_format)

#define TRACE_INTERNAL(loc, handle, data, level)                           \
  do {                                                                     \
    sri::tracing::TraceContextHelperStorage storage;                       \
    auto trace_helper =                                                    \
        sri::tracing::TraceContextHelper::create(handle, level, &storage); \
    if (trace_helper != nullptr) {                                         \
      sri::tracing::TraceContextHelperDestroy destroy(trace_helper);       \
      trace_helper->trace_stream() << data;                                \
      trace_helper->trace(std::move(loc));                                 \
    }                                                                      \
  } while (0)

namespace sri::tracing {
std::vector<std::string> split(const std::string &input,
                               const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t last = 0, next;
  if (input.empty()) return tokens;
  while ((next = input.find(delimiter, last)) != std::string::npos) {
    tokens.push_back(input.substr(last, next - last));
    last = next + 1;
  }
  tokens.push_back(input.substr(last));
  return tokens;
}

enum class TraceLevel : std::uint8_t {
  level0 = 0,
  level1 = 1,
  level2 = 2,
  level3 = 3,
  level4 = 4,
  level5 = 5,
  level6 = 6,
  level7 = 7,
};

typedef std::bitset<MAX_NUM_TRACE_LEVELS> TraceLevelSet;

class TraceSourceLocation {
public:
  explicit TraceSourceLocation(std::nullptr_t) : line_(0), column_(0), initialized_(false) {}

  TraceSourceLocation() : TraceSourceLocation(nullptr){};

  TraceSourceLocation(std::string file_name, std::string function_name,
                      int line, int col = 0)
      : file_name_(std::move(file_name)),
        function_name_(std::move(function_name)),
        line_(line),
        column_(col),
        initialized_(true){};

  explicit operator bool() const { return initialized_; }

  [[nodiscard]] const std::string &file_name() const { return file_name_; }

  [[nodiscard]] std::string line() const { return std::to_string(line_); }

  [[nodiscard]] const std::string &function_name() const { return function_name_; }

  [[maybe_unused, nodiscard]] std::string column() const { return std::to_string(column_); }

private:
  std::string file_name_;
  std::string function_name_;
  unsigned line_;
  unsigned column_;
  bool initialized_;
};

class TraceTimestamp {
public:
  using system_clock_time_point =
      std::chrono::time_point<std::chrono::system_clock>;

  explicit TraceTimestamp(std::nullptr_t) : initialized_(false) {}

  TraceTimestamp() : TraceTimestamp(nullptr) {}

  explicit TraceTimestamp(system_clock_time_point time_point)
      : time_point_(time_point), initialized_(true) {
    char buffer[40];
    std::tm now_tm =
        localtime_r(std::chrono::system_clock::to_time_t(time_point));
    strftime(buffer, sizeof buffer, "%Y-%m-%d", &now_tm);
    date_ = buffer;
    strftime(buffer, sizeof buffer, "%H-%M-%S", &now_tm);
    time_ = buffer;
    auto ms_part =
        std::chrono::time_point_cast<std::chrono::milliseconds>(time_point)
            .time_since_epoch()
            .count() %
        1000;
    snprintf(buffer, 4, "%03lld", ms_part);
    millis_ = buffer;
  }

  explicit operator bool() const { return initialized_; }

  [[nodiscard]] const std::string &date() const { return date_; }

  [[nodiscard]] const std::string &time() const { return time_; }

  [[nodiscard]] const std::string &millis() const { return millis_; }

  std::string to_string(const std::string& delimiter) {
    std::stringstream ss;
    ss << date_ << delimiter << time_ << delimiter << millis_;
    return ss.str();
  }

private:
  std::string date_;
  std::string time_;
  std::string millis_;
  system_clock_time_point time_point_;
  bool initialized_;

  static inline std::tm localtime_r(std::time_t timep) {
    std::tm bt{};
#if defined(__unix__)
    localtime_r(&timep, &bt);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    bt = *std::localtime(&timep);
#endif
    return bt;
  }
};

class TraceContext {
public:
  explicit TraceContext(std::string name)
      : name_(std::move(name)), enabled_levels_(DEFAULT_TRACE_LEVEL) {}

  inline bool enabled(const TraceLevel &level) {
    return enabled_levels_.test(static_cast<size_t>(level));
  }

  [[nodiscard]] const std::string &name() const { return name_; }

  void enabledLevelsIs(const TraceLevelSet &enabled_levels) {
    enabled_levels_ = enabled_levels;
  }

  void enabledLevelsIs(TraceLevel level, bool value) {
    enabled_levels_.set(static_cast<size_t>(level), value);
  }

  void clearLevels() { enabled_levels_.reset(); }

  [[nodiscard]] const TraceLevelSet &enabledLevels() const { return enabled_levels_; }

private:
  std::string name_;
  TraceLevelSet enabled_levels_;
};

struct TraceMetadata {
  TraceMetadata()
      : name(""),
        level(TraceLevel::level0),
        location(nullptr),
        timestamp(nullptr) {}

  std::string name;
  TraceLevel level;
  TraceSourceLocation location;
  TraceTimestamp timestamp;
};

class TraceFormat {
public:
  static TraceFormat &instance() {
    static TraceFormat instance_;
    return instance_;
  }

  static std::string defaultFormat() {
    return std::string("#date #time.#millis #name #level #func #message");
  }

  static const std::string &globalFormat() {
    TraceFormat &tf = TraceFormat::instance();
    return tf.global_format;
  }

  static void globalFormatIs(const char *format) {
    globalFormatIs(std::string(format));
  }

  static void globalFormatIs(const std::string &format) {
    TraceFormat &tf = TraceFormat::instance();
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    tf.global_format = format;
  }

  std::string global_format;

private:
  TraceFormat() {
    // Read the global trace format from environment variable
    auto env = getenv("TRACE_FORMAT");
    if (env && env[0]) {
      global_format = std::move(std::string(env));
    } else {
      global_format = "";
    }
  }
};

class TraceWriter {
public:
  TraceWriter(std::string format, bool color_supported)
      : writer_trace_format_(std::move(format)), color_supported_(color_supported) {}

  virtual ~TraceWriter() = default;

  virtual void write(const TraceMetadata &metadata,
                     const std::string &message) = 0;

  virtual void write_to_stream(std::ostream &stream,
                               const TraceMetadata &metadata,
                               const std::string &message) const {
    std::string result = TraceFormat::globalFormat();
    if (result.empty()) {
      result = writer_trace_format_;
    }
    size_t pos = result.find("#date");
    if (pos != std::string::npos)
      result.replace(pos, 5, metadata.timestamp.date());

    pos = result.find("#time");
    if (pos != std::string::npos)
      result.replace(pos, 5, metadata.timestamp.time());

    pos = result.find("#millis");
    if (pos != std::string::npos)
      result.replace(pos, 7, metadata.timestamp.millis());

    pos = result.find("#file");
    if (pos != std::string::npos)
      result.replace(pos, 5, metadata.location.file_name());

    pos = result.find("#line");
    if (pos != std::string::npos)
      result.replace(pos, 5, metadata.location.line());

    pos = result.find("#func");
    if (pos != std::string::npos)
      result.replace(pos, 5, metadata.location.function_name());

    pos = result.find("#name");
    if (pos != std::string::npos) result.replace(pos, 5, metadata.name);

    pos = result.find("#level");
    if (pos != std::string::npos)
      result.replace(pos, 6, std::to_string(static_cast<int>(metadata.level)));

    pos = result.find("#message");
    if (pos != std::string::npos) {
      result.replace(pos, 8, message);
      stream << result << '\n';
    } else {
      if (result.empty() || (result.back() == ' '))
        stream << result << message << '\n';
      else
        stream << result << " " << message << '\n';
    }
  }

private:
  std::string writer_trace_format_;
  bool color_supported_;
};

class TraceCoutWriter : public TraceWriter {
public:
  explicit TraceCoutWriter(
      const std::string &format = TraceFormat::defaultFormat())
      : TraceWriter(format, true) {}

  void write(const TraceMetadata &metadata,
             const std::string &message) override {
    write_to_stream(std::cerr, metadata, message);
  }

private:
};

class TraceCerrWriter : public TraceWriter {
public:
  explicit TraceCerrWriter(
      const std::string &format = TraceFormat::defaultFormat())
      : TraceWriter(format, true) {}

  void write(const TraceMetadata &metadata,
             const std::string &message) override {
    write_to_stream(std::cerr, metadata, message);
  }

private:
};

class TraceFileWriter : public TraceWriter {
public:
  explicit TraceFileWriter(
      const std::string &filename,
      const std::string &format = TraceFormat::defaultFormat())
      : TraceWriter(format, false) {
    std::ios_base::openmode mode = std::ofstream::out;
    auto append_to_file = []() {
      auto env = getenv("TRACE_APPEND");
      if (env && env[0]) {
        std::string append(env);
        if (append == "no" || append == "false") {
          return false;
        }
      }
      return true;
    };
    if (append_to_file) {
      mode |= std::ofstream::app;
    } else {
      mode |= std::ofstream::trunc;
    }
    ofs.open(filename.c_str(), mode);
  }

  ~TraceFileWriter() override { ofs.close(); }

  void write(const TraceMetadata &metadata,
             const std::string &message) override {
    write_to_stream(ofs, metadata, message);
  }

protected:
  mutable std::ofstream ofs;
};

using TraceWriterSharedPtr = std::shared_ptr<TraceWriter>;

class TraceWriterManager {
public:
  static TraceWriterManager &instance() {
    static TraceWriterManager instance_;
    return instance_;
  }

  static void writersIs(const std::vector<TraceWriterSharedPtr> &writers) {
    TraceWriterManager &twmgr = TraceWriterManager::instance();
    twmgr.clear_writers();
    for (auto &writer : writers) {
      twmgr.add_writer(writer);
    }
  }

  void add_writer(const TraceWriterSharedPtr &writer) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    trace_writers_.push_back(writer);
  }

  [[maybe_unused]]  void del_writer(const TraceWriterSharedPtr &writer) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    trace_writers_.erase(
        std::remove(trace_writers_.begin(), trace_writers_.end(), writer),
        trace_writers_.end());
  }

  void clear_writers() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    trace_writers_.clear();
  }

  void write(const TraceMetadata &metadata, const std::string &message) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (const auto &writer : trace_writers_) {
      writer->write(metadata, message);
    }
  }

private:
  TraceWriterManager() {
    // By default trace to cout
    add_writer(std::make_shared<TraceCoutWriter>());

    // Add file trace write based on environment variable
    auto env = getenv("TRACE_FILE");
    if (env && env[0]) {
      std::string file_name(env);
      auto pos = file_name.find("#pid");
      if (pos != std::string::npos) {
        auto myid = std::this_thread::get_id();
        std::stringstream ss;
        ss << std::hex << myid;
        file_name.replace(pos, 4, ss.str());
      }
      pos = file_name.find("#time");
      if (pos != std::string::npos) {
        TraceTimestamp timestamp(std::chrono::system_clock::now());
        file_name.replace(pos, 5, timestamp.to_string("-"));
      }
      add_writer(std::make_shared<TraceFileWriter>(file_name));
    }
  }

  std::vector<TraceWriterSharedPtr> trace_writers_;
  std::recursive_mutex mutex_;
};

class TraceFilter {
public:
  explicit TraceFilter(const char *name) : TraceFilter(std::string(name)){};

  explicit TraceFilter(const std::string &name) {
    auto tokens = std::move(split(name, "/"));
    if (tokens.size() == 2) {
      name_ = std::move(tokens[0]);
      trace_levels_ = parseTraceLevels(tokens[1]);
    } else if (tokens.size() == 1) {
      name_ = std::move(tokens[0]);
      trace_levels_ = TraceLevelSet(DEFAULT_TRACE_LEVEL);
    } else {
      name_ = "";
      trace_levels_ = TraceLevelSet(DEFAULT_TRACE_LEVEL);
    }
  }

  [[maybe_unused, nodiscard]] const std::string &name() const { return name_; }

  [[nodiscard]] const TraceLevelSet &traceLevels() const { return trace_levels_; }

  [[nodiscard]] bool nameMatch(const std::string &name) const {
    auto star_pos = name_.find('*', 0);
    if (star_pos != std::string::npos) {
      auto name_fixed_part = name_.substr(0, star_pos);
      return name_fixed_part == name.substr(0, name_fixed_part.size());
    }
    return name_ == name;
  }

private:
  static TraceLevelSet parseTraceLevels(const std::string &trace_levels_regex) {
    TraceLevelSet trace_levels;
    bool range = false;
    size_t rangeStartIndex = 0;
    char last_char = '0';
    for (const auto &c : trace_levels_regex) {
      if (std::isdigit(c)) {
        size_t val = (c - '0');
        assert(val < MAX_NUM_TRACE_LEVELS && val >= 0);
        if (range) {
          for (size_t i = rangeStartIndex; i <= val; ++i) {
            trace_levels.set(i);
          }
          range = false;
        } else {
          trace_levels.set(val);
        }
      } else if (c == '-') {
        range = true;
        rangeStartIndex = last_char - '0';
      } else if (c == '*') {
        assert(!range &&
               "'*' should not be added between ranged trace levels");
        trace_levels.set();
        break;
      } else {
        assert(c &&
               "Invalid trace level character ( allowed  "
               "'-','0','1','2','3','4','5','6','7','8','*' )");
      }
      last_char = c;
    }
    return trace_levels;
  }

  std::string name_;
  TraceLevelSet trace_levels_;
};

using TraceFilterSharedPtr = std::shared_ptr<TraceFilter>;

class TraceFilterManager {
public:
  static TraceFilterManager &instance() {
    static TraceFilterManager instance_;
    return instance_;
  }

  static void filtersIs(const std::string &filters) {
    TraceFilterManager &tfm = TraceFilterManager::instance();
    auto trace_filters = split(filters, ",");
    tfm.clear_filters();
    for (const auto &f : trace_filters) {
      tfm.add_filter(std::make_shared<TraceFilter>(f));
    }
  }

  void doReadEnvironment() {
    auto env = getenv("TRACE");
    if (env && env[0]) {
      auto trace_filters = split(env, ",");
      for (const auto &f : trace_filters) {
        add_filter(std::make_shared<TraceFilter>(f));
      }
    }
  }

  [[nodiscard]] const std::vector<TraceFilterSharedPtr> &traceFilters() const {
    return trace_filters_;
  }

  void add_filter(const TraceFilterSharedPtr &filter) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    trace_filters_.push_back(filter);
  }

  [[maybe_unused]] void del_filter(const TraceFilterSharedPtr &filter) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    trace_filters_.erase(
        std::remove(trace_filters_.begin(), trace_filters_.end(), filter),
        trace_filters_.end());
  }

  void clear_filters() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    trace_filters_.clear();
  }

private:
  TraceFilterManager() { doReadEnvironment(); }

  std::vector<TraceFilterSharedPtr> trace_filters_;
  std::recursive_mutex mutex_;
};

class TraceContextManager {
public:
  static TraceContextManager &instance() {
    static TraceContextManager instance_;
    return instance_;
  }

  static void reconcile() {
    TraceFilterManager &tfm = TraceFilterManager::instance();
    TraceContextManager &tcm = TraceContextManager::instance();
    for (const auto &[name, trace_context] : tcm.traceContexts()) {
      trace_context->clearLevels();
      for (auto &filter : tfm.traceFilters()) {
        if (filter->nameMatch(name)) {
          trace_context->enabledLevelsIs(trace_context->enabledLevels() |
                                         filter->traceLevels());
        }
      }
      if (trace_context->enabledLevels().none()) {
        trace_context->enabledLevelsIs(TraceLevel::level0, true);
      }
    }
  }

  [[maybe_unused]] TraceContext *traceContext(const std::string &name) {
    auto iter = trace_contexts_.find(name);
    if (iter != trace_contexts_.end()) {
      return iter->second;
    }
    return nullptr;
  }

  TraceContext *traceContextIs(const std::string &name) {
    if (trace_contexts_.find(name) == trace_contexts_.end()) {
      auto trace_context = new TraceContext(name);
      TraceFilterManager &tfm = TraceFilterManager::instance();
      for (const auto &filter : tfm.traceFilters()) {
        if (filter->nameMatch(name)) {
          trace_context->enabledLevelsIs(trace_context->enabledLevels() |
                                         filter->traceLevels());
        }
      }
      if (trace_context->enabledLevels().none()) {
        trace_context->enabledLevelsIs(TraceLevel::level0, true);
      }
      trace_contexts_[name] = trace_context;
    }
    return trace_contexts_[name];
  }

  [[nodiscard]] const std::unordered_map<std::string, TraceContext *> &traceContexts() const {
    return trace_contexts_;
  }

private:
  TraceContextManager() = default;

  std::unordered_map<std::string, TraceContext *> trace_contexts_;
};

class TraceContextHandle {
public:
  explicit TraceContextHandle(const std::string &name) {
    TraceContextManager &tcm = TraceContextManager::instance();
    trace_context_ = tcm.traceContextIs(name);
  }

  explicit TraceContextHandle(const char *name)
      : TraceContextHandle(std::string(name)) {}

  bool enabled(TraceLevel &level) { return trace_context_->enabled(level); }

  [[nodiscard]] TraceContext *traceContext() const { return trace_context_; }

private:
  TraceContext *trace_context_;
};

class TraceContextHelper {
public:
  static TraceContextHelper *create(TraceContextHandle &handle,
                                    TraceLevel level, void *storage) {
    if (handle.enabled(level)) {
      return new (storage) TraceContextHelper(handle, level);
    }
    return nullptr;
  }

  std::stringstream &trace_stream() { return ss_; }

  void trace(const TraceSourceLocation& location) {
    TraceWriterManager &twm = TraceWriterManager::instance();
    TraceMetadata metadata;
    metadata.name = handle_.traceContext()->name();
    metadata.location = location;
    metadata.level = level_;
    metadata.timestamp = TraceTimestamp(std::chrono::system_clock::now());
    twm.write(metadata, ss_.str());
  };

private:
  friend class TraceContextHelperDestroy;

  TraceContextHelper(TraceContextHandle &handle, TraceLevel level)
      : handle_(handle), level_(level) {}

  ~TraceContextHelper() = default;

  TraceLevel level_;
  TraceContextHandle &handle_;
  std::stringstream ss_;
};

typedef std::aligned_storage<sizeof(TraceContextHelper),
                             alignof(TraceContextHelper)>::type
    TraceContextHelperStorage;

class TraceContextHelperDestroy {
public:
  explicit TraceContextHelperDestroy(TraceContextHelper *trace_context_helper)
      : trace_context_helper_(trace_context_helper) {}

  ~TraceContextHelperDestroy() { trace_context_helper_->~TraceContextHelper(); }

private:
  TraceContextHelper *trace_context_helper_;
};


}// namespace sri::tracing

#endif// TRACING_TRACING_H
