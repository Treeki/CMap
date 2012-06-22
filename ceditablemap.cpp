#include "ceditablemap.h"


CEditableMap::CEditableMap(QObject *parent, int width, int height) :
	CMap(parent, width, height),
	hasSelection(false) {
	undo.baseStack()->setUndoLimit(64);
}


void CEditableMap::resize(int newWidth, int newHeight) {
	CMap::resize(newWidth, newHeight);

	hasSelection = false;
	selectionMask.resize(width() * height());
	selectionMask.clear();
}


void CEditableMap::modify(CMap::ObjectType type, const CMapPoint &pos, quint16 value) {
	emit updateTile(pos, (CEditableMap::UpdateType)type);

	switch (type) {
	case CMap::Item:
		items.set(pos, value);
		break;
	case CMap::Floor:
		floors.set(pos, value);
		break;
	case CMap::LeftWall: case CMap::RightWall: case CMap::Wall:
		walls.set(pos, value);
		break;
	case CMap::Region:
		regions.set(pos, value);
		break;
	case CMap::Effect:
		effects.set(pos, value);
		break;
	case CMap::NoObject: break;
	}

	emit updateTile(pos, (CEditableMap::UpdateType)type);
}
