#pragma once
#include "../PlatformShared/platform_shared.h"
#include "memory.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

/*
handmade hero style asset system

we have 4 master arrays
these master arrays contains all the data.



	AssetFamily assetFamilies[NUM_ASSET_FAMILY];

											asset *Assets;                          asset_slot *Bitmaps;
				 ___________
				| Asset_    |               backdrop0                               bitmap0
				|  Backdrop |               backdrop1                               bitmap1
				|___________|               backdrop2                               bitmap2
				| Asset_    |               backdrop3                               bitmap3
				|  Shadow   |               backdrop4                               bitmap4
				|___________|               shadow0         Bitmaps[slotID]         bitmap5
				| Asset_    |  --------->   shadow1         --------->              bitmap6
				|  Tree     |               shadow2                                 ...
				|___________|               shadow3                                 ...
				|           |               tree0
				|  ...      |               tree1
				|___________|               tree2
				|           |               tree3
				|  ...      |               tree4
				|___________|               tree5
				|           |               ...
				|           |               ...
				|___________|


*/

namespace AssetDataFormatType
{
	enum Enum
	{
		Bitmap,
		Sound,
		Font,
		FontGlyph
	};
}

namespace AssetFamilyType
{
	enum Enum
	{
		None,
		Default,
		Font,
		FontGlyph,
		Wall,
		NUM_ASSET_FAMILY,
	};
}
extern PlatformAPI platformAPI;

struct BitmapId
{
	uint32 value;
};

struct SoundId
{
	uint32 value;
};

struct FontId
{
	uint32 value;
};

enum AssetState
{
	Unloaded,
	Queued,
	Loaded,
	Locked
};

struct AssetBitmapInfo
{
	char* filename;
};

struct AssetFontInfo
{
	char* filename;
};

struct AssetFamily
{
	uint32 startAssetIndex;
	uint32 onePastLastAssetIndex;
};

struct AssetTag
{
	uint32 tagProperty;
	float value;
};


struct LoadedFont
{
	char* filename;
	int maxGlyphs;
	int numGlyphs;
	BitmapId* glyphs;
	float horizontalAdvance;
	uint16* unicodeMap;
};

struct LoadedGlyph
{
	char c;
	LoadedBitmap bitmap;
};

// contains meta data about the asset 
struct AssetHandle
{
	// this looks into our master tag Table
	uint32 startTagIndex;
	uint32 onePastLastTagIndex;


	union 
	{
		AssetBitmapInfo bitmapInfo;
		AssetFontInfo fontInfo;
	};

	// or int AssetIndex?
};



// This is the handmade hero assetAlot class
struct Asset
{
	AssetDataFormatType::Enum type;
	AssetState state;

	union
	{
		LoadedBitmap loadedBitmap;
		LoadedFont loadedFont;
		LoadedGlyph loadedGlyph;
	};
};


struct GameAssets
{
	AssetFamily masterAssetFamilyTable[AssetFamilyType::NUM_ASSET_FAMILY];

	uint32 maxAssetHandles;
	uint32 numAssetHandles;
	AssetHandle* masterAssetHandleTable;

	// Asset handle doesnt have a 
	uint32 maxAssets;
	uint32 numAssets;
	Asset* assets;

	uint32 numMasterTags;
	AssetTag* masterTagTable;

	// These are mostly used when writing in assets
	AssetFamily* currentEditedAssetFamily = nullptr;
//	uint32 currentNumAssetHandle;		do you need this guy
};



void BeginAssetFamily(GameAssets* ga, AssetFamilyType::Enum type, GameAssets* & gameAssets)
{
	assert(!ga->currentEditedAssetFamily);
	ga->currentEditedAssetFamily = &ga->masterAssetFamilyTable[(int)type];

	AssetFamily* family = ga->currentEditedAssetFamily;
	family->startAssetIndex = ga->numAssetHandles;   // does this work?
	family->onePastLastAssetIndex = family->startAssetIndex;
}


void BeginAssetFamily(GameAssets* ga, AssetFamilyType::Enum type)
{
	assert(!ga->currentEditedAssetFamily);
	ga->currentEditedAssetFamily = &ga->masterAssetFamilyTable[(int)type];

	AssetFamily* family = ga->currentEditedAssetFamily;
	family->startAssetIndex = ga->numAssetHandles;   // does this work?
	family->onePastLastAssetIndex = family->startAssetIndex;
}

void EndAssetFamily(GameAssets* ga)
{
//	std::cout << "onePastLastAssetIndex " << ga->currentAssetFamilyType->onePastLastAssetIndex << std::endl;

	assert(ga->currentEditedAssetFamily);
	ga->currentEditedAssetFamily = nullptr;

	
}

uint32 GetFirstAssetIdFrom(GameAssets* Assets, AssetFamilyType::Enum familyType)
{
	uint32 Result = 0;

	AssetFamily* family = &Assets->masterAssetFamilyTable[familyType];
	if (family->startAssetIndex != family->onePastLastAssetIndex)
	{
		Result = family->startAssetIndex;
	}

	return(Result);
}

