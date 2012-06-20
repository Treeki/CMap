#ifndef CMAPWIDGET_H
#define CMAPWIDGET_H

#include <QWidget>
#include <QAbstractScrollArea>
#include "ceditablemap.h"
#include "cpatchcontext.h"
#include "ctoolbase.h"

class CMapWidget : public QWidget {
	Q_OBJECT
public:
	explicit CMapWidget(QWidget *parent = 0);

private:
	CEditableMap *m_map;
	CPatchContext *m_patches;

	QAbstractScrollArea *m_scrollArea;

public:
	void setScrollArea(QAbstractScrollArea *scrollArea) { m_scrollArea = scrollArea; }

	CEditableMap *map() const { return m_map; }
	void setMap(CEditableMap *newMap);

	CPatchContext *patches() const { return m_patches; }
	void setPatches(CPatchContext *newPatches);

protected:
	void paintEvent(QPaintEvent *);
	void paintShape(QPainter &painter, const QRect &drawRect, int x, int y, CShapePool &pool, int shapeNum, int xOffset = 0, int yOffset = 0);

	// Updating
public slots:
	void updateTile(int x, int y, CEditableMap::UpdateType type = CEditableMap::EverythingUpdate);
	void updateTile(const CMapPoint &pos, CEditableMap::UpdateType type = CEditableMap::EverythingUpdate) {
		if (type & CEditableMap::WallUpdate) {
			type = (CEditableMap::UpdateType)(type & ~CEditableMap::WallUpdate);
			type = (CEditableMap::UpdateType)(type | ((pos.x & 1) ? CEditableMap::RightWallUpdate : CEditableMap::LeftWallUpdate));
		}

		updateTile(pos.x / 2, pos.y, type);
	}

	// Zoom
private:
	float m_zoomFactorX, m_zoomFactorY;

public:
	void setZoomLevel(float x, float y);
	void setZoomLevel(float level) {
		setZoomLevel(level, level);
	}

public slots:
	void updateWorld(int x, int y, int w, int h);

public:
	void fixWidgetSize();

	// Editing
protected:
	CToolBase *m_currentTool;
public:
	void setTool(CToolBase *tool);

	QImage mouseMapImage;
	QPixmap highlightPixmap, highlightTransPixmap;

	bool isHovering;
	CMapPoint hovered;

protected:
	CMapPoint calculateClickedPosition(const QPoint &rawScreenPos);

	bool m_isDraggingMap;
	QPoint m_dragStart;
	float m_roughDragDistance;

protected:
	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void leaveEvent(QEvent *);

signals:
	
public slots:
	
};

#endif // CMAPWIDGET_H
