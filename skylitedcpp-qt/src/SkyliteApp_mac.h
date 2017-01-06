/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#pragma once

#include <QApplication>
#include <QEvent>
#include <QTimer>
#include <QSessionManager>
#include "WulforSettings.h"
#include "MainWindow.h"
#include "qtsingleapp/qtsinglecoreapplication.h"
#include <objc/objc.h>
#include <objc/message.h>

bool dockClickHandler(id self,SEL _cmd,...);

class SkyliteEventFilter: public QObject{
Q_OBJECT
public:
    SkyliteEventFilter(): counter(0), has_activity(true) {
        timer.setInterval(60000);

        connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

        timer.start();
    }

    virtual ~SkyliteEventFilter() {}

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event){
        switch (event->type()){
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseButtonDblClick:
            case QEvent::MouseMove:
            case QEvent::KeyPress:
            case QEvent::KeyRelease:
            case QEvent::Wheel:
            {
                has_activity = true;
                counter = 0;

                break;
            }
            default:
            {
                has_activity = false;

                break;
            }
        }

        return QObject::eventFilter(obj, event);
    }

private Q_SLOTS:
    void tick(){
        if (!has_activity)
            ++counter;

        if (WBGET(WB_APP_AUTOAWAY_BY_TIMER)){
            int mins = WIGET(WI_APP_AUTOAWAY_INTERVAL);

            if (!mins)
                return;

            int mins_done = (counter*timer.interval()/1000)/60;

            if (mins <= mins_done)
                dcpp::Util::setAway(true);
        }
        else if (has_activity && !dcpp::Util::getManualAway())
            dcpp::Util::setAway(false);
    }

private:
    QTimer timer;
    int counter;
    bool has_activity;
};

class SkyliteApp: public QtSingleCoreApplication {
Q_OBJECT
public:
    SkyliteApp(int &argc, char *argv[], const QString& uniqKey): QtSingleCoreApplication(argc, argv, uniqKey)
    {
        installEventFilter(&ef);
        installMacHandlers();
    }

    void commitData(QSessionManager& manager){
        if (MainWindow::getInstance()){
            MainWindow::getInstance()->beginExit();
            MainWindow::getInstance()->close();
        }

        manager.release();
    }

    void saveState(QSessionManager &){ /** Do nothing */ }

private:
    SkyliteEventFilter ef;

    void installMacHandlers(){
        objc_object* cls = objc_getClass("NSApplication");
        SEL sharedApplication = sel_registerName("sharedApplication");
        objc_object* appInst = objc_msgSend(cls, sharedApplication);

        if (appInst){
            objc_object* delegate = objc_msgSend(appInst,  sel_registerName("delegate"));
            objc_object* delClass = objc_msgSend(delegate, sel_registerName("class"));
            const char* tst = class_getName(delClass->isa);
            bool test = class_addMethod((objc_class*)delClass,
                                        sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:"),
                                        (IMP)dockClickHandler,"B@:");
            if (!test){
                // failed to register handler...
            }
        }
    }
};
