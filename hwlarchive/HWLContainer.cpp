#include "HWLContainer.h"

HWLContainer::HWLContainer() {
	pFile = nullptr;
}

HWLContainer::~HWLContainer() {
	if (pFile != nullptr) {
		fclose(this->pFile);
	}
}

bool HWLContainer::Open(std::string filename) {
	pFile = fopen(filename.c_str(), "rb+");
	if (!pFile) {
		printf("Failed to open file: %s\n", filename.c_str());
		return false;
	}

	fread(&header, sizeof(HWLHeader), 1, pFile);
	if (header.uSignature != 'TD3D') {
		printf("Invalid format: %s\n", filename.c_str());
		return false;
	}
	//printf("  header.uFileTableOffset: %u\n", header.uFileTableOffset);
	fseek(pFile, header.uFileTableOffset, SEEK_SET);

	std::vector<HWLNode> vNodes;

	uint32_t uNumItems = 0;
	fread(&uNumItems, 4, 1, pFile);
	char tmpName[21];
	for (unsigned int i = 0; i < uNumItems; ++i) {
		fread(tmpName, 20, 1, pFile);
		tmpName[20] = 0;
		HWLNode node;
		node.sTextureName = std::string(tmpName);
		vNodes.push_back(node);
		printf(" name: %s - tmpName: %s\n", node.sTextureName.c_str(), tmpName);
	}

	for (unsigned int i = 0; i < uNumItems; ++i) {
		uint32_t uTextureOffset = 0;
		vNodes[i].uNodeOffsetInFileTable = ftell(pFile);
		fread(&uTextureOffset, 4, 1, pFile);
		vNodes[i].uTextureOffset = uTextureOffset;
		printf(" name: %s - uNodeOffsetInFileTable: %u - uTextureOffset: %u\n", vNodes[i].sTextureName.c_str(), vNodes[i].uNodeOffsetInFileTable, vNodes[i].uTextureOffset);
	}

	for (HWLNode &node : vNodes) {
		mNodes[node.sTextureName] = node;
		//printf(" - node: sTextureName: %s uTextureOffset: %u uNodeOffsetInFileTable: %u\n", node.sTextureName.c_str(), node.uTextureOffset, node.uNodeOffsetInFileTable);
	}

	return true;
}

bool HWLContainer::Create(std::string filename, std::vector<std::string> textureNames) {
	pFile = fopen(filename.c_str(), "wb+");
	HWLHeader header = HWLHeader();
	header.uSignature = 'TD3D';
	header.uFileTableOffset = 8;
	fwrite(&header, sizeof(HWLHeader), 1, pFile);

	uint32_t uNumItems = textureNames.size();
	fwrite(&uNumItems, 4, 1, pFile);

	for (auto& name : textureNames) {
		if (name.size() > 20) {
			printf(" bad filename: %s, max 20 chars. Skipping.\n", name.c_str());
			continue;
		}
		HWLNode node;
		node.sTextureName = name;
		mNodes[name] = node;
		char tmpName[21];
		strcpy(tmpName, name.c_str());
		fwrite(tmpName, 20, 1, pFile);
		printf(" name: %s - tmpName: %s\n", name.c_str(), tmpName);
	}

	for (auto& name : textureNames) {
		mNodes[name].uNodeOffsetInFileTable = ftell(pFile);
		size_t uTextureOffset = 0;
		fwrite(&uTextureOffset, 4, 1, pFile);
		printf(" name: %s - uNodeOffsetInFileTable: %u - uTextureOffset: %u\n", name.c_str(), mNodes[name].uNodeOffsetInFileTable, uTextureOffset);
	}

	return true;
}

void HWLContainer::UpdateTexture(std::string textureName, uint32_t width, uint32_t height, uint32_t compressedSize, uint8_t *compressedPixels) {
	if (width % 4 != 0 || height %4 != 0) {
		printf(" error updating %s, width and height must be multiple of 4 (must implement)\n", textureName.c_str());
		return;
	}

	fseek(pFile, 0, SEEK_END);
	size_t endPosition = ftell(pFile);

	uint32_t nodeOffset = mNodes[textureName].uNodeOffsetInFileTable;
	fseek(pFile, nodeOffset, SEEK_SET);
	uint32_t newTextureOffset = endPosition;
	fwrite(&newTextureOffset, 4, 1, pFile);
	mNodes[textureName].uTextureOffset = newTextureOffset;

	HWLTextureHeader header = HWLTextureHeader();
	header.uAreaHeigth = header.uBufferHeight = header.uHeight = height;
	header.uAreaWidth = header.uBufferWidth = header.uWidth = width;
	header.uAreaX = header.uAreaY = 0;
	header.uCompressedSize = compressedSize;

	fseek(pFile, newTextureOffset, SEEK_SET);
	fwrite(&header, sizeof(HWLTextureHeader), 1, pFile);
	fwrite(compressedPixels, sizeof(uint8_t), compressedSize, pFile);

	printf(" texture %s updated\n", textureName.c_str());
}

std::vector<std::string> HWLContainer::GetAllTextureNames() {
	std::vector<std::string> names;
	for (auto& node: mNodes) {
		names.push_back(node.first);
	}
	return names;
}
 