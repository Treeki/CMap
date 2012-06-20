#ifndef CEDITCOMMAND_H
#define CEDITCOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QVector>
#include <cmap.h>

class CEditableMap;

class CEditCommand : public QUndoCommand {
public:
	struct Change {
		CMap::ObjectType type;
		CMapPoint pos;
		quint16 before, after;
	};

	CEditCommand(CEditableMap *map);

	void undo();
	void redo();

	QVector<Change> changes;
	
private:
	CEditableMap *m_map;
};

Q_DECLARE_TYPEINFO(CEditCommand::Change, Q_PRIMITIVE_TYPE);

#endif // CEDITCOMMAND_H
