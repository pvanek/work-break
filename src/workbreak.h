/*
  Licensed under LPGL v2 or later.
  Author: Petr Vanek <petr@yarpen.cz>
 */

#ifndef WORKBREAK_H
#define WORKBREAK_H

#include <QtWidgets/QSystemTrayIcon>
#include <QtCore/QHash>
#ifdef HAVE_QTDBUS
#include "notifications_interface.h"
#endif

class QTimer;


class WorkBreak : public QSystemTrayIcon
{
    Q_OBJECT

public:

    enum Status {Init, Green, Yellow, Red};

    WorkBreak(QObject *parent = 0);
    ~WorkBreak();

    void setStatus(Status status);
    QString currentIcon() const;

public slots:
    void message(const QString &title,
                 const QString &body);

private:
    QString m_appPrefix;
    QTimer *m_timer;
    Status m_status;

    QHash<Status,QString> m_iconPaths;
    QHash<Status,QIcon> m_icons;

    QList<QTime> m_schedule;
    QList<QTime> m_scheduleDone;

#ifdef HAVE_QTDBUS
    OrgFreedesktopNotificationsInterface* m_interface;
#endif

    bool freedesktopNotify(const QString &title, const QString &summary);

private slots:
    void timerTimeout();
    void about();

#ifdef HAVE_QTDBUS
    void notificationClosed(uint, uint);
    void handleAction(uint, QString);
#endif

};

#endif // WORKBREAK_H
