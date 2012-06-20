#include "utility.h"
#include <QDirIterator>

#if defined Q_WS_WIN
#define cDoNothing
#elif defined Q_WS_X11 || Q_WS_MAC
#define cFixRegular
#else
#error "Unknown OS for path manipulation (utility.cpp)"
#endif


bool cCaseInsensitiveCd(QDir &dir, const QString &dirName) {
	if (!dir.cd(dirName)) {
#ifdef cFixRegular
		// I'm making an assumption that every non-Windows OS CMap is compiled
		// for (those that use cFixRegular) will use the typical /-separated
		// paths. If not, oh well... no CMap on Haiku or Mac OS 9, I guess :(

		QStringList pieces = dirName.split('/');

		foreach (const QString &piece, pieces) {
			if (!dir.cd(piece)) {
				// let's try to find it manually
				QDirIterator di(dir);
				bool foundThis = false;

				while (di.hasNext()) {
					di.next();
					if (di.fileName().compare(piece, Qt::CaseInsensitive) == 0) {
						dir.cd(di.fileName());
						foundThis = true;
						break;
					}
				}

				if (!foundThis)
					return false;
			}
		}

		// well, we got there in the end
#endif
	}

	return true;
}


QString cCaseInsensitiveFilePath(QDir &dir, const QString &fileName) {
	// Note: this doesn't take into account hierarchical paths!
	if (dir.exists(fileName)) {
		return dir.filePath(fileName);
	} else {
		// search the thing
		QDirIterator di(dir);

		while (di.hasNext()) {
			di.next();
			if (di.fileName().compare(fileName, Qt::CaseInsensitive) == 0)
				return di.filePath();
		}

		return QString();
	}
}


/*QString cFixCaseInsensitiveFilePath(const QString &origPath, bool *didWork) {
	if (didWork) *didWork = false;

	if (QFile::exists(origPath)) {
		if (didWork) *didWork = true;
		return origPath;
	}

	// now, try to normalise it...
#ifdef cFixRegular
	// search for the path
	QStringList pieces = origPath.split('/');

	QDir dir;
	if (origPath.at(0) == '/') {
		// absolute path
		dir = QDir::root();
		pieces.removeFirst();
	}

	QString filename = pieces.takeLast();

	foreach (const QString &piece, pieces) {
		if (!dir.cd(piece)) {
			// oops, let's try to find it manually!
			QDirIterator di(dir);
			bool foundThis = false;

			while (di.hasNext()) {
				di.next();
				if (di.fileName().compare(piece, Qt::CaseInsensitive) == 0) {
					dir.cd(di.fileName());
					foundThis = true;
					break;
				}
			}

			if (!foundThis) {
				return QString();
			}
		}
	}

	// if we got here, it means we found the correct dir!
	if (dir.exists(filename)) {
		// that was easy. nothing to mess with.
		if (didWork) *didWork = true;
		return dir.filePath(filename);

	} else {
		// search the directory
		QDirIterator di(dir);
		bool foundThis = false;

		while (di.hasNext()) {
			di.next();
			if (di.fileName().compare(filename, Qt::CaseInsensitive) == 0) {
				// got it
				if (didWork) *didWork = true;
				return di.filePath();
			}
		}

		// nothing there
	}
#endif

	return QString();
}*/
