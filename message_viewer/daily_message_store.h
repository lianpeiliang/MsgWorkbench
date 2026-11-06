#ifndef DAILY_MESSAGE_STORE_H
#define DAILY_MESSAGE_STORE_H

#include <QObject>
#include <QDateTime>

class DailyMessageStorePrivate;
// 每天使用一个 SQLite 文件，查询方法基于当前打开的日期。
class DailyMessageStore : public QObject
{
    Q_OBJECT
public:
    explicit DailyMessageStore(QObject *parent = nullptr);
    ~DailyMessageStore();

    // 数据库存放路径
    void setDbPath(const QString &path);
    // 按消息时间所属的日期写入数据库。
    bool addMsg(int type, const QString &msg, const QString &title,
                const QDateTime &time = QDateTime::currentDateTime());

    struct DbLogInfo
    {
        DbLogInfo() : id(0), type(0)
        {}
        qint64 id;
        QString title;
        QString info;
        QDateTime dt;
        int type;
    };
    // 判断指定日期的数据库文件是否存在。
    bool exist(const QDate &date);
    // 以只读模式打开指定日期的数据库。
    bool open(const QDate &date);
    // 返回数据库条目
    qint64 count();
    // 按记录 ID 升序分页，from 是从 0 开始的偏移量。
    QList<DbLogInfo> getData(qint64 from, int count);

    // 删除指定日期的数据库文件。
    bool remove(const QDate &date);
    bool removeall();

private:
    void close();
    void initDb(const QDate &date, bool bIsRead);
private:
    DailyMessageStorePrivate *d;
};

#endif // DAILY_MESSAGE_STORE_H
