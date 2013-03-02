#include "cmapwidget.h"
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>

CMapWidget::CMapWidget(QWidget *parent) :
	QWidget(parent), m_map(0), m_scrollArea(0),
	m_zoomFactorX(1.0f), m_zoomFactorY(1.0f),
	m_currentTool(0),
	isHovering(false), m_isDraggingMap(false) {

	//setAttribute(Qt::WA_OpaquePaintEvent);
	setMouseTracking(true);

	mouseMapImage.load(":/graphics/mousemap.png");
	highlightPixmap.load(":/graphics/highlight.png");
	highlightTransPixmap.load(":/graphics/highlight_trans.png");
}


static inline int screenXForPos(int x, int y) {
	return ((x * 64) + ((y & 1) ? 0 : 32)) - 2;
}
static inline int screenYForPos(int y) {
	return (y * 16) - 63;
}


void CMapWidget::paintShape(QPainter &painter, const QRect &drawRect, int x, int y, CShapePool &pool, int shapeNum, int xOffset, int yOffset) {
	if (shapeNum < 0)
		return;

	int screenX = screenXForPos(x, y) + xOffset;
	int screenY = screenYForPos(y) + yOffset;

	if (pool.exists(shapeNum)) {
		const CShape &shape = pool[shapeNum];
		const CFrame &frame = shape.frames.first();

		QRect shapeRect(
					screenX+frame.posX, screenY+frame.posY,
					frame.pixmap.width(), frame.pixmap.height());

		if (drawRect.intersects(shapeRect)) {
			painter.drawPixmap(screenX+frame.posX, screenY+frame.posY, frame.pixmap);
		}
	}
}


void CMapWidget::paintEvent(QPaintEvent *event) {
	if (!m_map || !m_patches)
		return;

	QPainter painter(this);
	if (m_zoomFactorX < 1.0f)
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.scale(m_zoomFactorX, m_zoomFactorY);

	const QRect &rawRect = event->rect();
	// scale the rect, todo: extract this into a method
	QRect rect(int(rawRect.x() / m_zoomFactorX),
			   int(rawRect.y() / m_zoomFactorY),
			   int(rawRect.width() / m_zoomFactorX),
			   int(rawRect.height() / m_zoomFactorY)
				);

	//painter.fillRect(rect, Qt::white);

	// for bounds checking, scale the rect
	int minX = qMax(0, (rect.x() / 64) - 8);
	int minY = qMax(0, (rect.y() / 16) - 16);
	int maxX = qMin(m_map->width(), (rect.right() / 64) + 8);
	int maxY = qMin(m_map->height(), (rect.bottom() / 16) + 16);

	// This code is a mess, I know.
	CMap::ObjectType toPreview = m_currentTool->typesToPreview();

	// Floors layer
	for (int y = minY; y < maxY; y++) {
		for (int x = minX; x < maxX; x++) {
			int floorNum = m_map->floors[x][y];
			if (toPreview & CMap::Floor)
				floorNum = m_currentTool->whatThingFor(CMap::Floor, x, y, floorNum);
			floorNum = m_map->randomisedFloor(x, y, floorNum);

			paintShape(painter, rect, x, y, m_patches->floors, floorNum);
		}
	}

	// Regions layer
	for (int y = minY; y < maxY; y++) {
		for (int x = minX; x < maxX; x++) {
			int regionNum = m_map->regions[x][y];
			if (toPreview & CMap::Region)
				regionNum = m_currentTool->whatThingFor(CMap::Region, x, y, regionNum);

			if (regionNum > 0)
				paintShape(painter, rect, x, y, m_patches->regions, m_patches->regionIndexer.imageNumForShape(regionNum));
		}
	}

	if (isHovering) {
		painter.drawPixmap(
					screenXForPos(hovered.x / 2, hovered.y) + 1,
					screenYForPos(hovered.y) + 63,
					highlightPixmap);
	}

	// Everything else
	for (int y = minY; y < maxY; y++) {
		for (int x = minX; x < maxX; x++) {
			int leftWall = m_map->walls[x*2][y];
			if (toPreview & CMap::LeftWall)
				leftWall = m_currentTool->whatThingFor(CMap::LeftWall, x*2, y, leftWall);

			if (leftWall > 0)
				paintShape(painter, rect, x, y, m_patches->walls, m_patches->wallIndexer.imageNumForShape(leftWall) + 1, -16, -8);


			int rightWall = m_map->walls[x*2+1][y];
			if (toPreview & CMap::RightWall)
				rightWall = m_currentTool->whatThingFor(CMap::RightWall, x*2+1, y, rightWall);

			if (rightWall > 0)
				paintShape(painter, rect, x, y, m_patches->walls, m_patches->wallIndexer.imageNumForShape(rightWall), 16, -8);


			int itemNum = m_map->items[x][y];
			if (toPreview & CMap::Item)
				itemNum = m_currentTool->whatThingFor(CMap::Item, x, y, itemNum);
			itemNum = m_map->randomisedItem(x, y, itemNum);

			if (itemNum > 0)
				paintShape(painter, rect, x, y, m_patches->items, itemNum);


			int effectNum = m_map->effects[x][y];
			if (toPreview & CMap::Effect)
				effectNum = m_currentTool->whatThingFor(CMap::Effect, x, y, effectNum);

			if (effectNum > 0)
				paintShape(painter, rect, x, y, m_patches->effects, effectNum);
		}
	}

	if (isHovering) {
		painter.drawPixmap(
					screenXForPos(hovered.x / 2, hovered.y) + 1,
					screenYForPos(hovered.y) + 63,
					highlightTransPixmap);
	}

	// Now, selections
	if (m_map->hasSelection) {
		for (int y = minY; y < maxY; y++) {
			int base = y * m_map->width();

			for (int x = minX; x < maxX; x++) {
				if (m_map->selectionMask.testBit(base + x)) {
					painter.drawPixmap(
								screenXForPos(x, y) + 1,
								screenYForPos(y) + 63,
								highlightTransPixmap);
				}
			}
		}
	}
}


