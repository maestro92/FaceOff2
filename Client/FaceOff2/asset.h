#pragma once
#include "../PlatformShared/platform_shared.h"



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

enum AssetFamilyType
{
	None,
	Default,
	Font,
	Wall,
	NUM_ASSET_FAMILY,
};

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

// contains meta data about the asset 
struct AssetHandle
{
	// this looks into our master tag Table
	uint32 startTagIndex;
	uint32 onePastLastTagIndex;

	// not sure if i want to mix in game stuff with asset structure stuff
	AssetState state;
	LoadedBitmap* loadedBitmap;

	// Either bitmapId, or soundId or fontId

	// uint32 assetId;

	union 
	{
		BitmapId bitmapId;
		SoundId soundId;
		FontId fontId;
	} assetId;

	// or int AssetIndex?
};



// This is the handmade hero assetAlot class
struct Asset
{
	AssetState state;
	LoadedBitmap loadedBitmap;
};


struct GameAssets
{
	AssetFamily masterAssetFamilyTable[AssetFamilyType::NUM_ASSET_FAMILY];

	uint32 maxAssetHandles;
	uint32 numAssetHandles;
	AssetHandle* masterAssetHandleTable;

	uint32 maxBitmaps;
	uint32 numBitmaps;
	AssetBitmapInfo* bitmapInfos;
	Asset* bitmaps;
	// Asset* fonts
	// Asset* sounds;

	uint32 numMasterTags;
	AssetTag* masterTagTable;

	// These are mostly used when writing in assets
	AssetFamily* currentAssetFamilyType = nullptr;
//	uint32 currentNumAssetHandle;		do you need this guy
};

void BeginAssetFamily(GameAssets* ga, AssetFamilyType type)
{
	assert(!ga->currentAssetFamilyType);
	ga->currentAssetFamilyType = &ga->masterAssetFamilyTable[(int)type];

	AssetFamily* family = ga->currentAssetFamilyType;
	family->startAssetIndex = ga->numAssetHandles;   // does this work?
	family->onePastLastAssetIndex = family->startAssetIndex;

}

BitmapId AddBitmapInfo(GameAssets* ga, char* filename)
{
	assert(ga->numBitmaps < ga->maxBitmaps);
	BitmapId id = { ga->numBitmaps++ };

	AssetBitmapInfo* info = &ga->bitmapInfos[id.value];
	info->filename = filename;
	return id;
}

void AddBitmapAsset(GameAssets* ga, char* filename)
{
	assert(ga->currentAssetFamilyType);
	int index = ga->currentAssetFamilyType->onePastLastAssetIndex;

	// std::cout << "index " << index << std::endl;

	AssetHandle* handle = &ga->masterAssetHandleTable[index];
	ga->currentAssetFamilyType->onePastLastAssetIndex++;
	
	handle->startTagIndex = 0;
	handle->onePastLastTagIndex = 0;
	
	handle->assetId.bitmapId = AddBitmapInfo(ga, filename);
	ga->numAssetHandles++;
//	std::cout << "handle->assetId.bitmapId " << handle->assetId.bitmapId.value << std::endl;
}

void EndAssetFamily(GameAssets* ga)
{
//	std::cout << "onePastLastAssetIndex " << ga->currentAssetFamilyType->onePastLastAssetIndex << std::endl;

	assert(ga->currentAssetFamilyType);
	ga->currentAssetFamilyType = nullptr;

	
}

uint32 GetFirstAssetIdFrom(GameAssets* Assets, AssetFamilyType familyType)
{
	uint32 Result = 0;

	AssetFamily* family = &Assets->masterAssetFamilyTable[familyType];
	if (family->startAssetIndex != family->onePastLastAssetIndex)
	{
		Result = family->startAssetIndex;
	}

	return(Result);
}

BitmapId GetFirstBitmapIdFrom(GameAssets* assets, AssetFamilyType familyType)
{
	BitmapId result = { GetFirstAssetIdFrom(assets, familyType) };
	return(result);
}

LoadedBitmap* GetBitmap(GameAssets* assets, BitmapId id)
{
	LoadedBitmap* result = &assets->bitmaps[id.value].loadedBitmap;
	return result;
}

void AssetSystemLoadBitmap(GameAssets* ga, BitmapId id)
{
	AssetBitmapInfo* info = &ga->bitmapInfos[id.value];
	BitmapInfo bitmap = platformAPI.readImageFile(info->filename);

	Asset* asset = &ga->bitmaps[id.value];
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


void AllocateGameAssets(GameAssets* ga)
{
	// replace this with pushing it to the memory arena.
	ga->maxBitmaps = 256;
	ga->numBitmaps = 0;
	ga->bitmapInfos = new AssetBitmapInfo[ga->maxBitmaps];
	ga->bitmaps = new Asset[ga->maxBitmaps];

	// the 0th handle is just the null asset.
	ga->maxAssetHandles = ga->maxBitmaps;
	ga->numAssetHandles = 0;
	ga->masterAssetHandleTable = new AssetHandle[ga->maxAssetHandles];

	ga->numMasterTags = 0;

	// simulating this process of loading pack file off disk 
	BeginAssetFamily(ga, AssetFamilyType::None);
	AddBitmapAsset(ga, "./Assets/error.bmp");
	EndAssetFamily(ga);

	BeginAssetFamily(ga, AssetFamilyType::Default);
	AddBitmapAsset(ga, "./Assets/white.bmp");
	EndAssetFamily(ga);

	BeginAssetFamily(ga, AssetFamilyType::Wall);
	AddBitmapAsset(ga, "./Assets/wall2.bmp");
	AddBitmapAsset(ga, "./Assets/wall1.bmp");
	EndAssetFamily(ga);

	std::cout << "numBitmaps2 " << ga->numBitmaps << std::endl;
	std::cout << "maxBitmaps2 " << ga->maxBitmaps << std::endl;

	// texture 0
	for (int i = 0; i < ga->numBitmaps; i++)
	{
		BitmapId bitmapId = ga->masterAssetHandleTable[i].assetId.bitmapId;
		// std::cout << bitmapId.value << std::endl;
		AssetSystemLoadBitmap(ga, bitmapId);
	}
}


// defined in the gamecode
