//
// Copyright (c) 2008-2017 the Unique project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Core/Object.h"
#include "../Core/StringUtil.h"
#include <mutex>

namespace Unique
{

/// Fictional message level to indicate a stored raw message.
static const int LOG_RAW = -1;
/// Debug message level. By default only shown in debug mode.
static const int LOG_DEBUG = 0;
/// Informative message level.
static const int LOG_INFO = 1;
/// Warning message level.
static const int LOG_WARNING = 2;
/// Error message level.
static const int LOG_ERROR = 3;
/// Disable all log messages.
static const int LOG_NONE = 4;

class File;

/// Stored log message from another thread.
struct StoredLogMessage
{
    /// Construct undefined.
    StoredLogMessage()
    {
    }

    /// Construct with parameters.
    StoredLogMessage(const String& message, int level, bool error) :
        message_(message),
        level_(level),
        error_(error)
    {
    }

    /// Message text.
    String message_;
    /// Message level. -1 for raw messages.
    int level_;
    /// Error flag for raw messages.
    bool error_;
};

/// Logging subsystem.
class UNIQUE_API Log : public Object
{
    uRTTI(Log, Object)

public:
    /// Construct.
    Log();
    /// Destruct. Close the log file if open.
    virtual ~Log();

    /// Open the log file.
    void Open(const String& fileName);
    /// Close the log file.
    void Close();
    /// Set logging level.
    void SetLevel(int level);
    /// Set whether to timestamp log messages.
    void SetTimeStamp(bool enable);
    /// Set quiet mode ie. only print error entries to standard error stream (which is normally redirected to console also). Output to log file is not affected by this mode.
    void SetQuiet(bool quiet);

    /// Return logging level.
    int GetLevel() const { return level_; }

    /// Return whether log messages are timestamped.
    bool GetTimeStamp() const { return timeStamp_; }

    /// Return last log message.
    String GetLastMessage() const { return lastMessage_; }

    /// Return whether log is in quiet mode (only errors printed to standard error stream).
    bool IsQuiet() const { return quiet_; }

    /// Write to the log. If logging level is higher than the level of the message, the message is ignored.
    static void Write(int level, const String& message);
    /// Write raw output to the log.
    static void WriteRaw(const String& message, bool error = false);

private:
    /// Handle end of frame. Process the threaded log messages.
    void HandleEndFrame(StringID eventType, const struct EndFrame& eventData);

    /// Mutex for threaded operation.
    std::mutex logMutex_;
    /// Log messages from other threads.
    List<StoredLogMessage> threadMessages_;
    /// Log file.
    SPtr<File> logFile_;
    /// Last log message.
    String lastMessage_;
    /// Logging level.
    int level_;
    /// Timestamp log messages flag.
    bool timeStamp_;
    /// In write flag to prevent recursion.
    bool inWrite_;
    /// Quiet mode flag.
    bool quiet_;
};

#ifdef UNIQUE_LOGGING
#define UNIQUE_LOGDEBUG(message) Unique::Log::Write(Unique::LOG_DEBUG, message)
#define UNIQUE_LOGINFO(message) Unique::Log::Write(Unique::LOG_INFO, message)
#define UNIQUE_LOGWARNING(message) Unique::Log::Write(Unique::LOG_WARNING, message)
#define UNIQUE_LOGERROR(message) Unique::Log::Write(Unique::LOG_ERROR, message)
#define UNIQUE_LOGRAW(message) Unique::Log::WriteRaw(message)
#define UNIQUE_LOGDEBUGF(format, ...) Unique::Log::Write(Unique::LOG_DEBUG, Unique::FormatString(format, ##__VA_ARGS__))
#define UNIQUE_LOGINFOF(format, ...) Unique::Log::Write(Unique::LOG_INFO, Unique::FormatString(format, ##__VA_ARGS__))
#define UNIQUE_LOGWARNINGF(format, ...) Unique::Log::Write(Unique::LOG_WARNING, Unique::FormatString(format, ##__VA_ARGS__))
#define UNIQUE_LOGERRORF(format, ...) Unique::Log::Write(Unique::LOG_ERROR, Unique::FormatString(format, ##__VA_ARGS__))
#define UNIQUE_LOGRAWF(format, ...) Unique::Log::WriteRaw(Unique::FormatString(format, ##__VA_ARGS__))
#else
#define UNIQUE_LOGDEBUG(message) ((void)0)
#define UNIQUE_LOGINFO(message) ((void)0)
#define UNIQUE_LOGWARNING(message) ((void)0)
#define UNIQUE_LOGERROR(message) ((void)0)
#define UNIQUE_LOGRAW(message) ((void)0)
#define UNIQUE_LOGDEBUGF(...) ((void)0)
#define UNIQUE_LOGINFOF(...) ((void)0)
#define UNIQUE_LOGWARNINGF(...) ((void)0)
#define UNIQUE_LOGERRORF(...) ((void)0)
#define UNIQUE_LOGRAWF(...) ((void)0)
#endif

}
