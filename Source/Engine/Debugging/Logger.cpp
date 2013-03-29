#include "EngineStd.h"
#include "Logger.h"
#include "../Multicore/CriticalSection.h"
#include "../TinyXML/tinyxml2.h"

using std::string;


#pragma region Constants, Statics, and Globals

static const char* ERRORLOG_FILENAME = "error.log";

//default display flags
#ifdef _DEBUG
  const unsigned char ERRORFLAG_DEFAULT = LOGFLAG_WRITE_TO_DEBUGGER;
  const unsigned char WARNINGFLAG_DEFAULT = LOGFLAG_WRITE_TO_DEBUGGER;
  const unsigned char LOGFLAG_DEFAULT = LOGFLAG_WRITE_TO_DEBUGGER;
#elif NDEBUG
  const unsigned char ERRORFLAG_DEFAULT = (LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);
  const unsigned char WARNINGFLAG_DEFAULT = (LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);
  const unsigned char LOGFLAG_DEFAULT = (LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);
#else
  const unsigned char ERRORFLAG_DEFAULT = 0
  const unsigned char WARNINGFLAG_DEFAULT = 0;
  const unsigned char LOGFLAG_DEFAULT = 0;
#endif

class LogMgr;
static LogMgr* g_log_mgr;
#pragma endregion

#pragma region LogMgr Class
/////////////////////////////////////////////////////////////////////////
//LogMgr
/////////////////////////////////////////////////////////////////////////
class LogMgr
{
public:
  enum ErrorDialogResult
  {
    LOGMGR_ERROR_ABORT,
    LOGMGR_ERROR_RETRY,
    LOGMGR_ERROR_IGNORE
  };

  typedef std::map<string, unsigned char> Tags;
  typedef std::list<Logger::ErrorMessenger*> ErrorMessengerList;

  Tags _tags;
  ErrorMessengerList _error_messengers;

  //thread safety
  CriticalSection _tag_critical_section;
  CriticalSection _messenger_critical_section;

public:
  LogMgr();
  ~LogMgr();
  void Init(const char* logging_config_filename);

  //logs
  void Log(const string& tag, const string& message, const char* func, const char* source, unsigned int line);
  void SetDisplayFlags(const std::string& tag, unsigned char flags);

  //error messengers
  void AddErrorMessenger(Logger::ErrorMessenger* messenger);
  LogMgr::ErrorDialogResult Error(const std::string& error_message, bool is_fatal, const char* func, const char* source, unsigned int line);

private:
  //log helpers
  void OutputFinalBufferToLogs(const string& final_buffer, unsigned char flags);
  void WriteToLogFile(const string& data);
  void GetOutputBuffer(std::string& out_output_buffer, const string& tag, const string& message, const char* func, const char* source, unsigned int line);

};
#pragma endregion

#pragma region LogMgr class definition

LogMgr::LogMgr()
{
  //setup default log tagas
  SetDisplayFlags("ERROR", ERRORFLAG_DEFAULT);
  SetDisplayFlags("WARNING", WARNINGFLAG_DEFAULT);
  SetDisplayFlags("INFO", LOGFLAG_DEFAULT);
}

LogMgr::~LogMgr()
{
  _messenger_critical_section.Lock();
  for(auto it = _error_messengers.begin(); it != _error_messengers.end(); ++it)
  {
    Logger::ErrorMessenger* messenger = (*it);
    delete messenger;
  }
  _error_messengers.clear();
  _messenger_critical_section.Unlock();
}