void CMapWidget::updateTile(int x, int y, CEditableMap::UpdateType type) {
	if (m_map == 0)
		return;

	// This is a bit messy and not very DRY ...
	int screenX = screenXForPos(x, y);
	int screenY = screenYForPos(y);

	CMap::ObjectType toPreview = m_currentTool->typesToPreview();

	if (type & CEditableMap::HighlightUpdate) {
		updateWorld(screenX + 2, screenY + 63, 62, 32);
	}

	if (type & CEditableMap::FloorUpdate) {
		int num = m_map->floors[x][y];
		if (toPreview & CMap::Floor)
			num = m_currentTool->whatThingFor(CMap::Floor, x, y, num);
		num = m_map->randomisedFloor(x, y, num);

		if (m_patches->floors.exists(num)) {
			const CShape &shape = m_patches->floors[num];
			const CFrame &frame = shape.frames.first();

			updateWorld(screenX + frame.posX, screenY + frame.posY,
				   frame.pixmap.width(), frame.pixmap.height());
		}
	}

	if (type & CEditableMap::ItemUpdate) {
		int num = m_map->items[x][y];
		if (toPreview & CMap::Item)
			num = m_currentTool->whatThingFor(CMap::Item, x, y, num);
		num = m_map->randomisedItem(x, y, num);

		if (num > 0 && m_patches->items.exists(num)) {
			const CShape &shape = m_patches->items[num];
			const CFrame &frame = shape.frames.first();

			updateWorld(screenX + frame.posX, screenY + frame.posY,
				   frame.pixmap.width(), frame.pixmap.height());
		}
	}

	if (type & CEditableMap::LeftWallUpdate) {
		int num = m_map->walls[x*2][y];
		if (toPreview & CMap::LeftWall)
			num = m_currentTool->whatThingFor(CMap::LeftWall, x*2, y, num);

		num = m_patches->wallIndexer.imageNumForShape(num) + 1;

		if (num >= 1 && m_patches->walls.exists(num)) {
			const CShape &shape = m_patches->walls[num];
			const CFrame &frame = shape.frames.first();

			updateWorld(screenX + frame.posX - 16, screenY + frame.posY - 8,
				   frame.pixmap.width(), frame.pixmap.height());
		}
	}

	if (type & CEditableMap::RightWallUpdate) {
		int num = m_map->walls[x*2+1][y];
		if (toPreview & CMap::RightWall)
			num = m_currentTool->whatThingFor(CMap::RightWall, x*2+1, y, num);

		num = m_patches->wallIndexer.imageNumForShape(num);

		if (num >= 0 && m_patches->walls.exists(num)) {
			const CShape &shape = m_patches->walls[num];
			const CFrame &frame = shape.frames.first();

			updateWorld(screenX + frame.posX + 16, screenY + frame.posY - 8,
				   frame.pixmap.width(), frame.pixmap.height());
		}
	}

	if (type & CEditableMap::RegionUpdate) {
		int num = m_map->regions[x][y];
		if (toPreview & CMap::Region)
			num = m_currentTool->whatThingFor(CMap::Region, x, y, num);

		num = m_patches->regionIndexer.imageNumForShape(num);

		if (num >= 0 && m_patches->regions.exists(num)) {
			const CShape &shape = m_patches->regions[num];
			const CFrame &frame = shape.frames.first();

			updateWorld(screenX + frame.posX, screenY + frame.posY,
				   frame.pixmap.width(), frame.pixmap.height());
		}
	}

	if (type & CEditableMap::EffectUpdate) {
		int num = m_map->effects[x][y];
		if (toPreview & CMap::Effect)
			num = m_currentTool->whatThingFor(CMap::Effect, x, y, num);

		if (num > 0 && m_patches->effects.exists(num)) {
			const CShape &shape = m_patches->effects[num];
			const CFrame &frame = shape.frames.first();

			updateWorld(screenX + frame.posX, screenY + frame.posY,
				   frame.pixmap.width(), frame.pixmap.height());
		}
	}
}


