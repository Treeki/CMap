#include "cpatchfile.h"
#include <QtCore/QFile>
#include "utility.h"

CPatchFile::CPatchFile() {
	version = 1;
	m_error = NoError;
}

CPatchFile::ErrorType CPatchFile::error() const {
	return m_error;
}


bool CPatchFile::loadFromDir(QDir path, QString name, QVector<QRgb> &palette) {
	// all this path fixing is a bit of a mess, I need to figure out
	// a way to make it, well ... not repeat the same operations 50 times to
	// load a patch ._.

	QString foxPath = cCaseInsensitiveFilePath(path, name + ".fox");

	if (QFile::exists(foxPath)) {
		QFile foxFile(foxPath);

		if (foxFile.open(QFile::ReadOnly)) {
			QDataStream foxStream(&foxFile);
			return loadFOX(foxStream, palette);
		}
	}


	QString fshPath = cCaseInsensitiveFilePath(path, name + ".fsh");

	if (QFile::exists(fshPath)) {
		QFile fshFile(fshPath);

		if (fshFile.open(QFile::ReadOnly)) {
			QDataStream fshStream(&fshFile);
			if (loadFSH(fshStream, palette)) {
				// Try the FBJ
				QString fbjPath = cCaseInsensitiveFilePath(path, name + ".fbj");

				if (QFile::exists(fbjPath)) {
					QFile fbjFile(fbjPath);

					if (fbjFile.open(QFile::ReadOnly)) {
						QDataStream fbjStream(&fbjFile);
						return loadFBJ(fbjStream);
					}
				}

				// if we couldn't open the FBJ or it didn't exist,
				// we don't care, so just return the successful FSH load
				return true;

			} else {
				// FSH failed, bail out
				return false;
			}
		}
	}

	// no FOX or FSH? that's a shame
	return false;
}


bool CPatchFile::loadFOX(QDataStream &stream, QVector<QRgb> &palette) {
	stream.setByteOrder(QDataStream::LittleEndian);

	// Read the file header
	char magic[4];
	stream.readRawData(magic, 4);

	if (magic[0] != 'F' || magic[1] != 'S' || magic[2] != 'H' || magic[3] != 'X') {
		m_error = BadMagicError;
		return false;
	}

	stream >> version;

	qint32 numShapes, generator, encryption, reserved1, reserved2;
	stream >> numShapes;
	stream >> generator;
	stream >> encryption;
	stream >> reserved1 >> reserved2;

	if (encryption > 0) {
		m_error = FileIsEncryptedError;
		return false;
	}

	// Go on to reading every shape now
	shapes.resize(numShapes);

	for (int shapeIdx = 0; shapeIdx < numShapes; shapeIdx++) {
		CShape &shape = shapes[shapeIdx];

		quint16 flags, numFrames, numSteps;

		stream >> flags;
		stream >> shape.replacedIndex;
		stream >> numFrames >> numSteps;

		shape.flags = (CShape::Flags)flags;

		// V3 and above has SHAPEEXT
		if (version >= 3) {
			quint16 shapeExtSize;
			stream >> shapeExtSize;

			// what else...?
			stream.skipRawData(shapeExtSize - 2);
		}

		// Frames come now
		shape.frames.resize(numFrames);

		for (int frameIdx = 0; frameIdx < numFrames; frameIdx++) {
			CFrame &frame = shape.frames[frameIdx];

			quint16 frameFormat, frameWidth, frameHeight;
			quint32 imageDataSize;

			stream >> frameFormat;
			stream >> frameWidth >> frameHeight;
			stream >> frame.posX >> frame.posY;
			stream >> frame.furrePosX >> frame.furrePosY;
			stream >> imageDataSize;

			frame.format = (CFrame::FormatType)frameFormat;

			// Deal with FRAMEEXT
			if (version >= 3) {
				quint16 frameExtSize;
				stream >> frameExtSize;

				// opacity and compressionType are always present.
				stream >> frame.opacity;
				quint8 compressionType;
				stream >> compressionType;
				frame.compression = (CFrame::CompressionType)compressionType;

				stream.skipRawData(frameExtSize - 4);
			} else {
				frame.opacity = 255;
				frame.compression = CFrame::NoCompression;
			}

			// Fail on unsupported things
			if (frame.format != CFrame::Format8Bit) {
				m_error = UnsupportedImageFormatError;
				return false;
			}

			if (frame.compression != CFrame::NoCompression) {
				m_error = UnsupportedCompressionError;
				return false;
			}

			// Now go ahead and read the image data ...
			QImage image(frameWidth, frameHeight, QImage::Format_Indexed8);
			image.setColorTable(palette);

			for (int y = frameHeight - 1; y >= 0; y--)
				stream.readRawData((char*)image.scanLine(y), frameWidth);

			frame.pixmap = QPixmap::fromImage(image);
		}

		// Finally do steps. These are simple!
		shape.steps.resize(numSteps);

		for (int stepIdx = 0; stepIdx < numSteps; stepIdx++) {
			CStep &step = shape.steps[stepIdx];

			quint16 stepType;
			stream >> stepType;

			step.type = (CStep::StepType)stepType;
			stream >> step.arg1;
			stream >> step.arg2;
		}
	}

	// And that's it.

	return true;
}


