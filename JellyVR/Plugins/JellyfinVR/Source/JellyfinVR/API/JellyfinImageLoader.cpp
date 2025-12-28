// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinImageLoader.h"
#include "JellyfinVRModule.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "Kismet/GameplayStatics.h"

UJellyfinImageLoader* UJellyfinImageLoader::Instance = nullptr;

UJellyfinImageLoader* UJellyfinImageLoader::Get(UObject* WorldContextObject)
{
	if (!Instance)
	{
		Instance = NewObject<UJellyfinImageLoader>();
		Instance->AddToRoot(); // Prevent garbage collection
	}
	return Instance;
}

void UJellyfinImageLoader::SetServerUrl(const FString& ServerUrl)
{
	JellyfinServerUrl = ServerUrl;
	if (JellyfinServerUrl.EndsWith(TEXT("/")))
	{
		JellyfinServerUrl = JellyfinServerUrl.LeftChop(1);
	}
}

void UJellyfinImageLoader::LoadItemImage(const FString& ItemId, const FOnImageLoaded& Callback, int32 MaxWidth, int32 MaxHeight)
{
	if (ItemId.IsEmpty() || JellyfinServerUrl.IsEmpty())
	{
		Callback.ExecuteIfBound(false, nullptr);
		return;
	}

	FString CacheKey = FString::Printf(TEXT("item_%s_%dx%d"), *ItemId, MaxWidth, MaxHeight);

	// Check memory cache first
	if (UTexture2D* CachedTexture = GetCachedImage(CacheKey))
	{
		Callback.ExecuteIfBound(true, CachedTexture);
		return;
	}

	// Build URL: /Items/{ItemId}/Images/Primary?maxWidth=X&maxHeight=Y
	FString Url = FString::Printf(TEXT("%s/Items/%s/Images/Primary?maxWidth=%d&maxHeight=%d&quality=90"),
		*JellyfinServerUrl, *ItemId, MaxWidth, MaxHeight);

	LoadImageFromUrl(Url, CacheKey, Callback);
}

void UJellyfinImageLoader::LoadBackdropImage(const FString& ItemId, const FOnImageLoaded& Callback, int32 MaxWidth, int32 MaxHeight)
{
	if (ItemId.IsEmpty() || JellyfinServerUrl.IsEmpty())
	{
		Callback.ExecuteIfBound(false, nullptr);
		return;
	}

	FString CacheKey = FString::Printf(TEXT("backdrop_%s_%dx%d"), *ItemId, MaxWidth, MaxHeight);

	// Check memory cache first
	if (UTexture2D* CachedTexture = GetCachedImage(CacheKey))
	{
		Callback.ExecuteIfBound(true, CachedTexture);
		return;
	}

	// Build URL: /Items/{ItemId}/Images/Backdrop?maxWidth=X&maxHeight=Y
	FString Url = FString::Printf(TEXT("%s/Items/%s/Images/Backdrop?maxWidth=%d&maxHeight=%d&quality=85"),
		*JellyfinServerUrl, *ItemId, MaxWidth, MaxHeight);

	LoadImageFromUrl(Url, CacheKey, Callback);
}

void UJellyfinImageLoader::LoadImageFromUrl(const FString& Url, const FString& CacheKey, const FOnImageLoaded& Callback)
{
	FString SafeKey = SanitizeCacheKey(CacheKey);

	// Check memory cache
	if (UTexture2D* CachedTexture = GetCachedImage(SafeKey))
	{
		Callback.ExecuteIfBound(true, CachedTexture);
		return;
	}

	// Check disk cache
	TArray<uint8> DiskData;
	if (LoadFromDiskCache(SafeKey, DiskData))
	{
		UTexture2D* Texture = CreateTextureFromBytes(DiskData);
		if (Texture)
		{
			// Add to memory cache
			MemoryCache.Add(SafeKey, Texture);
			CacheAccessTime.Add(SafeKey, FDateTime::UtcNow().ToUnixTimestamp());
			CacheImageSize.Add(SafeKey, DiskData.Num());
			CurrentCacheSizeBytes += DiskData.Num();

			Callback.ExecuteIfBound(true, Texture);
			OnAnyImageLoaded.Broadcast(SafeKey, Texture);
			return;
		}
	}

	// Check if request is already pending
	if (PendingRequests.Contains(SafeKey))
	{
		PendingRequests[SafeKey].Add(Callback);
		return;
	}

	// Start new request
	PendingRequests.Add(SafeKey, { Callback });

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->SetTimeout(30.0f);

	Request->OnProcessRequestComplete().BindLambda([this, SafeKey](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		OnHttpRequestComplete(Req, Res, bSuccess, SafeKey, FOnImageLoaded());
	});

	Request->ProcessRequest();
}

void UJellyfinImageLoader::OnHttpRequestComplete(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, FString CacheKey, FOnImageLoaded Callback)
{
	UTexture2D* Texture = nullptr;
	bool bSuccess = false;

	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
	{
		TArray<uint8> ImageData = Response->GetContent();

		if (ImageData.Num() > 0)
		{
			Texture = CreateTextureFromBytes(ImageData);

			if (Texture)
			{
				bSuccess = true;

				// Add to memory cache
				MemoryCache.Add(CacheKey, Texture);
				CacheAccessTime.Add(CacheKey, FDateTime::UtcNow().ToUnixTimestamp());
				CacheImageSize.Add(CacheKey, ImageData.Num());
				CurrentCacheSizeBytes += ImageData.Num();

				// Save to disk cache
				SaveToDiskCache(CacheKey, ImageData);

				// Enforce memory limit
				EnforceMemoryCacheLimit();
			}
		}
	}

	// Notify all pending callbacks for this key
	if (TArray<FOnImageLoaded>* Callbacks = PendingRequests.Find(CacheKey))
	{
		for (const FOnImageLoaded& PendingCallback : *Callbacks)
		{
			PendingCallback.ExecuteIfBound(bSuccess, Texture);
		}
		PendingRequests.Remove(CacheKey);
	}

	if (bSuccess)
	{
		OnAnyImageLoaded.Broadcast(CacheKey, Texture);
	}
}

