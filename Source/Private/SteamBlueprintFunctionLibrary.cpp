// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamBlueprintFunctionLibrary.h"

#include "OnlineFriendsInterfaceSteam.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemModule.h"
#include "OnlineSubsystemSteam.h"
#include "OnlineSubsystemSteamTypes.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#define DEFAULT_LOCAL_USER_NUM 0

static IOnlineIdentityPtr GetIdentity()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	IOnlineSubsystem* System = OSS.GetOnlineSubsystem(STEAM_SUBSYSTEM);
	return System->GetIdentityInterface();
}

static IOnlineFriendsPtr GetFriends()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	IOnlineSubsystem* System = OSS.GetOnlineSubsystem(STEAM_SUBSYSTEM);
	return System->GetFriendsInterface();
}

FOnReadFriendsList USteamBlueprintFunctionLibrary::OnReadFriendsList;
FOnFriendInviteToGame USteamBlueprintFunctionLibrary::OnFriendInviteToGame;
FOnMicroTxnAuthorizationResponse USteamBlueprintFunctionLibrary::OnMicroTxnAuthorizationResponse;

bool USteamBlueprintFunctionLibrary::FirstGetInviterId = true;

bool USteamBlueprintFunctionLibrary::IsEnable()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	FOnlineSubsystemSteam* System = reinterpret_cast<FOnlineSubsystemSteam*>(OSS.GetOnlineSubsystem(STEAM_SUBSYSTEM));
	return System && System->IsEnabled();
}

bool USteamBlueprintFunctionLibrary::Init()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	FOnlineSubsystemSteam* System = reinterpret_cast<FOnlineSubsystemSteam*>(OSS.GetOnlineSubsystem(STEAM_SUBSYSTEM));
	if (System && System->IsEnabled())
	{
		System->OnSteamInvitedToGameDelegates.AddLambda([](FString InviteParam)
		{
			UE_LOG(LogTemp, Log, TEXT("OnSteamInvitedToGameDelegates %s"), *InviteParam);
			FString InviterId;
			FParse::Value(*InviteParam,TEXT("InviterId="), InviterId);
			OnFriendInviteToGame.Broadcast(InviterId);
		});
		System->OnSteamMicroTxnAuthorizationResponseDelegates.AddLambda([](uint64 nOrderId, bool bConfirm)
		{
			UE_LOG(LogTemp, Log, TEXT("OnSteamMicroTxnAuthorizationResponseDelegates %llu,%s"), nOrderId,
			       bConfirm?TEXT("true"):TEXT("false"));
			OnMicroTxnAuthorizationResponse.Broadcast(nOrderId, bConfirm);
		});
		return true;
	}
	return false;
}

FString USteamBlueprintFunctionLibrary::GetLanguage()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	FOnlineSubsystemSteam* System = reinterpret_cast<FOnlineSubsystemSteam*>(OSS.GetOnlineSubsystem(STEAM_SUBSYSTEM));
	if(System)
	{
		return System->GetLanguage();
	}
	return TEXT("");
}

FString USteamBlueprintFunctionLibrary::GetUILanguage()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	FOnlineSubsystemSteam* System = reinterpret_cast<FOnlineSubsystemSteam*>(OSS.GetOnlineSubsystem(STEAM_SUBSYSTEM));
	if(System)
	{
		return System->GetUILanguage();
	}
	return TEXT("");
}

bool USteamBlueprintFunctionLibrary::Login()
{
	if (!IsEnable())return false;
	UE_LOG(LogTemp, Log, TEXT("USteamBlueprintFunctionLibrary::Login"));
	return GetIdentity()->Login(DEFAULT_LOCAL_USER_NUM, FOnlineAccountCredentials());
}

FString USteamBlueprintFunctionLibrary::GetUserId()
{
	if (!IsEnable())return FString();
	UE_LOG(LogTemp, Log, TEXT("USteamBlueprintFunction+Library::GetUserId"));
	const TSharedPtr<const FUniqueNetId> UniqueId = GetIdentity()->GetUniquePlayerId(DEFAULT_LOCAL_USER_NUM);
	return UniqueId->ToString();
}

