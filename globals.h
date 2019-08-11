#ifndef GLOBALS_H
#define GLOBALS_H

#include <QSize>
#include <QImage>

extern int position = 0;
extern int maxImages = 0;
extern int ssDelay = 3;
//extern bool direction = true; //true = forward.
struct myGlobals
{
    float maxw = 1920.0;
    float maxh = 1080.0;
    QString dirPath = "/home";
    bool more = false;
    QString arg = "";
    int unread = 0;
    int skipFactor = 0;
    int ssDelay = 3;
//    bool okNext = false;
} glob;

#endif // GLOBALS_H
