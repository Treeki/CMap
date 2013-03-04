#include <QApplication>
#include "ceditorwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CEditorWindow w;
	w.show();
	
	return a.exec();
}
