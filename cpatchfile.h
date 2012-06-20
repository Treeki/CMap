#ifndef CPATCHFILE_H
#define CPATCHFILE_H

#include <QtCore/QVector>
#include <QtCore/QDir>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

class CFrame {
public:
	QPixmap pixmap;

	qint16 posX, posY;
	qint16 furrePosX, furrePosY;

	enum FormatType {
		Format8Bit = 1,
		/* V2 and above... */
		FormatBGR = 2,
		FormatBGRA = 3,
		FormatBGRARecol = 7
	};

	FormatType format;

	// V3 and above
	quint8 opacity;

	enum CompressionType {
		NoCompression,
		PNGCompression
	};
	CompressionType compression;
};

class CStep {
public:
	enum StepType {
		FrameStep = 1,
		DelayStep,
		LoopStep,
		JumpStep,
		PosXStep,
		PosYStep,
		FurreXStep,
		FurreYStep,
		DrawFrontStep,
		DrawBehindStep,
		AutoFrameDelayStep,
		StopStep,
		CameraStateStep,
		/* V2/V3 and above? */
		RandFrameDelayStep,
		RandAutoFrameDelayStep,
		ShapeFrameStep,
		OpacityStep,
		SlidePosXStep,
		SlidePosYStep,
		SlideFurreXStep,
		SlideFurreYStep,
		SlideOpacityStep,
		ShowBGFrameStep,
		ShowFGFrameStep,
		ShowBGShapeStep,
		ShowFGShapeStep,
		HideBGStep,
		HideFGStep
	};

	StepType type;
	qint16 arg1, arg2;
};

class CShape {
public:
	QVector<CFrame> frames;
	QVector<CStep> steps;

	qint16 replacedIndex;

	enum Flags {
		WalkableFlag = 1,
		GettableFlag = 2,
		SittableFlag = 4
	};

	Flags flags;
};


class CPatchFile {
public:
	CPatchFile();

	static QString getDefaultPatchPath();
private:
	static QString generateDefaultPatchPath();
	static QString cachedDefaultPatchPath;
public:

	bool loadFromDir(QDir path, QString name, QVector<QRgb> &palette);

	bool loadFOX(QDataStream &stream, QVector<QRgb> &palette);
	bool loadFSH(QDataStream &stream, QVector<QRgb> &palette);
	bool loadFBJ(QDataStream &stream);

	qint32 version;

	QVector<CShape> shapes;

	enum ErrorType {
		NoError = 0,
		BadMagicError,
		FileIsEncryptedError,
		UnsupportedImageFormatError,
		UnsupportedCompressionError
	};

	ErrorType error() const;

private:
	ErrorType m_error;
};

#endif // CPATCHFILE_H
