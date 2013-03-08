#include <QApplication>
#include "ceditorwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("Treeki");
	a.setApplicationName("CMap");

	CEditorWindow w;
	w.show();
	
	return a.exec();
}
