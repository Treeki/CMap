#ifndef CSHAPEITEMDELEGATE_H
#define CSHAPEITEMDELEGATE_H

#include <QStyledItemDelegate>

class CShapeItemDelegate : public QStyledItemDelegate {
	Q_OBJECT
public:
	explicit CShapeItemDelegate(QObject *parent = 0);
	
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
	
public slots:
	
};

#endif // CSHAPEITEMDELEGATE_H
