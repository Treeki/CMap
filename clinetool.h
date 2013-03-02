#ifndef CLINETOOL_H
#define CLINETOOL_H

#include "ccommandtoolbase.h"
#include <QHash>

class CLineTool : public CCommandToolBase {
	Q_OBJECT
public:
	explicit CLineTool(QObject *parent = 0);

private:
	CMap::ObjectType m_whatType;
	int m_whatValue;

	CMapPoint m_hovered;
	CLine m_line;
	CMapPoint m_dragBegin;

	void updateLineOnWidget(CLine &line);

public:
	void tileMousePress(const CMapPoint &tile, QMouseEvent *event);
	void tileMouseAltAction(const CMapPoint &tile, QMouseEvent *event);

public slots:
	void setWhat(CMap::ObjectType type, int value);
signals:
	void shapePicked(int newValue);

public:
	CMap::ObjectType whatType() const { return m_whatType; }
	int whatValue() const { return m_whatValue; }

	int whatThingFor(CMap::ObjectType type, int x, int y, int normal) const;

	CMap::ObjectType typesToPreview() const;

	void hoverStatusChanged(bool fromValid, const CMapPoint &from, bool toValid, const CMapPoint &to, QMouseEvent *event);

protected:
	void finaliseCommand();

signals:

public slots:

};

#endif // CLINETOOL_H
