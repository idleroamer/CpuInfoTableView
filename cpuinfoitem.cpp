#include <QFile>
#include <QTextStream>

#include "cpuinfoitem.h"

namespace Task
{
QString loadCpuInfo(const QString& pathToFile)
{
    QString fileContent;

    QFile inputFile(pathToFile);
    if ( inputFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        fileContent = QTextStream(&inputFile).readAll();
    }
    else
    {
        throw (std::runtime_error("Could not open" + pathToFile.toStdString()));
    }
    return fileContent;
}

/**
 * @brief using "model" as tablecoloumn header in table view
 * messup with own qml internal data!!!!!
 * @param header
 */
static void QtBugWorkAround(QString* header)
{
    if (*header == "model")
        *header = "cpu model";
}
CpuInfoItem::CpuInfoItem(const bool track, const int updateInterval): m_cpuCount(0),
    m_timerInterval(updateInterval), m_track(track)
{
    m_notifyTimer.start(m_timerInterval, this);
}


int CpuInfoItem::cpuCount() const
{
    return m_cpuCount;
}

bool CpuInfoItem::getTrack() const
{
    return m_track;
}
void CpuInfoItem::setTrack(const bool track)
{
    if (m_track != track)
    {
        m_track = track;
        m_track ? m_notifyTimer.start(m_timerInterval, this) : m_notifyTimer.stop();
    }
}

int CpuInfoItem::parseCpuInfo(const QString &fileContent)
{
    bool ret = true;

    QStringList cpuBlocks = fileContent.split("\n\n");
    // remove empty blocks
    cpuBlocks.removeOne(QString(""));

    if (collectCpuDetail(cpuBlocks))
        m_cpuCount = cpuBlocks.length();
    else
        ret = false;

    return ret;
}

int CpuInfoItem::rowCount(const QModelIndex &) const {
    return cpuCount();
}

QVariant CpuInfoItem::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_cpuDetails.size())
        return QVariant();

    QString temp;
    int x_index = role - Qt::UserRole - 1;
    if (x_index <= m_cpuDetails[index.row()].size())
        temp = m_cpuDetails[index.row()][x_index];
    return temp;
}


QStringList CpuInfoItem::headers() const
{
    QVector<QPair<int, QByteArray> > roles;
    int i = 1;
    for(const QString& header : m_cpuDetailHeaders)
        roles.append(qMakePair(Qt::UserRole + i++, QByteArray(header.toUtf8())));
    QStringList headers;
    for (auto it = roles.begin(); it != roles.end(); ++it)
        headers.append(it->second);

    return headers;
}

QHash<int, QByteArray> CpuInfoItem::dataHeaders() const
{
    QHash<int, QByteArray>  roles;
    int i = 1;
    for(const QString& header : m_cpuDetailHeaders)
        roles[Qt::UserRole + i++] = QByteArray(header.toUtf8());
    return roles;
}

void CpuInfoItem::clearData()
{
    m_cpuDetailHeaders.clear();
    m_cpuDetails.clear();
    m_cpuCount = 0;
}

bool CpuInfoItem::collectCpuDetail(const QStringList& cpuBlocks)
{
    for (const QString& cpu : cpuBlocks)
    {
        QStringList cpuDetail;
        for (const QString& entry : cpu.split("\n"))
        {
            if (entry.indexOf(":") > 0)
            {
                QStringList headerValues = entry.split(":");
                // collect header
                m_cpuDetailHeaders.append(headerValues.first().trimmed());
                QtBugWorkAround(&m_cpuDetailHeaders.last());
                // collect values
                cpuDetail.append(headerValues.size() > 1 ? headerValues[1] : "");
            }

        }
        m_cpuDetails.append(cpuDetail);
    }
    m_cpuDetailHeaders.removeDuplicates();
    return !m_cpuDetailHeaders.empty();
}
void CpuInfoItem::timerEvent(QTimerEvent * ev) {
    if (ev->timerId() != m_notifyTimer.timerId()) return;

    clearData();

    QString fileContent = loadCpuInfo(QString("/proc/cpuinfo"));
    if (parseCpuInfo(fileContent))
    {
        emit dataChanged(createIndex(0, 0),
                         createIndex(m_cpuDetailHeaders.count() - 1, 0));
    }
}

}
