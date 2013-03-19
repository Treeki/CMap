#ifndef CRESIZEDREAMDIALOG_H
#define CRESIZEDREAMDIALOG_H

#include <QDialog>

namespace Ui {
class CResizeDreamDialog;
}

class CMap;

class CResizeDreamDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit CResizeDreamDialog(CMap *map, QWidget *parent = 0);
	~CResizeDreamDialog();

	bool applyChanges();
	
private:
	CMap *m_map;
	Ui::CResizeDreamDialog *ui;
};

#endif // CRESIZEDREAMDIALOG_H
