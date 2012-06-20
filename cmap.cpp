#include "cmap.h"
#include <QRegExp>
#include <QMap>
#include <QFile>
#include "cpatchcontext.h"
#include "utility.h"

CMap::CMap(QObject *parent, int width, int height) :
	QObject(parent),
	m_available(false),
	m_width(0),
	m_height(0),
	patchType(NoPatch),
	isEncrypted(false),
	revision(0),
	standard(TeenStandard),
	allowSummon(true),
	allowFollow(true),
	allowURLs(true),
	enableFilter(false),
	parentalWarning(false),
	restrictSitting(false),
	allowShouting(true),
	allowWho(true),
	allowTab(true),
	allowSeasonals(true),
	hasGroupPackage(false),
	m_error(NoError) {

	resize(width, height);
}


void CMap::resize(int newWidth, int newHeight) {
	items.resize(newWidth, newHeight);
	floors.resize(newWidth, newHeight);
	regions.resize(newWidth, newHeight);
	effects.resize(newWidth, newHeight);
	walls.resize(newWidth*2, newHeight);

	m_width = newWidth;
	m_height = newHeight;
	m_available = (newWidth > 0) && (newHeight > 0);
}


int CMap::get(CMap::ObjectType type, const CMapPoint &pos) {
	switch (type) {
	case CMap::Item:
		return items.get(pos);
	case CMap::Floor:
		return floors.get(pos);
	case CMap::LeftWall: case CMap::RightWall: case CMap::Wall:
		return walls.get(pos);
	case CMap::Region:
		return regions.get(pos);
	case CMap::Effect:
		return effects.get(pos);
	case CMap::NoObject: break;
	}
	return -1;
}


static const char *StandardNames[6] = {
	"Everyone 8+",
	"Teen+",
	"Mature 16+",
	"Adult 18+",
	"Adults Only",
	"AOClean"
};
static const int StandardCount = 6;


bool CMap::load(QString filePath) {
	m_error = NoError;

	QFile file(filePath);
	if (file.open(QFile::ReadOnly)) {
		QDataStream stream(&file);
		return load(stream);
	} else {
		m_error = CannotOpenFileError;
		return false;
	}
}

