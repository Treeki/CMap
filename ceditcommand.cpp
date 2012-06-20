#include "ceditcommand.h"
#include "ceditablemap.h"

CEditCommand::CEditCommand(CEditableMap *map) : m_map(map) {
}


void CEditCommand::undo() {
	foreach (const Change &ch, changes) {
		m_map->modify(ch.type, ch.pos, ch.before);
	}
}

void CEditCommand::redo() {
	for (int i = (changes.count() - 1); i >= 0; i--) {
		const Change &ch = changes.at(i);
		m_map->modify(ch.type, ch.pos, ch.after);
	}
}
