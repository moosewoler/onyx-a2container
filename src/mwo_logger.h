#ifndef _MWO_LOGGER_H_
#define _MWO_LOGGER_H_
#include <QtCore>

class MwoLogger 
{
    public:
        MwoLogger(const QString & filename);
        ~MwoLogger(void);
    public:
        void log(const QString & message);
        //MwoLogger & operator << (const QString & message);
    private:
        QFile       file_;
        QTextStream out_;
        int         loglevel_;
};

#ifndef MWO_LOGGER_IMPLEMENTATION
extern MwoLogger logger;
#endif

#endif
