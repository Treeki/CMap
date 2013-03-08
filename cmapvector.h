#ifndef CMAPVECTOR_H
#define CMAPVECTOR_H

#include <QtGlobal>
#include "cmappoint.h"
#include <cstring>

template <typename TValue, int TColumns, bool TIsMultiplied>
class CMapVector {
private:
	int m_width, m_height;
	TValue *m_columns[TColumns];

public:
	CMapVector() {
		m_width = 0;
		m_height = 0;

		memset(m_columns, 0, sizeof(m_columns));
	}

	~CMapVector() {
		for (int x = 0; x < m_width; x++)
			delete[] m_columns[x];
	}


	void resize(int newWidth, int newHeight) {
		int oldWidth = m_width, oldHeight = m_height;

		// If it's zero, just nuke it all
		newWidth = (newHeight == 0) ? 0 : qMin(newWidth, TColumns);

		// If there's any columns we don't need, nuke them
		if (oldWidth > newWidth) {
			for (int x = newWidth; x < oldWidth; x++) {
				delete[] m_columns[x];
				m_columns[x] = 0;
			}
		}

		// And if we need any new ones, create them
		if (newWidth > oldWidth) {
			for (int x = oldWidth; x < newWidth; x++) {
				m_columns[x] = new TValue[newHeight];
				memset(m_columns[x], 0, sizeof(TValue) * newHeight);
			}
		}

		// Finally, if the height's changed, realloc the existing ones
		if (oldHeight != newHeight) {
			int existingBound = qMin(oldWidth, newWidth);
			int copyCount = qMin(oldHeight, newHeight);

			for (int x = 0; x < existingBound; x++) {
				TValue *newColumn = new TValue[newHeight];

				memcpy(newColumn, m_columns[x], sizeof(TValue) * copyCount);

				// Fill in empty space if there's any
				if (newHeight > oldHeight) {
					memset(&newColumn[oldHeight], 0, sizeof(TValue) * (newHeight - oldHeight));
				}

				// replace the array
				delete[] m_columns[x];
				m_columns[x] = newColumn;
			}
		}

		m_width = newWidth;
		m_height = newHeight;
	}


	inline TValue *operator[](int x) {
		return m_columns[x];
	}

	inline const TValue *operator[](int x) const {
		return m_columns[x];
	}


	inline TValue get(const CMapPoint &point) const {
		return m_columns[point.x / (TIsMultiplied ? 2 : 1)][point.y];
	}

	inline void set(const CMapPoint &point, TValue value) {
		m_columns[point.x / (TIsMultiplied ? 2 : 1)][point.y] = value;
	}
};

#endif // CMAPVECTOR_H