bool CMap::load(QDataStream &stream) {
	stream.setByteOrder(QDataStream::LittleEndian);

	QIODevice &device = *stream.device();

	// Validate the MAP V... line
	QByteArray bVersionLine = device.readLine(32);
	QString versionLine = QString::fromLatin1(bVersionLine.constData());
	QRegExp versionRegex("MAP V(\\d\\d)\\.(\\d\\d)(\\+)? Furcadia\n");

	int version = 0;

	if (versionRegex.exactMatch(versionLine)) {
		int first = versionRegex.cap(1).toInt();
		int second = versionRegex.cap(2).toInt();
		version = (first * 100) + second;
	} else {
		m_error = BadVersionLineError;
		return false;
	}

	if (version < 100 || version > 199) {
		m_error = UnsupportedVersionError;
		return false;
	}

	// As of v29, DreamEd creates V01.40 if the map has regions/effects
	// V01.30 otherwise

	// Read in the header
	QMap<QString, QString> header;

	forever {
		QByteArray bLine = device.readLine(4096);
		QString line = QString::fromLatin1(bLine);

		if (line == "BODY\n") {
			// we've reached the end
			break;
		}

		int eqIndex = line.indexOf('=');

		if (!line.endsWith('\n') || eqIndex == -1) {
			m_error = BadHeaderError;
			return false;
		}

		QString key = line.left(eqIndex);
		QString value = line.mid(eqIndex+1);
		value.chop(1); // linefeed
		header.insert(key, value);
	}


	// Process everything from it!
	int width = header.value("width").toInt();
	int height = header.value("height").toInt();

	if (width < 1 || height < 1) {
		m_width = width;
		m_height = height;
		m_error = InvalidMapSizeError;
		return false;
	}

	if (width > MAX_MAP_WIDTH || height > MAX_MAP_HEIGHT) {
		m_width = width;
		m_height = height;
		m_error = MapTooBigError;
		return false;
	}

	if (header.value("noload").toInt() && header.value("encoded").toInt()) {
		m_error = FileIsEncryptedError;
		return false;
	}

	patchType = (PatchType)header.value("patcht").toInt();
	patchName = header.value("patchs");

	isEncrypted = (bool)header.value("encoded").toInt();

	revision = header.value("revision", "1").toInt();
	dreamName = header.value("name");

	QString standardStr = header.value("rating");
	standard = TeenStandard; // default
	for (int i = 0; i < StandardCount; i++)
		if (standardStr == StandardNames[i]) {
			standard = (StandardType)i;
			break;
		}

	allowSummon = (bool)header.value("allowjs", "1").toInt();
	allowFollow = (bool)header.value("allowlf", "1").toInt();
	allowURLs = (bool)header.value("allowfurl", "1").toInt();

	enableFilter = (bool)header.value("swearfilter", "1").toInt();
	parentalWarning = (bool)header.value("parentalcontrols").toInt();

	restrictSitting = (bool)header.value("forcesittable").toInt();

	allowShouting = (bool)header.value("allowshouts", "1").toInt();
	allowWho = !((bool)header.value("nowho").toInt());
	allowTab = !((bool)header.value("notab").toInt());
	allowSeasonals = !((bool)header.value("nonovelty").toInt());

	hasGroupPackage = (bool)header.value("allowlarge").toInt();


	// And finally, load the actual data! TODO: Make this endian-agnostic
	// (though I don't think anyone'll be running CMap on a big-endian CPU
	//  any time soon.)
	resize(width, height);

	for (int x = 0; x < width; x++)
		device.read((char*)floors[x], height * 2);
	for (int x = 0; x < width; x++)
		device.read((char*)items[x], height * 2);
	for (int x = 0; x < (width * 2); x++)
		device.read((char*)walls[x], height);

	if (version >= 140) {
		for (int x = 0; x < width; x++)
			device.read((char*)regions[x], height * 2);
		for (int x = 0; x < width; x++)
			device.read((char*)effects[x], height * 2);
	}

	// That's all, for now

	return true;
}



bool CMap::save(QString filePath) {
	m_error = NoError;

	QFile file(filePath);
	if (file.open(QFile::WriteOnly)) {
		QDataStream stream(&file);
		return save(stream);
	} else {
		m_error = CannotOpenFileError;
		return false;
	}
}


static void writeHeaderLine(QIODevice &device, const char *key, int value) {
	device.write(QString("%0=%1\n").arg(key).arg(value).toLatin1());
}

static void writeHeaderLine(QIODevice &device, const char *key, bool value) {
	device.write(QString("%0=%1\n").arg(key).arg(value ? 1 : 0).toLatin1());
}

static void writeHeaderLine(QIODevice &device, const char *key, QString value) {
	device.write(QString("%0=%1\n").arg(key).arg(value).toLatin1());
}

static void writeHeaderLine(QIODevice &device, const char *key, const char *value) {
	device.write(QString("%0=%1\n").arg(key).arg(value).toLatin1());
}


