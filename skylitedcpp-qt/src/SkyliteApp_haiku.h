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
#include <Roster.h>
#include "WulforSettings.h"
#include "MainWindow.h"

class SkyliteEventFilter: public QObject{
Q_OBJECT
public:
    SkyliteEventFilter(): has_activity(true), counter(0){
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

class SkyliteApp: public QApplication {
Q_OBJECT
public:
    SkyliteApp(int &argc, char *argv[], const QString& uniqKey): QApplication(argc, argv)
    {
        installEventFilter(&ef);
    }
    bool isRunning() {
        return be_roster->IsRunning("application/x-vnd.Skylitedcpp++");
    }

    void commitData(QSessionManager& manager){
        if (MainWindow::getInstance()){
            MainWindow::getInstance()->beginExit();
            MainWindow::getInstance()->close();
        }

        manager.release();
    }

    void saveState(QSessionManager &){ /** Do nothing */ }

Q_SIGNALS:
    void messageReceived(const QString &message);

private:
    SkyliteEventFilter ef;
};
