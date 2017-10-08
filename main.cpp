
#include <QGuiApplication>
#include <QStringList>

#include <QObject>

#include <qqmlengine.h>
#include <qqmlcontext.h>
#include <qqml.h>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickview.h>

#include "cpuinfoitem.h"
/*
   This example illustrates exposing a QStringList as a
   model in QML
*/

using namespace Task;

int main(int argc, char ** argv)
{
    QGuiApplication app(argc, argv);
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    QQmlContext *ctxt = view.rootContext();

    QString fileContent;
    try
    {
        fileContent = loadCpuInfo(QString("/proc/cpuinfo"));
    }
    catch (std::exception e)
    {
        qErrnoWarning(e.what());
        return -1;
    }

    // start parsing the file, keep updating the table every 1000 ms
    CpuInfoItem cpuInfoItem(true, 1000);
    if (cpuInfoItem.parseCpuInfo(fileContent))
    {
        ctxt->setContextProperty("cpuInfo", &cpuInfoItem);
        view.setSource(QUrl("qrc:view.qml"));
    }
    else // fallback: failed to parse, display raw data
    {
        ctxt->setContextProperty("fallback", fileContent);
        view.setSource(QUrl("qrc:fallback.qml"));
    }

    view.show();

    return app.exec();
}
