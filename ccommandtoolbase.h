#ifndef CCOMMANDTOOLBASE_H
#define CCOMMANDTOOLBASE_H

#include "ctoolbase.h"
#include "ceditcommand.h"

class CCommandToolBase : public CToolBase {
	Q_OBJECT
public:
	explicit CCommandToolBase(QObject *parent = 0);

	void tileMousePress(const CMapPoint &tile, QMouseEvent *event);
	void tileMouseRelease(const CMapPoint &tile, QMouseEvent *event);
	void deactivate();

	bool commandActive() const { return (m_command != 0); }

protected:
	CEditCommand *m_command;

	virtual void finaliseCommand();

signals:
	
public slots:
	
};

#endif // CCOMMANDTOOLBASE_H