bool CMap::save(QDataStream &stream) {
	stream.setByteOrder(QDataStream::LittleEndian);

	QIODevice &device = *stream.device();

	// what version are we? 1.30 without effects/regions, 1.40 with
	int version = needsVersion140() ? 140 : 130;
	int ver1 = version / 100;
	int ver2 = version % 100;

	// make the version line
	QString versionLine = QString("MAP V%0.%1 Furcadia\n").arg(ver1, 2, 10, QChar('0')).arg(ver2, 2, 10, QChar('0'));
	QByteArray bVersionLine = versionLine.toLatin1();

	device.write(bVersionLine);

	// now put together a header
	writeHeaderLine(device, "height", m_height);
	writeHeaderLine(device, "width", m_width);
	writeHeaderLine(device, "revision", revision);
	writeHeaderLine(device, "patcht", (int)patchType);
	writeHeaderLine(device, "name", dreamName);
	writeHeaderLine(device, "patchs", patchName);
	writeHeaderLine(device, "encoded", isEncrypted);
	writeHeaderLine(device, "allowjs", allowSummon);
	writeHeaderLine(device, "allowlf", allowFollow);
	writeHeaderLine(device, "allowfurl", allowURLs);
	writeHeaderLine(device, "swearfilter", enableFilter);
	writeHeaderLine(device, "nowho", !allowWho);
	writeHeaderLine(device, "forcesittable", restrictSitting);
	writeHeaderLine(device, "allowshouts", allowShouting);
	writeHeaderLine(device, "rating", StandardNames[(int)standard]);
	writeHeaderLine(device, "allowlarge", hasGroupPackage);
	writeHeaderLine(device, "notab", !allowTab);
	writeHeaderLine(device, "nonovelty", !allowSeasonals);
	writeHeaderLine(device, "parentalcontrols", parentalWarning);
	device.write("BODY\n");

	// now write the actual data
	for (int x = 0; x < m_width; x++)
		device.write((const char *)floors[x], m_height * 2);
	for (int x = 0; x < m_width; x++)
		device.write((const char *)items[x], m_height * 2);
	for (int x = 0; x < (m_width * 2); x++)
		device.write((const char *)walls[x], m_height);

	if (version >= 140) {
		for (int x = 0; x < m_width; x++)
			device.write((const char *)regions[x], m_height * 2);
		for (int x = 0; x < m_width; x++)
			device.write((const char *)effects[x], m_height * 2);
	}

	return true;
}


bool CMap::needsVersion140() const {
	for (int x = 0; x < m_width; x++) {
		const quint16 *reg = regions[x];
		const quint16 *ef = effects[x];

		for (int y = 0; y < m_height; y++) {
			if (reg[y] != 0 || ef[y] != 0)
				return true;
		}
	}

	return false;
}


static quint16 s_randomShape(int base, int count, int x, int y) {
	quint32 seed = (quint32)((y / 4) + y + (x * 18));
	seed = (((seed * 0x343FD) + 0x269EC3) >> 0x10) & 0x7FFF;
	return (quint16)((seed & (count - 1)) + base);
}

int CMap::randomisedFloor(int x, int y) const {
	return randomisedFloor(x, y, floors[x][y]);
}

/*static*/ int CMap::randomisedFloor(int x, int y, int shapeNum) {
	switch (shapeNum) {
	case 0: return s_randomShape(shapeNum, 8, x, y);
	case 8: return s_randomShape(shapeNum, 4, x, y);
	case 32: return s_randomShape(shapeNum, 8, x, y);
	case 40: return s_randomShape(shapeNum, 8, x, y);
	case 116: return s_randomShape(shapeNum, 4, x, y);
	case 120: return s_randomShape(shapeNum, 4, x, y);
	case 124: return s_randomShape(shapeNum, 4, x, y);
	case 132: return s_randomShape(shapeNum, 4, x, y);
	default: return shapeNum;
	}
}

int CMap::randomisedItem(int x, int y) const {
	return randomisedItem(x, y, items[x][y]);
}

/*static*/ int CMap::randomisedItem(int x, int y, int shapeNum) {
	if (shapeNum == 128)
		return s_randomShape(128, 16, x, y);
	else
		return shapeNum;
}


const char *CMap::nameForObjectType(ObjectType type) {
	switch (type) {
	case Item: return "item";
	case Floor: return "floor";
	case LeftWall: case RightWall: case Wall: return "wall";
	case Region: return "region";
	case Effect: return "effect";
	default: return "thing";
	}
}



void CMap::loadPatchContext(CPatchContext *pc, QString defaultPath, QString mapDir) {
	if (patchType == CustomPatch && !mapDir.isNull()) {
		// resolve the path! fun
		// got to take into account that non-Windows systems are case-sensitive.
		qDebug() << "[CMap::loadPatchContext]";
		qDebug() << "Map directory:" << mapDir;
		qDebug() << "Patch:" << patchName;

		QDir customPath(mapDir);
		if (cCaseInsensitiveCd(customPath, patchName)) {
			qDebug() << "After CD:" << customPath;
			pc->loadPatches(defaultPath, customPath.canonicalPath());
			return;
		} else {
			qDebug() << "Could not cd";
		}
	}

	pc->loadPatches(defaultPath);
}
