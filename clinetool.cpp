#include "clinetool.h"
#include "cmapwidget.h"
#include "ceditcommand.h"

CLineTool::CLineTool(QObject *parent) : CCommandToolBase(parent) {
	m_whatType = CMap::Item;
	m_whatValue = 1;
}



CMap::ObjectType CLineTool::typesToPreview() const {
	return m_whatType;
}

int CLineTool::whatThingFor(CMap::ObjectType type, int x, int y, int normal) const {
	if (commandActive()) {
		if (m_line.contains(x * 2, y))
			return m_whatValue;
		else
			return normal;
	} else {
		if (x*2 == m_hovered.x && y == m_hovered.y)
			return m_whatValue;
		else
			return normal;
	}
}

void CLineTool::setWhat(CMap::ObjectType type, int value) {
	widget()->updateTile(m_hovered, (CEditableMap::UpdateType)m_whatType);
	m_whatType = type;
	m_whatValue = value;
	widget()->updateTile(m_hovered, (CEditableMap::UpdateType)m_whatType);
}


void CLineTool::updateLineOnWidget(CLine &line) {
	line.forEach([this](const CMapPoint &point) {
		if (widget()->map()->positionValid(point))
			widget()->updateTile(point, (CEditableMap::UpdateType)m_whatType);
	});
}


void CLineTool::hoverStatusChanged(bool fromValid, const CMapPoint &from, bool toValid, const CMapPoint &to, QMouseEvent *event) {
	// this is a bit weird: we can't actually use "from" because we might have
	// manipulated that position in a previous call!
	CMapPoint realFrom = m_hovered;
	CMapPoint realTo = to;

	// make sure we're always at an even position
	realTo.x &= ~1;

	if (fromValid)
		widget()->updateTile(realFrom, (CEditableMap::UpdateType)m_whatType);
	if (toValid)
		widget()->updateTile(realTo, (CEditableMap::UpdateType)m_whatType);

	m_hovered = realTo;

	if (fromValid)
		widget()->updateTile(realFrom, (CEditableMap::UpdateType)m_whatType);
	if (toValid) {
		widget()->updateTile(realTo, (CEditableMap::UpdateType)m_whatType);

		if (commandActive()) {
			// What's the nearest line we can snap to?
			CLine newLine;
			newLine.snapToPoints(m_dragBegin, realTo);
			if (!(m_line == newLine)) {
				updateLineOnWidget(m_line);
				updateLineOnWidget(newLine);
				CLine oldLine = m_line;
				m_line = newLine;
				updateLineOnWidget(oldLine);
				updateLineOnWidget(newLine);
			}
		}
	}
}


void CLineTool::tileMousePress(const CMapPoint &tile, QMouseEvent *event) {
	CCommandToolBase::tileMousePress(tile, event);

	if (commandActive()) {
		widget()->updateTile(tile, (CEditableMap::UpdateType)m_whatType);
		m_dragBegin = tile;
		m_dragBegin.clearRightWall();
		m_line.tryAssign(tile, tile);
		widget()->updateTile(tile, (CEditableMap::UpdateType)m_whatType);
	}
}


void CLineTool::tileMouseAltAction(const CMapPoint &tile, QMouseEvent *event) {
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


void CLineTool::finaliseCommand() {
	if (m_line.isValid()) {
		m_command->changes.reserve(m_line.size());

		m_line.forEach([this](const CMapPoint &point) {
			if (widget()->map()->positionValid(point)) {
				CEditCommand::Change chg;

				chg.pos = point;
				chg.before = widget()->map()->get(m_whatType, point);
				chg.after = m_whatValue;
				chg.type = m_whatType;

				m_command->changes.append(chg);
			}
		});

		int amount = m_command->changes.count();

		bool isPlacement = !((m_whatType != CMap::Floor) && m_whatValue == 0);

		QString text;
		const char *typeName = CMap::nameForObjectType(m_whatType);

		if (amount > 1) {
			text = QStringLiteral("%1 line: %2 %3s")
					.arg(isPlacement ? "place" : "delete")
					.arg(amount).arg(typeName);
		} else {
			text = QStringLiteral("%1 one %2")
					.arg(isPlacement ? "place" : "delete")
					.arg(typeName);
		}

		if (isPlacement)
			text.append(QStringLiteral(" of type %1").arg(m_whatValue));

		m_command->setText(text);
	}

	m_line.makeInvalid();

	CCommandToolBase::finaliseCommand();
}

