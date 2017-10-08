#ifndef CPUINFOITEM_H
#define CPUINFOITEM_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtQml>

namespace Task
{
QString loadCpuInfo(const QString& pathToFile);
/**
 * @brief Load and parse cpu info and provide a table view of the resutls
 * the object will constantly updated to keep the model uptodate
 */
class CpuInfoItem : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList dataHeaders READ headers  NOTIFY dataHeadersChanged)
    Q_PROPERTY(bool track READ getTrack WRITE setTrack NOTIFY dataHeadersChanged)
public:
    explicit CpuInfoItem(const bool track, const int updateInterval = 1000);

    int cpuCount() const;
    /**
     * @brief whether to track the cpu info
     * @return
     */
    bool getTrack() const;
    /**
     * @brief set to track the cpu info
     * @param track
     */
    void setTrack(const bool track);

    int parseCpuInfo(const QString& fileContent);

    int rowCount(const QModelIndex & = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    /**
     * @brief collect the cpu info entry headers to display as table headers
     * @return
     */
    QStringList headers() const;

protected:
    void timerEvent(QTimerEvent * ev);
    QHash<int, QByteArray> dataHeaders() const;

signals:
    void dataHeadersChanged();

private:
    void clearData();
    bool collectCpuDetail(const QStringList& cpuBlocks);
    int m_cpuCount;
    int m_timerInterval;
    bool m_track;
    QStringList m_cpuDetailHeaders;
    QVector<QStringList> m_cpuDetails;
    QBasicTimer m_notifyTimer;
};
}

#endif // CPUINFOITEM_H
