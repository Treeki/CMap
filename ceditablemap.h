#ifndef CEDITABLEMAP_H
#define CEDITABLEMAP_H

#include "cmap.h"
#include <QBitArray>
#include "cundostack.h"

class CMapWidget;

class CEditableMap : public CMap
{
	Q_OBJECT

	friend class CMapWidget;

public:
	explicit CEditableMap(QObject *parent = 0, int width = 70, int height = 70);
	
	void resize(int newWidth, int newHeight);

	enum UpdateType {
		NoUpdate = 0,
		FloorUpdate = 1,
		ItemUpdate = 2,
		LeftWallUpdate = 4,
		RightWallUpdate = 8,
		WallUpdate = LeftWallUpdate | RightWallUpdate,
		RegionUpdate = 16,
		EffectUpdate = 32,
		HighlightUpdate = 64,
		EverythingUpdate = FloorUpdate | ItemUpdate | WallUpdate | RegionUpdate | EffectUpdate
	};

	void modify(CMap::ObjectType type, const CMapPoint &pos, quint16 value);

	CUndoStack undo;

signals:
	void updateTile(const CMapPoint &point, CEditableMap::UpdateType type);

protected:
	bool m_hasSelection;
	QBitArray m_selectionMask;
	
public slots:
	
};

#endif // CEDITABLEMAP_H