void CMapWidget::setTool(CToolBase *tool) {
	if (m_currentTool) {
		m_currentTool->deactivate();
	}

	m_currentTool = tool;
	tool->activate();
}


void CMapWidget::updateWorld(int x, int y, int w, int h) {
	int left = int(x * m_zoomFactorX);
	int right = int((x + w) * m_zoomFactorX);

	int top = int(y * m_zoomFactorY);
	int bottom = int((y + h) * m_zoomFactorY);

	update(left, top, (right-left+1), (bottom-top+1));
}


void CMapWidget::setZoomLevel(float x, float y) {
	m_zoomFactorX = x;
	m_zoomFactorY = y;

	fixWidgetSize();
	update();
}


void CMapWidget::fixWidgetSize() {
	if (m_map == 0)
		return;

	resize(((m_map->width()*64)+32)*m_zoomFactorX, ((m_map->height()*16)+16)*m_zoomFactorY);
}


void CMapWidget::setMap(CEditableMap *newMap) {
	// unlink the old signals first
	if (m_map)
		disconnect(0, m_map, 0);

	m_map = newMap;
	m_patches = 0;
	fixWidgetSize();
	update();

	connect(newMap, SIGNAL(updateTile(const CMapPoint&,CEditableMap::UpdateType)),
			SLOT(updateTile(const CMapPoint&,CEditableMap::UpdateType)), Qt::DirectConnection);
}


void CMapWidget::setPatches(CPatchContext *newPatches) {
	m_patches = newPatches;
	update();
}




CMapPoint CMapWidget::calculateClickedPosition(const QPoint &rawScreenPos) {
	int screenX = (rawScreenPos.x() / m_zoomFactorX) + 1;
	int screenY = (rawScreenPos.y() / m_zoomFactorY);

	CMapPoint pos;

	if (screenX < 0 || screenY < 0) {
		pos.x = -1;
		pos.y = -1;
	} else {
		pos.x = ((screenX / 64) * 2) - 2;
		pos.y = ((screenY / 32) * 2);

		QRgb check = mouseMapImage.pixel(screenX % 64, screenY % 32);

		// Black: NE left
		if (check == qRgb(0, 0, 0)) { pos.x += 2; pos.y--; }
		// Red: NE right
		else if (check == qRgb(255, 0, 0)) { pos.x += 3; pos.y--; }
		// White: This tile right
		else if (check == qRgb(255, 255, 255)) { pos.x += 1; }
		// Blue: E left
		else if (check == qRgb(0, 0, 255)) { pos.x += 2; }
		// Green: SE left
		else if (check == qRgb(0, 255, 0)) { pos.x += 2; pos.y += 1; }
		// Yellow: SE right
		else if (check == qRgb(255, 255, 0)) { pos.x += 3; pos.y += 1; }
	}

	return pos;
}

