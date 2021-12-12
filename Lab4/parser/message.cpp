#include <stdio.h>
#include <stdarg.h>
#include <cstdlib>
#include <unistd.h>
#include <assert.h>
#include "message.h" 
using std::string;

enum WarningLevel_E{ warning=0, infomation=1, debug=2} ;
const WarningLevel_E MSGLevel=infomation;

//-------------------------------------------------------------------------------------------------
void messageWithLevel ( WarningLevel_E p_nLevel, 
        va_list vptr,
        const char *format)
{
    if((int)MSGLevel>=(int)p_nLevel)
    {
        if(debug==p_nLevel)
        {
            printf("debug: ");
        }
        else if(warning==p_nLevel)
        {
            printf("warning: ");
        }
        else if(infomation==p_nLevel)
        {
            printf("info: ");
        }
        else
        {
            ;
        }
        vprintf(format,   vptr);  
    }
    else
    {
        ;//do nothing
    }
}
//-------------------------------------------------------------------------------------------------
void find_and_replace( string &source, const string find, string replace ) 
{
    size_t j;
    for ( ; (j = source.find( find )) != string::npos ; ) 
    {
        source.replace( j, find.length(), replace );
    }
}
//-------------------------------------------------------------------------------------------------
void InfoMsg (const char * format,... )
{
    va_list   vptr;
    va_start(vptr,   format);
    messageWithLevel(infomation,vptr,format);
    va_end(vptr);
}
//-------------------------------------------------------------------------------------------------
void WarnMsg (const char * format,... )
{
    va_list   vptr;  
    va_start(vptr,   format);  
    messageWithLevel(warning,vptr,format);   
    va_end(vptr); 
}
//-------------------------------------------------------------------------------------------------
#if RD_DEBUG!=1
void DebugMsg (const char * format,... )
{
    //-------------Internal use--------------------
    va_list   vptr;
    va_start(vptr,   format);
    messageWithLevel(debug,vptr,format);
    va_end(vptr);
}
#else
void DebugMsg (const char * format,... ){}
#endif
//-------------------------------------------------------------------------------------------------
Dumper::Dumper(const char * p_pFileName,const char * mode)
{
    m_pFile=fopen(p_pFileName,mode);
    if(!m_pFile)
    {
        InfoMsg("Open File: %s Fail\n",p_pFileName);
    }
}
//-------------------------------------------------------------------------------------------------
Dumper::~Dumper()
{
    if(m_pFile)
    {
        fflush((FILE*)m_pFile);
        fsync(fileno((FILE*)m_pFile));
        fclose((FILE*)m_pFile);
        m_pFile=NULL;
    }
}
//-------------------------------------------------------------------------------------------------
void Dumper::write( const char * format, ... )
{
    assert(m_pFile);
    va_list   vptr;
    va_start(vptr,   format);
    vfprintf((FILE*)m_pFile,format,   vptr);
    va_end(vptr);
}
//-------------------------------------------------------------------------------------------------


