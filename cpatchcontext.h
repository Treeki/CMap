#ifndef CPATCHCONTEXT_H
#define CPATCHCONTEXT_H

#include <QtCore/QVector>
#include <QtCore/QDir>
#include "cshapepool.h"

class CPatchContext : public QObject {
	Q_OBJECT

public:
	explicit CPatchContext(QObject *parent = 0);

	QVector<QRgb> palette;
	CShapePool items, floors, walls, effects, regions;

	// Todo: This thing should handle animations?

	void loadDefaultPalette();
	void loadPatches(QString defaultPath, QString customPath = QString::null);
private:
	void loadPatchDir(QDir path, bool isCustom);
};

#endif // CPATCHCONTEXT_H
