/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#pragma once

#include <QPushButton>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QMouseEvent>

class QResizeEvent;
class QPaintEvent;
class QLabel;
class QEvent;

class TabButton : public QPushButton
{
Q_OBJECT
public:
    explicit TabButton(QWidget *parent = 0);

    QSize sizeHint() const;
    void setWidgetIcon(const QPixmap &px);
    void resetGeometry() { updateGeometry(); }
    int normalWidth() const;
    int normalHeight() const;

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual bool eventFilter(QObject *, QEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dragMoveEvent(QDragMoveEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *e);

signals:
    void closeRequest();
    void dropped(TabButton*);

private:
    void updateStyles();
    void updateGeometry();

    QLabel *label;
    QLabel *px_label;
    int parentHeight;
    bool isLeftBtnHold;
};