FString USteamBlueprintFunctionLibrary::GetNickname()
{
	if (!IsEnable())return FString();
	UE_LOG(LogTemp, Log, TEXT("USteamBlueprintFunctionLibrary::GetNickname"));
	return GetIdentity()->GetPlayerNickname(DEFAULT_LOCAL_USER_NUM);
}

FString USteamBlueprintFunctionLibrary::GetAuthToken()
{
	if (!IsEnable())return FString();
	UE_LOG(LogTemp, Log, TEXT("USteamBlueprintFunctionLibrary::GetAuthToken"));
	return GetIdentity()->GetAuthToken(DEFAULT_LOCAL_USER_NUM);
}

static TArray<TSharedRef<FOnlineFriend>> OutFriends;

void USteamBlueprintFunctionLibrary::ReadFriendList()
{
	if (!IsEnable())return;
	UE_LOG(LogTemp, Log, TEXT("USteamBlueprintFunctionLibrary::ReadFriendList"));
	GetFriends()->ReadFriendsList(DEFAULT_LOCAL_USER_NUM, EFriendsLists::ToString(EFriendsLists::Default),
	                              FOnReadFriendsListComplete::CreateLambda(
		                              [](int32 ReadFriendsListLocalUserNum, bool bReadFriendsListWasSuccessful,
		                                 const FString& ReadFriendsListListName,
		                                 const FString& ReadFriendsListErrorStr)
		                              {
			                              UE_LOG(LogTemp, Log,
			                                     TEXT("USteamBlueprintFunctionLibrary::ReadFriendListRsp %s"),
			                                     bReadFriendsListWasSuccessful?TEXT("true"):TEXT("false"));
			                              OnReadFriendsList.Broadcast(bReadFriendsListWasSuccessful);
		                              }));
}

int USteamBlueprintFunctionLibrary::GetFriendListCount()
{
	if (!IsEnable())return 0;
	UE_LOG(LogTemp, Log, TEXT("USteamBlueprintFunctionLibrary::GetFriendListCount"));
	OutFriends.Empty();
	GetFriends()->GetFriendsList(DEFAULT_LOCAL_USER_NUM, EFriendsLists::ToString(EFriendsLists::Default), OutFriends);
	return OutFriends.Num();
}

bool USteamBlueprintFunctionLibrary::GetFriend(const int Index, FString& OutAccount, FString& OutNickname,
                                               int& OutLevel,
                                               bool& OutIsOnline, bool& OutIsPlayThisGame)
{
	if (!IsEnable())return false;
	if (Index < 0 || Index >= OutFriends.Num())
	{
		return false;
	}
	FOnlineFriendSteam& FriendRef = static_cast<FOnlineFriendSteam&>(OutFriends[Index].Get());
	OutAccount = FriendRef.GetUserId()->ToString();
	OutNickname = FriendRef.GetDisplayName();
	OutIsOnline = FriendRef.GetPresence().bIsOnline;
	OutIsPlayThisGame = FriendRef.GetPresence().bIsPlayingThisGame;
	OutLevel = FriendRef.GetLevel();
	return true;
}

UObject* USteamBlueprintFunctionLibrary::GetFriendImage(const int Index)
{
	if (!IsEnable())return nullptr;
	if (Index < 0 || Index >= OutFriends.Num())
	{
		return nullptr;
	}
	FOnlineFriendSteam& FriendRef = static_cast<FOnlineFriendSteam&>(OutFriends[Index].Get());
	return FriendRef.GetUserImage();
}

bool USteamBlueprintFunctionLibrary::InviteFriend(const FString& InviterId, const FString& InviteeId)
{
	if (!IsEnable())return false;
	UE_LOG(LogTemp, Log, TEXT("USteamBlueprintFunctionLibrary::InviteFriend %s,%s"), *InviterId, *InviteeId);
	const FString NewInviterId = TEXT("InviterId=") + InviterId;
	return GetFriends()->SendInvite(DEFAULT_LOCAL_USER_NUM, FUniqueNetIdSteam(InviteeId), NewInviterId);
}

FString USteamBlueprintFunctionLibrary::GetInviterId()
{
	if (!IsEnable())return FString();
	FString InviterId;
	if (FirstGetInviterId)
	{
		FParse::Value(FCommandLine::Get(),TEXT("InviterId="), InviterId);
		FirstGetInviterId = false;
	}
	return InviterId;
}
