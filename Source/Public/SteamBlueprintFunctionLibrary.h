// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "UObject/Object.h"
#include "SteamBlueprintFunctionLibrary.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnReadFriendsList,bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFriendInviteToGame,FString);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMicroTxnAuthorizationResponse,uint64,bool);

/**
 * 
 */
UCLASS()
class ONLINESUBSYSTEMSTEAM_API USteamBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static bool FirstGetInviterId;
public:
	static FOnReadFriendsList OnReadFriendsList;
	static FOnFriendInviteToGame OnFriendInviteToGame;
	static FOnMicroTxnAuthorizationResponse OnMicroTxnAuthorizationResponse;
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static bool IsEnable();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static bool Init();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static FString GetLanguage();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static FString GetUILanguage();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static bool Login();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static FString GetUserId();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static FString GetNickname();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static FString GetAuthToken();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static void ReadFriendList();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static int GetFriendListCount();
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static bool GetFriend(const int Index, FString &OutAccount, FString &OutNickname,int &OutLevel, bool &OutIsOnline, bool &OutIsInGame);
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static UObject* GetFriendImage(const int Index);
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static bool InviteFriend(const FString& InviterId,const FString& InviteeId);
	UFUNCTION(BlueprintCallable, Category="SteamBlueprintFunctionLibrary")
	static FString GetInviterId();
};

