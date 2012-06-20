#include "cundostack.h"

CUndoStack::CUndoStack(QObject *parent) :
	QObject(parent), m_forcedDirty(false),
	m_lastCleanEmitted(true) {

	m_baseStack = new QUndoStack(this);
	connect(m_baseStack, SIGNAL(cleanChanged(bool)), SLOT(handleCleanChanged(bool)));
}

void CUndoStack::setClean() {
	m_forcedDirty = false;
	m_baseStack->setClean();
}

void CUndoStack::makeUnfixablyDirty() {
	m_forcedDirty = true;
	tryEmitCleanChanged();
}


// TOOD: redo
/*void CUndoStack::makeReallyDirty(const QString &description) {
	int saveLimit = m_baseStack->undoLimit();
	m_baseStack->clear();
	// TODO: try screwing with UndoView to disable icon or something
	m_baseStack->setUndoLimit(1);
	push(new QUndoCommand("dummy"));
	push(new QUndoCommand(description));
	m_baseStack->setUndoLimit(saveLimit);
}*/

void CUndoStack::handleCleanChanged(bool clean) {
	(void)clean;
	tryEmitCleanChanged();
}

void CUndoStack::tryEmitCleanChanged() {
	bool ic = isClean();
	if (ic != m_lastCleanEmitted) {
		m_lastCleanEmitted = ic;
		emit cleanChanged(ic);
	}
}
