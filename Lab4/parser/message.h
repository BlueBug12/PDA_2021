#ifndef __MESSAGE__H_
#define __MESSAGE__H_

#include <string>
void DebugMsg (const char * format, ... );
void WarnMsg  (const char * format, ... );
void InfoMsg  (const char * format, ... );


#ifdef __cplusplus
void find_and_replace( std::string& source, const std::string find, std::string replace ); 
class Dumper
{
    public: 
        Dumper(const char * p_pFileName,const char * mode="w");
        ~Dumper();
        void        write( const char * format, ... );
    private:
        void*       m_pFile;//FILE*
    private:// no assignment guard
        const Dumper& operator=(const Dumper&);//don't define me
        Dumper(const Dumper&);
};
#endif

#endif