BitmapId GetFirstBitmapIdFrom(GameAssets* ga, AssetFamilyType::Enum familyType)
{
	BitmapId result = { GetFirstAssetIdFrom(ga, familyType) };
	return(result);
}

BitmapId GetBitmapForGlyph(GameAssets* ga, LoadedFont *font, uint32 desiredCodePoint)
{
	BitmapId result = font->glyphs[10];	
	return(result);
}


LoadedBitmap* GetBitmap(GameAssets* ga, BitmapId id)
{
	LoadedBitmap* result = &ga->assets[id.value].loadedBitmap;
	return result;
}

LoadedFont* GetFont(GameAssets* ga, FontId id)
{
	LoadedFont* result = &ga->assets[id.value].loadedFont;
	return result;
}


LoadedBitmap CreateEmptyBitmap(MemoryArena* memoryArena, uint32 width, uint32 height, bool clearToZero)
{
	LoadedBitmap result = {};

	//	Result.AlignPercentage = V2(0.5f, 0.5f);
	//	Result.WidthOverHeight = SafeRatio1((r32)Width, (r32)Height);

	result.width = width;
	result.height = height;
	result.pitch = result.width * 4;
	int32 totalBitmapSize = result.pitch * height;
	result.memory = PushSize(memoryArena, totalBitmapSize);

	return result;
}

// 
LoadedFont CreateEmptyLoadedFont(MemoryArena* memoryArena, char* filename)
{
	LoadedFont loadedFont = {};
	loadedFont.maxGlyphs = 256;
	loadedFont.numGlyphs = 0;
	loadedFont.glyphs = PushArray(memoryArena, loadedFont.maxGlyphs, BitmapId);
	loadedFont.filename = "c:/Windows/Fonts/arial.ttf";

	loadedFont.horizontalAdvance = 10;

	return loadedFont;
}

void LoadBitmapToMemory(GameAssets* ga, BitmapId id)
{
	AssetHandle* handle = &ga->masterAssetHandleTable[id.value];

	AssetBitmapInfo* info = &handle->bitmapInfo;
	BitmapInfo bitmap = platformAPI.readImageFile(info->filename);

	Asset* asset = &ga->assets[id.value];
	asset->state = AssetState::Unloaded;

	LoadedBitmap result = {};
	result.width = bitmap.width;
	result.height = bitmap.height;
	result.memory = bitmap.memory;

	void* textureHandle = platformAPI.allocateTexture(result.width, result.height, result.memory);
	result.textureHandle = POINTER_TO_UINT32(textureHandle);
	std::cout << "texture handle " << result.textureHandle << std::endl;
	asset->loadedBitmap = result;
}

void LoadGlyphBitmapToMemory(MemoryArena* memoryArena, GameAssets* ga, LoadedFont* loadedFont, BitmapId id)
{
	AssetHandle* handle = &ga->masterAssetHandleTable[id.value];

	Asset* asset = &ga->assets[id.value];
	asset->state = AssetState::Unloaded;

	char c = asset->loadedGlyph.c;


	unsigned char* fileContent = 0;
	int fileSize = 0;

	FILE *ptr;

//	ptr = fopen("c:/Windows/Fonts/arial.ttf", "rb");  // r for read, b for binary
	ptr = fopen(loadedFont->filename, "rb");
	fseek(ptr, 0L, SEEK_END);
	fileSize = ftell(ptr);
	fseek(ptr, 0L, SEEK_SET);

	fileContent = (unsigned char*)VirtualAlloc(0, (size_t)fileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	fread(fileContent, sizeof(uint8), fileSize, ptr); // read 10 bytes to our buffer

	int width, height, xOffset, yOffset;

	stbtt_fontinfo font;
	stbtt_InitFont(&font, fileContent, stbtt_GetFontOffsetForIndex(fileContent, 0));
	unsigned char*  monoBitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, 20), c, &width, &height, &xOffset, &yOffset);
	uint8* src = monoBitmap;

	// 4 bytes per pixel
	LoadedBitmap result = CreateEmptyBitmap(memoryArena, width, height, false);
	result.width = width;
	result.height = height;
	result.memory = VirtualAlloc(0, (size_t)width * height * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	uint32* dst = (uint32*)result.memory;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			uint8 alpha = *src++;
			*dst++ = (alpha << 24) | (alpha << 16) | (alpha << 8) | (alpha << 0);
		}
	}

	stbtt_FreeBitmap(monoBitmap, 0);


	void* textureHandle = platformAPI.allocateTexture(result.width, result.height, result.memory);
	result.textureHandle = POINTER_TO_UINT32(textureHandle);
	std::cout << "texture handle " << result.textureHandle << std::endl;
	asset->loadedGlyph.bitmap = result;
}





// This is just used so that I can return two things
struct AddedAsset
{
	uint32 id;  // index of the handle in the master handle array 
				// also the index of data inside the master data array
	AssetHandle* handle;
	Asset* data;
};

