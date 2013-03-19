#include "caboutdialog.h"
#include "ui_caboutdialog.h"

CAboutDialog::CAboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CAboutDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
}

CAboutDialog::~CAboutDialog()
{
	delete ui;
}
