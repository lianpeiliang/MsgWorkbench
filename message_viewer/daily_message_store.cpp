#include "daily_message_store.h"
#include <QtSql>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QUuid>

class DailyMessageStorePrivate
{
public:
    QSqlDatabase db;
    bool bIsRead = false;
    QDate date;
    QString path;
};

DailyMessageStore::DailyMessageStore(QObject *parent) : QObject(parent), d(new DailyMessageStorePrivate)
{
    if (!QSqlDatabase::drivers().contains("QSQLITE"))
    {
        qCritical() << "DailyMessageStore:Unable to load SQLITE driver";
    }
}

DailyMessageStore::~DailyMessageStore()
{
    close();
    delete d;
}

void DailyMessageStore::setDbPath(const QString &path)
{
    if (d->path != path)
    {
        close();
    }
    d->path = path;
}

bool DailyMessageStore::addMsg(int type, const QString &msg, const QString &title, const QDateTime &time)
{
    initDb(time.date(), false);
    if (!d->db.isOpen())
    {
        qCritical() << "addMsg failed: DailyMessageStore is not open";
        return false;
    }
    QSqlQuery q(d->db);
    // 插入数据库记录
    if (!q.prepare(QLatin1String("insert into logtable(title, info, dtime, type) values(:title, :info, :dtime, :type)")))
    {
        qCritical() << "DailyMessageStore insert error" << q.lastError();
        return false;
    }
    q.bindValue(":title", title);
    q.bindValue(":info", msg);
    q.bindValue(":dtime", time);
    q.bindValue(":type", type);
    if (!q.exec())
    {
        qCritical() << "DailyMessageStore insert into table error" << q.lastError();
        return false;
    }
    return true;
}

bool DailyMessageStore::exist(const QDate &date)
{
    QDir dir(d->path);
    QString path = dir.absoluteFilePath(date.toString("yyyy-MM-dd") + ".db");
    // 判断制定日志的数据库文件是否存在
    return QFile::exists(path);
}

bool DailyMessageStore::open(const QDate &date)
{
    initDb(date, true);
    return d->db.isOpen();
}

qint64 DailyMessageStore::count()
{
    if (!d->db.isOpen())
    {
        qCritical() << "DailyMessageStore is not open";
        return 0;
    }

    QSqlQuery q(d->db);
    // 统计个数
    q.exec("select count(id) from logtable");
    q.next();
    return q.value(0).toLongLong();
}

QList<DailyMessageStore::DbLogInfo> DailyMessageStore::getData(qint64 from, int count)
{
    QList<DbLogInfo> infos;
    if (!d->db.isOpen())
    {
        qCritical() << "getData error: DailyMessageStore is not open";
        return infos;
    }

    QSqlQuery q(d->db);
    // 跳过前 from 条记录，最多读取 count 条。按 ID 排序保证分页顺序稳定。
    q.exec(QString("select id,title,info,dtime,type from logtable order by id limit %1 offset %2")
           .arg(count).arg(from));
    while (q.next())
    {
        DbLogInfo tmp;
        tmp.id = q.value("id").toLongLong();
        tmp.title = q.value("title").toString();
        tmp.info = q.value("info").toString();
        tmp.dt = q.value("dtime").toDateTime();
        tmp.type = q.value("type").toInt();
        infos.append(tmp);
    }
    return infos;
}

bool DailyMessageStore::remove(const QDate &date)
{
    // 删除前释放 SQLite 文件句柄，避免 Windows 拒绝删除仍打开的文件。
    close();
    QDir dir(d->path);
    QString path = dir.absoluteFilePath(date.toString("yyyy-MM-dd") + ".db");
    return !QFile::exists(path) || QFile::remove(path);
}

bool DailyMessageStore::removeall()
{
    close();
    QDir dir(d->path);
    bool removed = true;
    for (const auto &entry : dir.entryInfoList({QStringLiteral("*.db")}, QDir::Files))
    {
        if (!QFile::remove(entry.filePath()))
        {
            removed = false;
        }
    }
    return removed;
}

void DailyMessageStore::close()
{
    const QString connectionName = d->db.connectionName();
    if (!connectionName.isEmpty())
    {
        d->db.close();
        // 释放连接对象后，再从 Qt 的全局连接表中移除它。
        d->db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }
}

void DailyMessageStore::initDb(const QDate &date, bool bIsRead)
{
    if (d->db.isOpen() && date == d->date && bIsRead == d->bIsRead)
    {
        return;
    }
    close();
    d->date = date;
    d->bIsRead = bIsRead;
    // Qt 按名称管理连接，使用独立名称避免不同实例覆盖彼此的连接。
    d->db = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    QDir dir(d->path);
    QString path = dir.absoluteFilePath(date.toString("yyyy-MM-dd") + ".db");
    // 设置数据库文件路径
    d->db.setDatabaseName(path);
    if (bIsRead)
    {
        // 查询历史时即使文件已被移除，也不会创建空数据库。
        d->db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY"));
    }

    if (!d->db.open())
    {
        qCritical() << "DailyMessageStore open" << d->db.lastError();
        return;
    }

    if (bIsRead)
    {
        return;
    }
    QStringList tables = d->db.tables();
    // 判断是否存在日志表格
    if (tables.contains("logtable", Qt::CaseInsensitive))
    {
        return;
    }

    QSqlQuery q(d->db);
    // 如果不存在表格，插入新表
    if (!q.exec(QLatin1String("create table logtable(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                              "title TEXT NOT NULL, info TEXT NOT NULL, dtime TEXT NOT NULL, type INTEGER NOT NULL)")))
    {
        qCritical() << "DailyMessageStore create table" << q.lastError();
        return;
    }
}