void CMapWidget::mouseMoveEvent(QMouseEvent *event) {
	if (!m_map || !m_patches)
		return;

	event->accept();

	// Figure out the hovered position
	const QPoint &rawScreenPos = event->pos();
	CMapPoint newHover = calculateClickedPosition(rawScreenPos);

	// If it hasn't changed, do nothing!
	if (!(hovered == newHover)) {
		// Mark the old position as modified, if it was valid
		if (isHovering)
			updateTile(hovered, CEditableMap::HighlightUpdate);

		bool newFlag = m_map->positionValid(newHover);

		// before we clobber "hovered", call the tool's hoverstatus
		m_currentTool->hoverStatusChanged(isHovering, hovered, newFlag, newHover, event);

		// now update + write the new position's info
		isHovering = newFlag;
		hovered = newHover;

		if (newFlag)
			updateTile(newHover, CEditableMap::HighlightUpdate);
	}



	// Now, do drag processing!

	if (m_isDraggingMap && m_scrollArea) {
		// translate this position to the one clicked on-screen,
		// not the position in "world space" ... this is a bad explanation
		// oh well
		QPoint scrollOffset = QPoint(m_scrollArea->horizontalScrollBar()->value(), m_scrollArea->verticalScrollBar()->value());
		QPoint newPos = rawScreenPos - scrollOffset;

		// now that they're both on the same base, the delta is easy to figure
		// out
		QPoint delta = newPos - m_dragStart;
		delta *= 1.5f;
		m_roughDragDistance += delta.manhattanLength();

		QPoint newOffset = scrollOffset - delta;

		m_scrollArea->horizontalScrollBar()->setValue(newOffset.x());
		m_scrollArea->verticalScrollBar()->setValue(newOffset.y());

		m_dragStart = newPos;
	}
}

void CMapWidget::mousePressEvent(QMouseEvent *event) {
	if (!m_map || !m_patches)
		return;

	event->accept();

	CMapPoint clicked = calculateClickedPosition(event->pos());
	if (m_map->positionValid(clicked))
		m_currentTool->tileMousePress(clicked, event);

	if (event->button() == Qt::RightButton) {
		m_isDraggingMap = true;

		QPoint scrollOffset = QPoint(m_scrollArea->horizontalScrollBar()->value(), m_scrollArea->verticalScrollBar()->value());
		m_dragStart = event->pos() - scrollOffset;
		m_roughDragDistance = 0;
	}
}

void CMapWidget::mouseReleaseEvent(QMouseEvent *event) {
	if (!m_map || !m_patches)
		return;

	event->accept();

	CMapPoint clicked = calculateClickedPosition(event->pos());
	if (m_map->positionValid(clicked)) {
		m_currentTool->tileMouseRelease(clicked, event);
	} else {
		// call this whether it's valid or not
		m_currentTool->tileMouseRelease(CMapPoint(9999, 9999), event);
	}

	if (event->button() == Qt::RightButton) {
		m_isDraggingMap = false;

		if (m_roughDragDistance < 6 && m_map->positionValid(clicked))
			m_currentTool->tileMouseAltAction(clicked, event);
	}
}

void CMapWidget::leaveEvent(QEvent *) {
	if (!m_map || !m_patches)
		return;

	CMapPoint nullp(9999, 9999);

	m_currentTool->hoverStatusChanged(isHovering, hovered, false, nullp, 0);

	isHovering = false;
	updateTile(hovered, CEditableMap::HighlightUpdate);
	hovered = nullp;
}
