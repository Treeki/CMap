#ifndef UTILITY_H
#define UTILITY_H

#include <QFile>
#include <QDir>

bool cCaseInsensitiveCd(QDir &dir, const QString &dirName);
QString cCaseInsensitiveFilePath(QDir &dir, const QString &fileName);

//QString cFixCaseInsensitiveFilePath(const QString &origPath, bool *didWork = 0);

#endif // UTILITY_H
