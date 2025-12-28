// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "JellyfinImageLoader.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnImageLoaded, bool, bSuccess, UTexture2D*, Texture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImageLoadedMulti, const FString&, ImageId, UTexture2D*, Texture);

/**
 * Image loader and cache for Jellyfin media artwork
 * Handles async download, memory caching, and disk caching
 */
UCLASS(BlueprintType)
class JELLYFINVR_API UJellyfinImageLoader : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Get the singleton instance
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Images", meta = (WorldContext = "WorldContextObject"))
	static UJellyfinImageLoader* Get(UObject* WorldContextObject);

	/**
	 * Set the Jellyfin server URL for image requests
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Images")
	void SetServerUrl(const FString& ServerUrl);

	/**
	 * Load an item's primary image (poster)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Images")
	void LoadItemImage(const FString& ItemId, const FOnImageLoaded& Callback, int32 MaxWidth = 400, int32 MaxHeight = 600);

	/**
	 * Load an item's backdrop image
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Images")
	void LoadBackdropImage(const FString& ItemId, const FOnImageLoaded& Callback, int32 MaxWidth = 1920, int32 MaxHeight = 1080);

	/**
	 * Load an image from URL directly
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Images")
	void LoadImageFromUrl(const FString& Url, const FString& CacheKey, const FOnImageLoaded& Callback);

	/**
	 * Check if image is in cache
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Images")
	bool IsImageCached(const FString& CacheKey) const;

	/**
	 * Get cached image (returns nullptr if not cached)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Images")
	UTexture2D* GetCachedImage(const FString& CacheKey) const;

	/**
	 * Clear memory cache (keeps disk cache)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Images")
	void ClearMemoryCache();

	/**
	 * Clear all caches including disk
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Images")
	void ClearAllCaches();

	/**
	 * Set maximum memory cache size in MB
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Images")
	void SetMaxCacheSize(int32 SizeMB);

	/**
	 * Get current memory cache size in MB
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Images")
	float GetCurrentCacheSizeMB() const;

	// Event fired when any image loads
	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnImageLoadedMulti OnAnyImageLoaded;

protected:
	void OnHttpRequestComplete(TSharedPtr<class IHttpRequest> Request, TSharedPtr<class IHttpResponse> Response, bool bWasSuccessful, FString CacheKey, FOnImageLoaded Callback);
	UTexture2D* CreateTextureFromBytes(const TArray<uint8>& ImageData);
	FString GetDiskCachePath() const;
	void SaveToDiskCache(const FString& CacheKey, const TArray<uint8>& ImageData);
	bool LoadFromDiskCache(const FString& CacheKey, TArray<uint8>& OutImageData);
	void EnforceMemoryCacheLimit();
	FString SanitizeCacheKey(const FString& Key) const;

private:
	FString JellyfinServerUrl;

	// Memory cache
	UPROPERTY()
	TMap<FString, UTexture2D*> MemoryCache;

	// Cache metadata for LRU eviction
	TMap<FString, int64> CacheAccessTime;
	TMap<FString, int32> CacheImageSize;

	int32 MaxCacheSizeMB = 256;
	int64 CurrentCacheSizeBytes = 0;

	// Pending requests to avoid duplicate downloads
	TMap<FString, TArray<FOnImageLoaded>> PendingRequests;

	static UJellyfinImageLoader* Instance;
};
