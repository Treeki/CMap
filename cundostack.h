#ifndef CUNDOSTACK_H
#define CUNDOSTACK_H

#include <QUndoStack>

// this is terrible what am I doing

class CUndoStack : public QObject {
	Q_OBJECT
public:
	explicit CUndoStack(QObject *parent = 0);

	QUndoStack *baseStack() { return m_baseStack; }

	bool isClean() const {
		return m_forcedDirty ? false : m_baseStack->isClean();
	}

private slots:
	void handleCleanChanged(bool clean);
	
signals:
	void cleanChanged(bool clean);

public:
	void push(QUndoCommand *cmd) { m_baseStack->push(cmd); }

public slots:
	void undo() { m_baseStack->undo(); }
	void redo() { m_baseStack->redo(); }

	void setClean();

	// basic change that can't be undone, so the stack will always be "dirty"
	// but doesn't affect any changes that ARE undoable
	// (for example, a dream settings change won't affect the map)
	void makeUnfixablyDirty();

	// major change that requires the undo stack to be lost entirely
	// TODO: remove
	//void makeReallyDirty(const QString &description);

private:
	QUndoStack *m_baseStack;
	bool m_forcedDirty;

	bool m_lastCleanEmitted;
	void tryEmitCleanChanged();
};

#endif // CUNDOSTACK_H