void LogMgr::Init(const char* logging_config_filename)
{
  if(logging_config_filename)
  {
    tinyxml2::XMLDocument log_file;
    if(log_file.LoadFile(logging_config_filename) == 0)
    {
      tinyxml2::XMLNode*  root = log_file.FirstChild();
      if(!root)
        return;
      //loop through each child element and load component
      for(tinyxml2::XMLElement* node = root->FirstChildElement(); node; node = node->NextSiblingElement())
      {
        unsigned char flags = 0;
        std::string tag(node->Attribute("tag"));

        int debugger = node->IntAttribute("debugger");
        if(debugger)
          flags |= LOGFLAG_WRITE_TO_DEBUGGER;

        int logfile = node->IntAttribute("file");
        if(logfile)
          flags |= LOGFLAG_WRITE_TO_LOG_FILE;

        SetDisplayFlags(tag, flags);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// this function builds up the log string and outputs it to various places based on display flags
///////////////////////////////////////////////////////////////////////////////////////////////////////
void LogMgr::Log(const string& tag, const string& message, const char* func, const char* source, unsigned int line)
{
  _tag_critical_section.Lock();
  Tags::iterator it = _tags.find(tag);
  if(it != _tags.end())
  {
    _tag_critical_section.Unlock();
    string buffer;
    GetOutputBuffer(buffer, tag, message, func, source, line);
    OutputFinalBufferToLogs(buffer, it->second);
  }
  else
  {
    //critical section is exited in the if above, so need to do it here if above wasnt executed
    _tag_critical_section.Unlock();
  }
}

///////////////////////////////////////////////////////////////////////////////////////
// sets one or more display flags
///////////////////////////////////////////////////////////////////////////////////////
void LogMgr::SetDisplayFlags(const std::string& tag, unsigned char flags)
{
  _tag_critical_section.Lock();
  if(flags != 0)
  {
    Tags::iterator it = _tags.find(tag);
    if(it == _tags.end())
      _tags.insert(std::make_pair(tag, flags));
    else
      it->second = flags;
  }
  else
  {
    _tags.erase(tag);
  }
  _tag_critical_section.Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// adds error message to list
////////////////////////////////////////////////////////////////////////////////////////////////
void LogMgr::AddErrorMessenger(Logger::ErrorMessenger* messenger)
{
  _messenger_critical_section.Lock();
  _error_messengers.push_back(messenger);
  _messenger_critical_section.Unlock();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//helper for ErrorMessenger
/////////////////////////////////////////////////////////////////////////////////////////////
LogMgr::ErrorDialogResult LogMgr::Error(const std::string& error_message, bool is_fatal, const char* func, const char* source, unsigned int line)
{
  string tag = ((is_fatal) ? ("FATAL") : ("ERROR"));
  //buffer for final output string
  string buffer;
  GetOutputBuffer(buffer, tag, error_message, func, source, line);

  //write final buffer to various logs
  _tag_critical_section.Lock();
  Tags::iterator it = _tags.find(tag);
  if(it != _tags.end())
    OutputFinalBufferToLogs(buffer, it->second);
  _tag_critical_section.Unlock();

  //show the dialog box
  int result = ::MessageBoxA(NULL, buffer.c_str(), tag.c_str(),  MB_ABORTRETRYIGNORE|MB_ICONERROR|MB_DEFBUTTON3);
  switch(result)
  {
    case IDIGNORE:  return LogMgr::LOGMGR_ERROR_IGNORE;
    case IDABORT: __debugbreak(); return LogMgr::LOGMGR_ERROR_ABORT;
    case IDRETRY: return LogMgr::LOGMGR_ERROR_RETRY;
    default:  return LogMgr::LOGMGR_ERROR_RETRY;
  }
}

//------------------------------------------------------------------------------------------------------------------------------------
// This is a helper function that checks all the display flags and outputs the passed in finalBuffer to the appropriate places.
// 
// IMPORTANT: The two places this function is called from wrap the code in the tag critical section (m_pTagCriticalSection), 
// so that makes this call thread safe.  If you call this from anywhere else, make sure you wrap it in that critical section.
//------------------------------------------------------------------------------------------------------------------------------------
void LogMgr::OutputFinalBufferToLogs(const string& final_buffer, unsigned char flags)
{
  if((flags & LOGFLAG_WRITE_TO_LOG_FILE) > 0)
    WriteToLogFile(final_buffer);
  if((flags & LOGFLAG_WRITE_TO_DEBUGGER) > 0)
    ::OutputDebugStringA(final_buffer.c_str());
}

/////////////////////////////////////////////////////////////////////////////
//helper function that writes data string to log file
/////////////////////////////////////////////////////////////////////////////
void LogMgr::WriteToLogFile(const string& data)
{
  FILE* log_file = 0;
  fopen_s(&log_file, ERRORLOG_FILENAME, "a+");
  if(!log_file)
    return;
  fprintf_s(log_file, data.c_str());
  fclose(log_file);
}

/////////////////////////////////////////////////////////////////////////////
//fills outoutputbuffer with the find error string
/////////////////////////////////////////////////////////////////////////////
void LogMgr::GetOutputBuffer(std::string& out_output_buffer, const string& tag, const string& message, const char* func, const char* source, unsigned int line)
{
  if(!tag.empty())
    out_output_buffer = "[" + tag + "]" + message;
  else
    out_output_buffer = message;

  if(func != NULL)
  {
    out_output_buffer += "\nFunction: ";
    out_output_buffer += func;
  }
  if(source != NULL)
  {
    out_output_buffer += "\n";
    out_output_buffer += source;
  }
  if(line != 0)
  {
    out_output_buffer += "\nLine: ";
    char line_buf[11];
    memset(line_buf, 0, sizeof(char));
    out_output_buffer += _itoa_s(line, line_buf, 10);
  }
  out_output_buffer += "\n";
}

#pragma endregion

#pragma region ErrorMessenger definition

Logger::ErrorMessenger::ErrorMessenger()
{
  g_log_mgr->AddErrorMessenger(this);
  _enabled = true;
}

void Logger::ErrorMessenger::Show(const std::string& error_message, bool is_fatal, const char* func, const char* source, unsigned int line)
{
  if(_enabled)
  {
    if(g_log_mgr->Error(error_message, is_fatal, func, source, line) == LogMgr::LOGMGR_ERROR_IGNORE)
      _enabled = false;
  }
}

#pragma endregion

#pragma region C Interface

namespace Logger
{
  void Init(const char* log_config_filename)
  {
    if(!g_log_mgr)
    {
      g_log_mgr = SOL_NEW LogMgr;
      g_log_mgr->Init(log_config_filename);
    }
  }

  void Destroy()
  {
    delete g_log_mgr;
    g_log_mgr = 0;
  }

  void Log(const string& tag, const string& message, const char* func, const char* source, unsigned int line)
  {
    SOL_ASSERT(g_log_mgr);
    g_log_mgr->Log(tag, message, func, source, line);
  }

  void SetDisplayFlags(const string& tag, unsigned char flags)
  {
    SOL_ASSERT(g_log_mgr);
    g_log_mgr->SetDisplayFlags(tag, flags);
  }
}

#pragma endregion
