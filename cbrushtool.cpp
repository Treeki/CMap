#include "cbrushtool.h"
#include "cmapwidget.h"
#include "ceditcommand.h"

CBrushTool::CBrushTool(QObject *parent) : CCommandToolBase(parent) {
	m_whatType = CMap::Item;
	m_whatValue = 1;
	m_hovered.x = -1;
	m_hovered.y = -1;
}



CMap::ObjectType CBrushTool::typesToPreview() {
	return m_whatType;
}

int CBrushTool::whatItemFor(int x, int y, int normal) {
	if (x == m_hoveredHalfX && y == m_hovered.y)
		return m_whatValue;
	else
		return m_paintedSoFar.value(CMap::positionKey(x * 2, y), normal);
}

int CBrushTool::whatFloorFor(int x, int y, int normal) {
	if (x == m_hoveredHalfX && y == m_hovered.y)
		return m_whatValue;
	else
		return m_paintedSoFar.value(CMap::positionKey(x * 2, y), normal);
}

int CBrushTool::whatWallFor(int x, int y, int normal) {
	if (x == m_hovered.x && y == m_hovered.y)
		return m_whatValue;
	else
		return m_paintedSoFar.value(CMap::positionKey(x, y), normal);
}

int CBrushTool::whatRegionFor(int x, int y, int normal) {
	if (x == m_hoveredHalfX && y == m_hovered.y)
		return m_whatValue;
	else
		return m_paintedSoFar.value(CMap::positionKey(x * 2, y), normal);
}

int CBrushTool::whatEffectFor(int x, int y, int normal) {
	if (x == m_hoveredHalfX && y == m_hovered.y)
		return m_whatValue;
	else
		return m_paintedSoFar.value(CMap::positionKey(x * 2, y), normal);
}


void CBrushTool::setWhat(CMap::ObjectType type, int value) {
	m_whatType = type;
	m_whatValue = value;
	// TODO: figure out if we need to refresh anything here?
}


void CBrushTool::hoverStatusChanged(bool fromValid, const CMapPoint &from, bool toValid, const CMapPoint &to, QMouseEvent *event) {
	// this is a bit weird: we can't actually use "from" because we might have
	// manipulated that position in a previous call!
	CMapPoint realFrom = m_hovered;
	CMapPoint realTo = to;

	if (m_whatType & CMap::Wall) {
		// if we're currently painting a wall, then force the hovered position
		// to face the same direction as the painted walls
		if (commandActive())
			realTo.x = (realTo.x & ~1) | m_wallPaintDirection;
	} else {
		// otherwise, make sure we're always at an even position
		realTo.x &= ~1;
	}

	if (fromValid)
		widget()->updateTile(realFrom, (CEditableMap::UpdateType)m_whatType);
	if (toValid)
		widget()->updateTile(realTo, (CEditableMap::UpdateType)m_whatType);

	m_hovered = realTo;
	m_hoveredHalfX = realTo.x / 2;

	if (fromValid)
		widget()->updateTile(realFrom, (CEditableMap::UpdateType)m_whatType);
	if (toValid) {
		widget()->updateTile(realTo, (CEditableMap::UpdateType)m_whatType);

		if (commandActive()) {
			// TODO: refactor this so the same code is used here and in
			// tileMousePress

			int currentValue = widget()->map()->get(m_whatType, m_hovered);
			currentValue = m_paintedSoFar.value(CMap::positionKey(m_hovered), currentValue);

			if (currentValue != m_whatValue)
				m_paintedSoFar.insert(CMap::positionKey(m_hovered), m_whatValue);
		}
	}
}


void CBrushTool::tileMousePress(const CMapPoint &tile, QMouseEvent *event) {
	CCommandToolBase::tileMousePress(tile, event);

	if (commandActive()) {
		int canonicalX = (m_whatType & CMap::Wall) ? tile.x : (tile.x & ~1);

		m_wallPaintDirection = tile.x & 1;

		// figure out what's there now
		// if it already has the type being painted, then don't touch it!
		int currentValue = widget()->map()->get(m_whatType, tile);
		currentValue = m_paintedSoFar.value(CMap::positionKey(canonicalX, tile.y), currentValue);

		if (currentValue != m_whatValue)
			m_paintedSoFar.insert(CMap::positionKey(canonicalX, tile.y), m_whatValue);
	}
}


void CBrushTool::finaliseCommand() {
	if (m_paintedSoFar.size() > 0) {
		m_command->changes.reserve(m_paintedSoFar.size());

		QHash<int, int>::const_iterator i;
		for (i = m_paintedSoFar.constBegin(); i != m_paintedSoFar.constEnd(); i++) {
			CEditCommand::Change chg;

			chg.pos.x = i.key() >> 12;
			chg.pos.y = i.key() & ((1 << 12) - 1);

			chg.before = widget()->map()->get(m_whatType, chg.pos);
			chg.after = m_whatValue;

			chg.type = m_whatType;

			m_command->changes.append(chg);
		}

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

		m_paintedSoFar.clear();
	}

	CCommandToolBase::finaliseCommand();
}
