#include "BmpReader.h"

uint16_t *BmpReader::Read24or32BitsFileTo16Bits(std::string fileName, uint32_t &outWidth, uint32_t &outHeight) {

	FILE *pFile = fopen(fileName.c_str(), "rb");
	if (pFile == nullptr) {
		return nullptr;
	}

	BITMAPFILEHEADER header1;
	BITMAPINFOHEADER header2; // TODO: another bmp version?

	fread(&header1, sizeof(BITMAPFILEHEADER), 1, pFile);
	fread(&header2, sizeof(BITMAPINFOHEADER), 1, pFile);
	unsigned short bytesPerPixel = header2.biBitCount == 32 ? 4 : 3;

	fseek(pFile, header1.bfOffBits, SEEK_SET);  // just in case

	uint8_t *pPixels = new uint8_t[header2.biWidth * header2.biHeight * bytesPerPixel];

	fread(pPixels, bytesPerPixel, header2.biWidth * header2.biHeight, pFile);

	fclose(pFile);

	uint16_t *pPixels16 = new uint16_t[header2.biWidth * header2.biHeight]; // 16 bits

	// TODO: tiene que ser multiplo de 4
	for (int h = 0; h < header2.biHeight; h++) {
		for (int w = 0; w < header2.biWidth; w++) {

			//555 format, each color is from 0 to 32, instead of 0 to 256
			//int p24i = h * width_in_bytes_32 + w * 4;
			//int p16i = h * width_in_bytes_16 + w * 2;
			int src_i = (h * header2.biWidth + w) * bytesPerPixel; // TODO: padding???
			uint16_t red = (uint16_t)(pPixels[src_i + 0] * 31.f / 255.f);
			uint16_t grn = (uint16_t)(pPixels[src_i + 1] * 31.f / 255.f);
			uint16_t blu = (uint16_t)(pPixels[src_i + 2] * 31.f / 255.f);
			uint16_t alpha = header2.biBitCount == 32 ? (uint16_t)(pPixels[src_i + 3]) : 0;
			uint16_t sum = (red) | (grn << 5) | (blu << 10) | (alpha << 15);

			int dest_i = ((header2.biHeight - 1 - h) * header2.biWidth + w); // * 2;
			memcpy(&pPixels16[dest_i], &sum, 2);
		}
	}

	outWidth = header2.biWidth;
	outHeight = header2.biHeight;

	return pPixels16;
}