AddedAsset AddBaseAsset(GameAssets* ga)
{
	assert(ga->currentEditedAssetFamily);
	int index = ga->currentEditedAssetFamily->onePastLastAssetIndex++;

	// add a new handle and a source
	AssetHandle* handle = &ga->masterAssetHandleTable[index];
	Asset* data = &ga->assets[index];

	ga->numAssetHandles++;
	ga->numAssets++;

	handle->startTagIndex = 0;
	handle->onePastLastTagIndex = 0;
	
	AddedAsset addedAsset = {};
	addedAsset.id = index;
	addedAsset.handle = handle;
	addedAsset.data = data;
	return addedAsset;
}

void AddBitmapAsset(GameAssets* ga, char* filename)
{
	assert(ga->currentEditedAssetFamily);
	AddedAsset newAssetInfo = AddBaseAsset(ga);

	newAssetInfo.handle->bitmapInfo.filename = filename;
	newAssetInfo.data->type = AssetDataFormatType::Bitmap;

}

void AddCharacterAsset(GameAssets* ga, LoadedFont* fontAssetInfo, char c)
{
	assert(ga->currentEditedAssetFamily);
	AddedAsset newAssetInfo = AddBaseAsset(ga);

	newAssetInfo.data->type = AssetDataFormatType::FontGlyph;
	int glyphIndex = fontAssetInfo->numGlyphs;
	fontAssetInfo->glyphs[glyphIndex] = { newAssetInfo.id };
	fontAssetInfo->numGlyphs++;

	newAssetInfo.data->loadedGlyph.c = c;
}

void AddFontAsset(GameAssets* ga, LoadedFont* fontAssetInfo)
{
	assert(ga->currentEditedAssetFamily);
	AddedAsset newAssetInfo = AddBaseAsset(ga);

	newAssetInfo.data->type = AssetDataFormatType::Font;


	
	LoadedFont font = {};
	font.numGlyphs = fontAssetInfo->numGlyphs;
	font.horizontalAdvance = fontAssetInfo->horizontalAdvance;

	int size = font.numGlyphs * sizeof(BitmapId);
	font.glyphs = (BitmapId*)malloc(size);
	memcpy((void*)font.glyphs, (void*)fontAssetInfo->glyphs, size);

	newAssetInfo.data->loadedFont = font;
}


void AllocateGameAssets(MemoryArena* memoryArena, GameAssets* ga, GameAssets* & gameAssets)
{

	// replace this with pushing it to the memory arena.
	ga->maxAssets = 256;
	ga->numAssets = 0;
	ga->assets = PushArray(memoryArena, ga->maxAssets, Asset);

	// the 0th handle is just the null asset.
	ga->maxAssetHandles = ga->maxAssets;
	ga->numAssetHandles = 0;
	ga->masterAssetHandleTable = PushArray(memoryArena, ga->maxAssetHandles, AssetHandle);

	ga->numMasterTags = 0;

	// simulating this process of loading pack file off disk 
	BeginAssetFamily(ga, AssetFamilyType::None);
	AddBitmapAsset(ga, "./Assets/error.bmp");
	EndAssetFamily(ga);

	BeginAssetFamily(ga, AssetFamilyType::Default);
	AddBitmapAsset(ga, "./Assets/white.bmp");
	EndAssetFamily(ga);

	BeginAssetFamily(ga, AssetFamilyType::Wall, gameAssets);
	AddBitmapAsset(ga, "./Assets/wall2.bmp");
	AddBitmapAsset(ga, "./Assets/wall1.bmp");
	EndAssetFamily(ga);

	LoadedFont loadedFont = CreateEmptyLoadedFont(memoryArena, "c:/Windows/Fonts/arial.ttf");
	
	BeginAssetFamily(ga, AssetFamilyType::FontGlyph);
	for (int i = 'a'; i <= 'z'; i++)
	{
		AddCharacterAsset(ga, &loadedFont, i);
	}

	for (int i = 'A'; i <= 'Z'; i++)
	{
		AddCharacterAsset(ga, &loadedFont, i);
	}
	EndAssetFamily(ga);

	BeginAssetFamily(ga, AssetFamilyType::Font);
	AddFontAsset(ga, &loadedFont);
	EndAssetFamily(ga);


	// Load all assets into memory

	// texture 0
	for (int i = 0; i < ga->numAssets; i++)
	{
		Asset* asset = &ga->assets[i];

		if (asset->type == AssetDataFormatType::Bitmap)
		{
			BitmapId bitmapId = { i };
			// std::cout << bitmapId.value << std::endl;
			LoadBitmapToMemory(ga, bitmapId);
		}
		else if (asset->type == AssetDataFormatType::Font)
		{
			// we already have one
			//	LoadFontToMemory(ga, bitmapId);
		}
		else if (asset->type == AssetDataFormatType::FontGlyph)
		{
			BitmapId bitmapId = { i };
			// std::cout << bitmapId.value << std::endl;
			LoadGlyphBitmapToMemory(memoryArena, ga, &loadedFont, bitmapId);
		}
	}
}


// defined in the gamecode
