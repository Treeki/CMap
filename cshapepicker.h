#ifndef CSHAPEPICKER_H
#define CSHAPEPICKER_H

#include <QWidget>
#include "cmap.h"
#include "cshapemodel.h"

class QSpinBox;

class QListView;

class CShapePicker : public QWidget {
	Q_OBJECT
public:
	explicit CShapePicker(CMap::ObjectType objectType, QWidget *parent = 0);

	void setup(CShapeIndexer *indexer);

protected:
	CShapeModel *m_model;
	CShapeIndexer *m_indexer;

	QSpinBox *m_typeSpinBox;
	QListView *m_view;

	CMap::ObjectType m_objectType;
	
public:
	CMap::ObjectType objectType() const { return m_objectType; }

	int selectedItem() const;
public slots:
	void setSelectedShape(int newShape);
signals:
	void selectedItemChanged(int item);
	
protected slots:
	void handleRowChanged(const QModelIndex &current, const QModelIndex &previous);
};

#endif // CSHAPEPICKER_H
