#include "workbreak.h"
#include <QtDebug>
#include <QtGui/QMessageBox>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QTime>

#ifdef HAVE_QTDBUS
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#endif


WorkBreak::WorkBreak(QObject *parent)
    : QSystemTrayIcon(parent)
{
    m_iconPaths[Green] = QString(ICON_PREFIX) + "green.png";
    m_iconPaths[Yellow] = QString(ICON_PREFIX) + "yellow.png";
    m_iconPaths[Red] = QString(ICON_PREFIX) + "red.png";

    m_icons[Green] = QIcon(m_iconPaths[Green]);
    m_icons[Yellow] = QIcon(m_iconPaths[Yellow]);
    m_icons[Red] = QIcon(m_iconPaths[Red]);

    QTime initial(0, 0);
    int intervals = 24;
    for (int i = 0; i < intervals; ++i)
        m_schedule << initial.addSecs(i * (24*60*60/intervals));
    qSort(m_schedule.begin(), m_schedule.end());
    qDebug() << "Schedule:" << m_schedule;

    setStatus(Green);
    setToolTip(tr("Application just started. Calculating schedule..."));

#ifdef HAVE_QTDBUS
    m_interface = new OrgFreedesktopNotificationsInterface("org.freedesktop.Notifications",
                                                           "/org/freedesktop/Notifications",
                                                           QDBusConnection::sessionBus());
    connect(m_interface, SIGNAL(NotificationClosed(uint, uint)),
           this, SLOT(notificationClosed(uint,uint)));
    connect(m_interface, SIGNAL(ActionInvoked(uint,QString)),
            this, SLOT(handleAction(uint,QString)));
#endif

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    // minute interval
    m_timer->setInterval(60000);
    m_timer->start();
}

WorkBreak::~WorkBreak()
{
}

void WorkBreak::setStatus(Status status)
{
    m_status = status;
    setIcon(m_icons[status]);
}

void WorkBreak::timerTimeout()
{
    qDebug() << "void WorkBreak::timerTimeout()";
    m_timer->stop();

    QTime current = QTime::currentTime();
    QTime nextSchedule;

    // find the nearest next event
    foreach (QTime t, m_schedule)
    {
        if (t > current && !m_scheduleDone.contains(t))
        {
            nextSchedule = t;
            break;
        }
        else
            continue;
    }

    if (nextSchedule.isNull())
    {
        qDebug() << "No next schedule found";
        m_scheduleDone.clear();
    }
    else
    {
        qDebug() << "Next trigger @" << nextSchedule << "current:" << current;

        int interval = current.secsTo(nextSchedule);
        if (interval < 59) // main alert
        {
            setStatus(Red);
            message(tr("A title"), tr("Take a rest!"));
            m_scheduleDone.append(nextSchedule);
            setToolTip(tr("Take a rest!"));
        }
        else if (interval < 1600) // warm-up icon
        {
            setStatus(Yellow);
            setToolTip(tr("Next break in %1 minute(s)").arg(interval/60));
        }
        else
        {
            if (m_status != Green)
            {
                setStatus(Green);
                setToolTip(tr("You are fresh. Continue to work..."));
            }
        }
    }

    m_timer->start();
}

void WorkBreak::message(const QString &title,
                        const QString &body)
{
    qDebug() << "Notification to show:" << title << body;
    if (freedesktopNotify(title, body))
    {
        return;
    }
    else if (QSystemTrayIcon::supportsMessages())
    {
        qDebug() << "No freedesktop dbus service available. Trying tray message.";
        showMessage(title, body, QSystemTrayIcon::Warning, 10000);
    }
    else
    {
        qDebug() << "No appropriate message method found. Using modal dialog.";
        QMessageBox msg(QMessageBox::Warning, title, body, QMessageBox::Close);
        msg.setIconPixmap(m_icons[m_status].pixmap(64));
        msg.exec();
    }
}

bool WorkBreak::freedesktopNotify(const QString &title, const QString &summary)
{
#ifdef HAVE_QTDBUS
    if (m_interface->lastError().isValid())
    {
        qDebug() << "FD01" << m_interface->lastError().message();
        return false;
    }

    QDBusPendingReply<uint> reply = m_interface->Notify(qApp->applicationName(), // app_name
                                                        0, // replaces_id - server assigns ID, we will just ignore it
                                                        m_iconPaths[m_status], // app_icon
                                                        title, // summary
                                                        summary, // body
                                                        QStringList() << "default" << tr("Yes, I'm taking a break!"), // actions
                                                        QVariantMap(), // hints
                                                        0); // expire_timeout = never

    reply.waitForFinished();
    if (reply.isError())
    {
        qDebug() << "FD02" << reply.error().message();
        return false;
    }
    qDebug() << "Notification ID: "<< reply.value();
    return true;
#else
    return false;
#endif
}

#ifdef HAVE_QTDBUS
void WorkBreak::notificationClosed(uint, uint)
{
    timerTimeout();
}

void WorkBreak::handleAction(uint id, QString)
{
    m_interface->CloseNotification(id);
}
#endif

