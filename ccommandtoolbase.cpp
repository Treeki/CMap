#include "ccommandtoolbase.h"
#include "cmapwidget.h"

CCommandToolBase::CCommandToolBase(QObject *parent) :
	CToolBase(parent),
	m_command(0) {
}


void CCommandToolBase::tileMousePress(const CMapPoint &tile, QMouseEvent *event) {
	if (event->button() == Qt::LeftButton && m_command == 0) {
		m_command = new CEditCommand(widget()->map());
	}

	CToolBase::tileMousePress(tile, event);
}

void CCommandToolBase::tileMouseRelease(const CMapPoint &tile, QMouseEvent *event) {
	if (event->button() == Qt::LeftButton && m_command)
		finaliseCommand();
}

void CCommandToolBase::deactivate() {
	if (m_command)
		finaliseCommand();
}

void CCommandToolBase::finaliseCommand() {
	if (m_command->changes.empty())
		delete m_command;
	else
		widget()->map()->undo.push(m_command);
	m_command = 0;
}
