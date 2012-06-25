#include "cshapeitemdelegate.h"
#include <QPainter>

CShapeItemDelegate::CShapeItemDelegate(QObject *parent) :
	QStyledItemDelegate(parent) {
}


void CShapeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	bool sel = (option.state & QStyle::State_Selected);
	const QPalette &pal = option.palette;

	const QBrush &bgBrush = sel ? pal.highlight() : pal.window();
	const QBrush &fgBrush = sel ? pal.highlightedText() : pal.windowText();

	painter->fillRect(option.rect, bgBrush);

	QPixmap shape = index.model()->data(index, Qt::DecorationRole).value<QPixmap>();

	int placeWidth = option.rect.width();
	int placeHeight = option.rect.height();
	int displayX = option.rect.x();
	int displayY = option.rect.y();
	int displayWidth = shape.width();
	int displayHeight = shape.height();

	if (displayWidth > placeWidth)
		displayWidth = placeWidth;
	else
		displayX += ((placeWidth - displayWidth) / 2);

	if (displayHeight > placeHeight)
		displayHeight = placeHeight;
	else
		displayY += ((placeHeight - displayHeight) / 2);

	painter->drawPixmap(displayX, displayY, displayWidth, displayHeight, shape);

	// draw the text
	QString text = index.model()->data(index).toString();
	const QFontMetrics &fm = painter->fontMetrics();

	int textWidth = fm.width(text);
	painter->setOpacity(0.5);
	painter->fillRect(option.rect.x(), option.rect.y(), textWidth + 4, fm.height() + 2, bgBrush);

	painter->setOpacity(1.0);
	painter->setPen(fgBrush.color());
	painter->drawText(option.rect.x() + 2, option.rect.y() + 2 + fm.ascent(), text);
}