bool CPatchFile::loadFSH(QDataStream &stream, QVector<QRgb> &palette) {
	stream.setByteOrder(QDataStream::LittleEndian);

	// Finding docs for the FSH format is like pulling teeth.
	// So.. my only choice is to convert Kylix's old C# code. Oh well.

	quint16 numShapes;
	stream >> numShapes;

	// Skip over chunk sizes, whatever they are
	stream.skipRawData(2 * numShapes);

	shapes.resize(numShapes);

	for (int shapeIdx = 0; shapeIdx < numShapes; shapeIdx++) {
		CShape &shape = shapes[shapeIdx];

		shape.flags = (CShape::Flags)0;
		shape.replacedIndex = -1;

		// Set up the frame
		shape.frames.resize(1);
		CFrame &frame = shape.frames.first();

		quint8 width, height;
		qint8 x, y;

		stream >> width >> height;
		stream >> x >> y;

		// Use default values where FSH doesn't provide anything
		frame.format = CFrame::Format8Bit;
		frame.compression = CFrame::NoCompression;

		frame.posX = x;
		frame.posY = y;
		frame.furrePosX = 0;
		frame.furrePosY = 0;

		frame.opacity = 255;

		// Load the image
		QImage image(width, height, QImage::Format_Indexed8);
		image.setColorTable(palette);

		for (int y = height - 1; y >= 0; y--)
			stream.readRawData((char*)image.scanLine(y), width);

		frame.pixmap = QPixmap::fromImage(image);
	}

	// That's all, pretty much.
	// Not a lot that can go wrong in a FSH file!
	return true;
}


bool CPatchFile::loadFBJ(QDataStream &stream) {
	stream.setByteOrder(QDataStream::LittleEndian);

	// Read the file header
	char magic[4];
	stream.readRawData(magic, 4);

	if (magic[0] != 'F' || magic[1] != 'O' || magic[2] != '0' || magic[3] != '1') {
		m_error = BadMagicError;
		return false;
	}

	qint32 shapeCount;
	stream >> shapeCount;

	qint32 toModify = qMin(shapes.count(), shapeCount);

	for (int shapeIdx = 0; shapeIdx < toModify; shapeIdx++) {
		CShape &shape = shapes[shapeIdx];

		quint8 flag;
		stream >> flag;

		shape.flags = (CShape::Flags)(flag & 7);

		if (flag & 8) {
			quint8 furreX;
			stream >> furreX;
			shape.frames.first().furrePosX = furreX;
		}

		if (flag & 0x10) {
			quint8 furreY;
			stream >> furreY;
			shape.frames.first().furrePosY = furreY;
		}
	}

	return true;
}



// should this be somewhere else...?
#include <QSettings>
#include <QFileDialog>
#include <QApplication>

QString CPatchFile::cachedDefaultPatchPath;

QString CPatchFile::generateDefaultPatchPath() {
	QSettings settings;

	// might we have a custom one saved...?
	if (settings.contains("cmDefaultPatchDir")) {
		QString checkThis = settings.value("cmDefaultPatchDir").toString();
		QDir tryIt(checkThis);

		if (tryIt.exists("regions.fox"))
			return checkThis;
	}

	// if we're not on Windows don't even bother trying
//#ifdef Q_WS_WIN
	QByteArray pf = qgetenv((QSysInfo::WordSize == 64) ? "ProgramFiles(x86)" : "ProgramFiles");
	if (!pf.isEmpty()) {
		QDir pfDir(QString::fromLocal8Bit(pf.constData()));
		if (pfDir.cd("Furcadia") &&
				pfDir.cd("patches") &&
				pfDir.cd("default") &&
				pfDir.exists("regions.fox")) {
			// seems like this might work!
			return pfDir.canonicalPath();
		}
	}
//#endif

	// no dice, ask the user to choose one
	QString error;

	forever {
		QString userPath = QFileDialog::getExistingDirectory(0, error + "Please locate the Furcadia default patch folder.");
		if (userPath.isEmpty()) {
			// oh.
			QApplication::exit();
			return QString();
		}

		QDir userDir(userPath);
		if (userDir.exists("regions.fox")) {
			// this should be the one
			settings.setValue("cmDefaultPatchDir", userPath);
			settings.sync();
			return userPath;
		} else {
			error = "That doesn't seem to be right! ";
		}
	}
}

QString CPatchFile::getDefaultPatchPath() {
	if (cachedDefaultPatchPath.isNull()) {
		cachedDefaultPatchPath = generateDefaultPatchPath();
	}

	return cachedDefaultPatchPath;
}
