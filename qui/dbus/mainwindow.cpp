#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDBusConnection>
#include <QDebug>
#include <QtDBus>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (!QDBusConnection::systemBus().isConnected()) {
        qDebug() << "Cannot connect to system bus";
    }

    bool connected = QDBusConnection::systemBus().connect(
            "org.freedesktop.Hal",
            "/org/freedesktop/Hal/Manager",
            "org.freedesktop.Hal.Manager",
            "DeviceAdded",
            this, SLOT(deviceAdded(const QString &)));

    bool disconnect = QDBusConnection::systemBus().connect(
                "org.freedesktop.Hal",
                "/org/freedesktop/Hal/Manager",
                "org.freedesktop.Hal.Manager",
                "DeviceRemoved",
                this, SLOT(deviceRemoved(const QString &)));

}


MainWindow::~MainWindow()
{
    delete ui;
}

template<typename T>
T dbusRequest(QDBusInterface &i, const QString &method, const QString &param)
{
    QDBusReply<T> reply = i.call(method, param);

    return reply.value();
}


void MainWindow::deviceAdded(const QString &udi)
{
    QDBusInterface i("org.freedesktop.Hal",
                     udi,
                     "org.freedesktop.Hal.Device",
                     QDBusConnection::systemBus());

    QStringList caps = dbusRequest<QStringList>(i, "GetProperty", "info.capabilities");

    //qDebug() << "Added device with UDI" << udi << caps;
    //ui->textEdit->append((QString("Added device with UDI %1 %2").arg(udi).arg(caps)));
    ui->textEdit->append((QString("Added device with UDI %1").arg(udi)));

    if(dbusRequest<QString>(i, "GetProperty", "volume.fsusage") != "filesystem" &&
            !dbusRequest<bool>(i, "GetProperty", "volume.disc.has_audio"))
        return;

    QDBusMessage reply = i.call("GetProperty", "volume.size");


    QString product = dbusRequest<QString>(i, "GetProperty", "info.product");
    QString size = reply.arguments().first().toString();
    QString parent = dbusRequest<QString>(i, "GetProperty", "block.storage_device");


    QDBusInterface iParent("org.freedesktop.Hal",
                parent,
                "org.freedesktop.Hal.Device",
                QDBusConnection::systemBus());

    QString model = dbusRequest<QString>(iParent, "GetProperty", "storage.model");
    QString vendor = dbusRequest<QString>(iParent, "GetProperty", "storage.vendor");
    QString bus = dbusRequest<QString>(iParent, "GetProperty", "storage.bus");

    qDebug() << (QString("Added device, model (%1), vendor (%2), bus (%3), product (%4), size (%5)")
            .arg(model).arg(vendor).arg(bus).arg(product).arg(size));

}

void MainWindow::deviceRemoved(const QString &udi)
{
    ui->textEdit->append((QString("Removed device with UDI %1").arg(udi)));
}
