#include "cresizedreamdialog.h"
#include "ui_cresizedreamdialog.h"
#include "cmap.h"

CResizeDreamDialog::CResizeDreamDialog(CMap *map, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CResizeDreamDialog)
{
	m_map = map;
	ui->setupUi(this);

	ui->widthBox->setValue(m_map->width() * 2);
	ui->heightBox->setValue(m_map->height());
}

CResizeDreamDialog::~CResizeDreamDialog() {
	delete ui;
}

bool CResizeDreamDialog::applyChanges() {
	int newWidth = ui->widthBox->value() / 2;
	int newHeight = ui->heightBox->value();

	if (newWidth == m_map->width() && newHeight == m_map->height())
		return false;

	m_map->resize(newWidth, newHeight);
	return true;
}
