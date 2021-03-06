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

int main(int argc, char *argv[])
{
	printf("hwlarchive v0.2\n\n");
	printf("usage: hwlarchive [name.hwl]   --> d3dbitmap.hwl if none provided\n\n");
	printf("Copy .exe and zlibwapi.dll to DATA folder.\n");
	printf("Create 'bmps' folder with the bmps (24 or 32 bits) that will be updated in the hwl file.\n");
	printf("Beware that contents will be appended, so if you make the update several times the file will be larger every time.\n");
	printf("\n");

	printf("Starting...\n");

	HWLContainer* pD3DBitmaps = new HWLContainer();
	pD3DBitmaps->Open(std::string(dataFolder) + (argc > 1 ? argv[1] : "d3dbitmap.hwl"));

	auto textureNames = pD3DBitmaps->GetAllTextureNames();
	for (auto textureName : textureNames) {
		updateTexture(pD3DBitmaps, textureName);
	}

	delete pD3DBitmaps;

	printf("Finished. Press enter to exit.\n");
	scanf("enter");
}

