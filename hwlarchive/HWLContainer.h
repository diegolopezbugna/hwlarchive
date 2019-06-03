#pragma once

#include <cstdint>
#include <cstdio>
#include <map>
#include <vector>

struct HWLHeader {
	uint32_t uSignature;
	uint32_t uFileTableOffset;
};

struct HWLTextureHeader {
	uint32_t uCompressedSize;
	uint32_t uBufferWidth;
	uint32_t uBufferHeight;
	uint32_t uAreaWidth;
	uint32_t uAreaHeigth;
	uint32_t uWidth;
	uint32_t uHeight;
	uint32_t uAreaX;
	uint32_t uAreaY;
};

struct HWLNode {
	std::string sTextureName;
	size_t uTextureOffset;
	size_t uNodeOffsetInFileTable;
};

class HWLContainer {
public:
	HWLContainer();
	virtual ~HWLContainer();

	bool Open(std::string filename);
	bool Create(std::string filename, std::vector<std::string> textureNames);
	std::vector<std::string> GetAllTextureNames();
	void UpdateTexture(std::string textureName, uint32_t width, uint32_t height, uint32_t compressedSize, uint8_t *compressedPixels);

protected:
	FILE *pFile;
	HWLHeader header;
	std::map<std::string, HWLNode> mNodes;
};
