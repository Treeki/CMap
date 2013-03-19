#include "cfilltool.h"
#include "cmapwidget.h"
#include "ceditcommand.h"
#include <QQueue>

CFillTool::CFillTool(QObject *parent) : CCommandToolBase(parent) {
	m_whatType = CMap::Item;
	m_whatValue = 1;
	m_hoveredHalfX = -1;
}



CMap::ObjectType CFillTool::typesToPreview() const {
	return m_whatType;
}

int CFillTool::whatThingFor(CMap::ObjectType type, int x, int y, int normal) const {
	if (type & CMap::Wall && m_whatType & CMap::Wall) {
		if (x == m_hovered.x && y == m_hovered.y)
			return m_whatValue;
	} else {
		if (x == m_hoveredHalfX && y == m_hovered.y)
			return m_whatValue;
	}
	return normal;
}


void CFillTool::setWhat(CMap::ObjectType type, int value) {
	widget()->updateTile(m_hovered, (CEditableMap::UpdateType)m_whatType);
	m_whatType = type;
	m_whatValue = value;
	widget()->updateTile(m_hovered, (CEditableMap::UpdateType)m_whatType);
}


void CFillTool::hoverStatusChanged(bool fromValid, const CMapPoint &from, bool toValid, const CMapPoint &to, QMouseEvent *event) {
	CMapPoint realFrom = m_hovered;
	CMapPoint realTo = to;

	if (!(m_whatType & CMap::Wall)) {
		// make sure we're always at an even position
		realTo.clearRightWall();
	}

	if (fromValid)
		widget()->updateTile(realFrom, (CEditableMap::UpdateType)m_whatType);
	if (toValid)
		widget()->updateTile(realTo, (CEditableMap::UpdateType)m_whatType);

	m_hovered = realTo;
	m_hoveredHalfX = realTo.x / 2;

	if (fromValid)
		widget()->updateTile(realFrom, (CEditableMap::UpdateType)m_whatType);
	if (toValid)
		widget()->updateTile(realTo, (CEditableMap::UpdateType)m_whatType);
}


void CFillTool::tileMouseAltAction(const CMapPoint &tile, QMouseEvent *event) {
	CCommandToolBase::tileMouseAltAction(tile, event);

	int thing = widget()->map()->get(m_whatType, tile);
	if (thing != m_whatValue) {
		// update the tile before and after changing m_whatValue
		widget()->updateTile(tile, (CEditableMap::UpdateType)m_whatType);
		m_whatValue = thing;
		widget()->updateTile(tile, (CEditableMap::UpdateType)m_whatType);

		emit shapePicked(thing);
	}
}


void CFillTool::finaliseCommand() {
	// fill wherever we are!
	doFill();

	if (m_command->changes.size() > 0) {
		int amount = m_command->changes.count();

		bool isPlacement = !((m_whatType != CMap::Floor) && m_whatValue == 0);

		QString text = isPlacement ? "place " : "delete ";
		const char *typeName = CMap::nameForObjectType(m_whatType);

		if (amount > 1) {
			text.append(QString("%1 %2s").arg(amount).arg(typeName));
		} else {
			text.append("one ");
			text.append(typeName);
		}

		if (isPlacement)
			text.append(QString(" of type %1").arg(m_whatValue));

		m_command->setText(text);
	}

	CCommandToolBase::finaliseCommand();
}



void CFillTool::doFill() {
	// This is not the nicest code, but.. it works?

	auto map = widget()->map();
	int toReplace = map->get(m_whatType, m_hovered);

	if (toReplace == m_whatValue)
		return;

	QBitArray checked;
	checked.resize(map->width() * map->height());

	QQueue<CMapPoint> queue;
	queue.enqueue(m_hovered);

	while (!queue.isEmpty()) {
		CMapPoint point = queue.dequeue();

		int bitID = ((point.x / 2) * map->width()) + point.y;
		checked.setBit(bitID);

		CEditCommand::Change change;
		change.before = toReplace;
		change.after = m_whatValue;
		change.type = m_whatType;
		change.pos = point;
		m_command->changes.append(change);

		// Check every side to see what we've got to add to the list
		CMapPoint nw = point;
		nw.moveNW();
		int nwBit = ((nw.x / 2) * map->width()) + nw.y;
		if (map->positionValid(nw) && !checked.testBit(nwBit) && map->get(m_whatType, nw) == toReplace) {
			checked.setBit(nwBit);
			queue.enqueue(nw);
		}

		CMapPoint ne = point;
		ne.moveNE();
		int neBit = ((ne.x / 2) * map->width()) + ne.y;
		if (map->positionValid(ne) && !checked.testBit(neBit) && map->get(m_whatType, ne) == toReplace) {
			checked.setBit(neBit);
			queue.enqueue(ne);
		}

		CMapPoint sw = point;
		sw.moveSW();
		int swBit = ((sw.x / 2) * map->width()) + sw.y;
		if (map->positionValid(sw) && !checked.testBit(swBit) && map->get(m_whatType, sw) == toReplace) {
			checked.setBit(swBit);
			queue.enqueue(sw);
		}

		CMapPoint se = point;
		se.moveSE();
		int seBit = ((se.x / 2) * map->width()) + se.y;
		if (map->positionValid(se) && !checked.testBit(seBit) && map->get(m_whatType, se) == toReplace) {
			checked.setBit(seBit);
			queue.enqueue(se);
		}
	}

/*	while (!queue.isEmpty()) {
		CMapPoint point = queue.dequeue();

		// First off, check the NW side of this point
		CMapPoint check = point;

		while (true) {
			check.moveNW();
			if (!map->positionValid(check))
				break;
			if (map->get(m_whatType, check) != toReplace)
				break;

			int bitID = ((check.x / 2) * map->width()) + check.y;
			if (checked.testBit(bitID))
				break;
			checked.setBit(bitID);

			CEditCommand::Change change;
			change.before = toReplace;
			change.after = m_whatValue;
			change.type = m_whatType;
			change.pos = check;
			m_command->changes.append(change);

			// Check if we need to add anything SW/NE of this one
			CMapPoint checkSW = check;
			checkSW.moveSW();
			if (map->positionValid(checkSW) && map->get(m_whatType, checkSW) == toReplace)
				queue.enqueue(checkSW);

			CMapPoint checkNE = check;
			checkNE.moveNE();
			if (map->positionValid(checkNE) && map->get(m_whatType, checkNE) == toReplace)
				queue.enqueue(checkNE);
		}

		// Now check the SE side
		check = point;

		while (true) {
			check.moveSE();
			if (!map->positionValid(check))
				break;
			if (map->get(m_whatType, check) != toReplace)
				break;

			int bitID = ((check.x / 2) * map->width()) + check.y;
			if (checked.testBit(bitID))
				break;
			checked.setBit(bitID);

			CEditCommand::Change change;
			change.before = toReplace;
			change.after = m_whatValue;
			change.type = m_whatType;
			change.pos = check;
			m_command->changes.append(change);

			// Check if we need to add anything SW/NE of this one
			CMapPoint checkSW = check;
			checkSW.moveSW();
			if (map->positionValid(checkSW) && map->get(m_whatType, checkSW) == toReplace)
				queue.enqueue(checkSW);

			CMapPoint checkNE = check;
			checkNE.moveNE();
			if (map->positionValid(checkNE) && map->get(m_whatType, checkNE) == toReplace)
				queue.enqueue(checkNE);
		}
	}*/
}
