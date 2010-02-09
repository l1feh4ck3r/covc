#ifndef COLORER_H
#define COLORER_H

#include <QThread>

class Colorer : public QThread
{
public:
    Colorer();


public:
    int     build_source();
    bool    is_opencl_available();
    int     load_from_file(QString filename);
    virtual void run();
};

#endif // COLORER_H
