#include "log.h"

FILE* LOGFILE;

void log_init(FILE* file)
{
    LOGFILE = file;
}

void log_init(std::string fname)
{
    LOGFILE = fopen(fname.c_str(),"w");
}

void log(LogLevel lvl, std::string message)
{
    fprintf(
            LOGFILE,
            "[%s] %s\n",
            (lvl == LOG_DEBUG)   ? "DEBUG"   :
            (lvl == LOG_WARNING) ? "WARNING" :
                               "ERROR",
            message.c_str()
    );
}

void log_free()
{
    if(LOGFILE == stdout || LOGFILE == stderr);
    else fclose(LOGFILE);
}
