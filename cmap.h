#ifndef CMAP_H
#define CMAP_H

#include <QtCore/QtDebug>
#include <QtCore/QString>
#include <QtCore/QDataStream>
#include "cmappoint.h"
#include "cmapvector.h"

#define MAX_MAP_WIDTH 300
#define MAX_MAP_HEIGHT 600

#define MAX_REGION 40000

class CPatchContext;

class CMap : public QObject {
	Q_OBJECT
public:
	enum ObjectType {
		NoObject = 0, Floor = 1, Item = 2,
		LeftWall = 4, RightWall = 8, Wall = 12,
		Region = 16, Effect = 32
	};

	static const char *nameForObjectType(ObjectType type);

	explicit CMap(QObject *parent = 0, int width = 70, int height = 70);

	// Utility
	int randomisedItem(int x, int y) const;
	static int randomisedItem(int x, int y, int shapeNum);

	int randomisedFloor(int x, int y) const;
	static int randomisedFloor(int x, int y, int shapeNum);

	bool positionValid(CMapPoint &pos) const {
		return (pos.x < (m_width * 2) && pos.y < m_height);
	}

	static inline int positionKey(int x, int y) {
		return (x << 12) | y;
	}
	static inline int positionKey(const CMapPoint &pos) {
		return (pos.x << 12) | pos.y;
	}


	// Core map data
private:
	bool m_available;
	int m_width, m_height;

public:
	bool available() const { return m_available; }

	int width() const { return m_width; }
	int height() const { return m_height; }

	CMapVector<quint16, MAX_MAP_WIDTH, true> items;
	CMapVector<quint16, MAX_MAP_WIDTH, true> floors;
	CMapVector<quint16, MAX_MAP_WIDTH, true> regions;
	CMapVector<quint16, MAX_MAP_WIDTH, true> effects;
	CMapVector<quint8, MAX_MAP_WIDTH*2, false> walls;

	virtual void resize(int newWidth, int newHeight);

	int get(CMap::ObjectType type, const CMapPoint &pos);

	// Patch info
	enum PatchType {
		NoPatch = 0,
		CustomPatch,
		ServerPatch
	};

	PatchType patchType;
	QString patchName;

	bool isEncrypted;

	// Settings
	int revision;
	QString dreamName;

	enum StandardType {
		E8Standard = 0,
		TeenStandard,
		M16Standard,
		A18Standard,
		AOStandard,
		AOCleanStandard
	};

	StandardType standard;

	bool allowSummon, allowFollow, allowURLs;
	bool enableFilter, parentalWarning;
	bool restrictSitting;
	bool allowShouting, allowWho, allowTab, allowSeasonals;
	bool hasGroupPackage;



	bool load(QDataStream &stream);
	bool load(QString filePath);

	bool save(QDataStream &stream);
	bool save(QString filePath);

	bool needsVersion140() const;

	void loadPatchContext(CPatchContext *pc, QString defaultPath, QString mapDir);

	enum ErrorType {
		NoError = 0,
		BadVersionLineError,
		UnsupportedVersionError,
		BadHeaderError,
		InvalidMapSizeError,
		MapTooBigError,
		FileIsEncryptedError,
		CannotOpenFileError
	};

	ErrorType error() const;

private:
	ErrorType m_error;
};

#endif // CMAP_H
