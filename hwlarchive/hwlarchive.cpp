// hwlpack.cpp : Defines the entry point for the console application.
//

#include "hwlarchive.h"

//const char* dataFolder = "C:\\Program Files (x86)\\3DO\\Might and Magic VII\\DATA\\";
//const char* texturesFolder = "E:\\MM7\\converted\\";
const char* dataFolder = "";
const char* texturesFolder = "bmps\\";
BmpReader bmpReader;

int updateTexture(HWLContainer* pD3DBitmaps, std::string textureName) {
	uint32_t width = 0;
	uint32_t height = 0;

	auto bmpFilename = std::string(texturesFolder) + textureName + ".bmp";
	if (textureName == "HWR1BS~1") {
		bmpFilename = std::string(texturesFolder) + "HWR1BS1.bmp";
	} else if (textureName == "HWR1B~1") {
		bmpFilename = std::string(texturesFolder) + "HWR1B.bmp";
	}

	uint16_t *pixels = bmpReader.Read24or32BitsFileTo16Bits(bmpFilename, width, height);

	if (pixels == nullptr) {
		printf(" error reading %s\n", bmpFilename.c_str());
		return -1;
	}

	zlib::uLong compressedSize = zlib::compressBound(width * height * 2);
	uint8_t *compressedPixels = (uint8_t *)malloc(compressedSize);

	auto result = zlib::compress(compressedPixels, &compressedSize, (uint8_t *)pixels, width * height * 2);
	if (result != Z_OK) {
		printf(" error compressing %s -> result: %i\n", textureName, result);
		return -1;
	}

	pD3DBitmaps->UpdateTexture(textureName, width, height, compressedSize, (uint16_t *)compressedPixels);

	delete[] pixels;
	free(compressedPixels);
}

int main()
{
	printf("Starting...\n");

	HWLContainer* pD3DBitmaps = new HWLContainer();
	pD3DBitmaps->Open(std::string(dataFolder) + "d3dbitmap.hwl");

	auto textureNames = pD3DBitmaps->GetAllTextureNames();
	for (auto textureName : textureNames) {
		updateTexture(pD3DBitmaps, textureName);
	}

	delete pD3DBitmaps;

	printf("Finished. Press enter to exit.\n");
	scanf("enter");
}