UTexture2D* UJellyfinImageLoader::CreateTextureFromBytes(const TArray<uint8>& ImageData)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	// Detect image format
	EImageFormat Format = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());
	if (Format == EImageFormat::Invalid)
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("Could not detect image format"));
		return nullptr;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(Format);
	if (!ImageWrapper.IsValid())
	{
		return nullptr;
	}

	if (!ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("Failed to decompress image"));
		return nullptr;
	}

	TArray<uint8> RawData;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("Failed to get raw image data"));
		return nullptr;
	}

	int32 Width = ImageWrapper->GetWidth();
	int32 Height = ImageWrapper->GetHeight();

	// Create texture
	UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!Texture)
	{
		return nullptr;
	}

	// Lock and copy data
	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

	// Update texture
	Texture->UpdateResource();

	return Texture;
}

bool UJellyfinImageLoader::IsImageCached(const FString& CacheKey) const
{
	return MemoryCache.Contains(SanitizeCacheKey(CacheKey));
}

UTexture2D* UJellyfinImageLoader::GetCachedImage(const FString& CacheKey) const
{
	FString SafeKey = SanitizeCacheKey(CacheKey);

	if (UTexture2D* const* Found = MemoryCache.Find(SafeKey))
	{
		// Update access time for LRU
		const_cast<UJellyfinImageLoader*>(this)->CacheAccessTime.Add(SafeKey, FDateTime::UtcNow().ToUnixTimestamp());
		return *Found;
	}
	return nullptr;
}

void UJellyfinImageLoader::ClearMemoryCache()
{
	MemoryCache.Empty();
	CacheAccessTime.Empty();
	CacheImageSize.Empty();
	CurrentCacheSizeBytes = 0;
	UE_LOG(LogJellyfinVR, Log, TEXT("Memory cache cleared"));
}

void UJellyfinImageLoader::ClearAllCaches()
{
	ClearMemoryCache();

	// Clear disk cache
	FString CachePath = GetDiskCachePath();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.DeleteDirectoryRecursively(*CachePath);
	PlatformFile.CreateDirectory(*CachePath);

	UE_LOG(LogJellyfinVR, Log, TEXT("All caches cleared"));
}

void UJellyfinImageLoader::SetMaxCacheSize(int32 SizeMB)
{
	MaxCacheSizeMB = FMath::Max(16, SizeMB);
	EnforceMemoryCacheLimit();
}

float UJellyfinImageLoader::GetCurrentCacheSizeMB() const
{
	return CurrentCacheSizeBytes / (1024.0f * 1024.0f);
}

FString UJellyfinImageLoader::GetDiskCachePath() const
{
	return FPaths::ProjectSavedDir() / TEXT("JellyfinImageCache");
}

void UJellyfinImageLoader::SaveToDiskCache(const FString& CacheKey, const TArray<uint8>& ImageData)
{
	FString CachePath = GetDiskCachePath();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!PlatformFile.DirectoryExists(*CachePath))
	{
		PlatformFile.CreateDirectory(*CachePath);
	}

	FString FilePath = CachePath / (CacheKey + TEXT(".cache"));
	FFileHelper::SaveArrayToFile(ImageData, *FilePath);
}

bool UJellyfinImageLoader::LoadFromDiskCache(const FString& CacheKey, TArray<uint8>& OutImageData)
{
	FString FilePath = GetDiskCachePath() / (CacheKey + TEXT(".cache"));
	return FFileHelper::LoadFileToArray(OutImageData, *FilePath);
}

void UJellyfinImageLoader::EnforceMemoryCacheLimit()
{
	int64 MaxBytes = (int64)MaxCacheSizeMB * 1024 * 1024;

	while (CurrentCacheSizeBytes > MaxBytes && MemoryCache.Num() > 0)
	{
		// Find oldest accessed entry
		FString OldestKey;
		int64 OldestTime = INT64_MAX;

		for (const auto& Pair : CacheAccessTime)
		{
			if (Pair.Value < OldestTime)
			{
				OldestTime = Pair.Value;
				OldestKey = Pair.Key;
			}
		}

		if (!OldestKey.IsEmpty())
		{
			// Remove from cache
			MemoryCache.Remove(OldestKey);
			if (int32* Size = CacheImageSize.Find(OldestKey))
			{
				CurrentCacheSizeBytes -= *Size;
			}
			CacheAccessTime.Remove(OldestKey);
			CacheImageSize.Remove(OldestKey);
		}
		else
		{
			break;
		}
	}
}

FString UJellyfinImageLoader::SanitizeCacheKey(const FString& Key) const
{
	FString SafeKey = Key;
	SafeKey = SafeKey.Replace(TEXT("/"), TEXT("_"));
	SafeKey = SafeKey.Replace(TEXT("\\"), TEXT("_"));
	SafeKey = SafeKey.Replace(TEXT(":"), TEXT("_"));
	SafeKey = SafeKey.Replace(TEXT("?"), TEXT("_"));
	SafeKey = SafeKey.Replace(TEXT("&"), TEXT("_"));
	return SafeKey;
}
