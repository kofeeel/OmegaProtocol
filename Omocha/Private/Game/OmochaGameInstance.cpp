// Copyright It's My Turn


#include "Game/OmochaGameInstance.h"
#include "GameAnalytics.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/PlayerState.h"
#include "Player/OmochaPlayerController.h"

void UOmochaGameInstance::Init()
{
	Super::Init();

	SetupDisplaySettings();

	InitGameAnalystic();

	LoadLevelData();
}

void UOmochaGameInstance::SetupDisplaySettings()
{
	if (UGameUserSettings* Settings = GEngine->GetGameUserSettings()) {
		Settings->SetFullscreenMode(EWindowMode::Type::Fullscreen);

		FIntPoint DesktopResolution = Settings->GetDesktopResolution();
		Settings->SetScreenResolution(DesktopResolution);

		Settings->ApplySettings(false);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("GameUserSettings NULL"));
	}
}

int32 UOmochaGameInstance::GetPlayerNumber(const FString& PlayerId) const
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		return 0;
	}
	if (Player2.PlayerId.Equals(PlayerId)) {
		return 1;
	}
	if (Player3.PlayerId.Equals(PlayerId)) {
		return 2;
	}
	return -1;
}

FGameplayTag UOmochaGameInstance::GetPlayerState(const FString& PlayerId)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		return Player1.State;
	}
	if (Player2.PlayerId.Equals(PlayerId)) {
		return Player2.State;
	}
	if (Player3.PlayerId.Equals(PlayerId)) {
		return Player3.State;
	}
	return FGameplayTag();
}

FString UOmochaGameInstance::GetPlayerStartTag(const FString& PlayerId)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		return Player1.DefaultStartTag;
	}
	if (Player2.PlayerId.Equals(PlayerId)) {
		return Player2.DefaultStartTag;
	}
	if (Player3.PlayerId.Equals(PlayerId)) {
		return Player3.DefaultStartTag;
	}
	return FString();
}

bool UOmochaGameInstance::IsPlayerAbilitySystemSaved(const FString& PlayerId) const
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		return /*Player1.bAbilitiesSaved && */Player1.bAttributesSaved;
	}
	if (Player2.PlayerId.Equals(PlayerId)) {
		return /*Player2.bAbilitiesSaved && */Player2.bAttributesSaved;
	}
	if (Player3.PlayerId.Equals(PlayerId)) {
		return /*Player3.bAbilitiesSaved && */Player3.bAttributesSaved;
	}
	return false;
}

bool UOmochaGameInstance::NewPlayer(const FPlayerInformation& PlayerInformation)
{
	if (!Player1.bPlayed) {
		Player1 = PlayerInformation;
		Player1.bPlayed = true;
		Player1.DefaultStartTag = "Player1";
		return true;
	}
	if (!Player2.bPlayed) {
		Player2 = PlayerInformation;
		Player2.bPlayed = true;
		Player2.DefaultStartTag = "Player2";
		return true;
	}
	if (!Player3.bPlayed) {
		Player3 = PlayerInformation;
		Player3.bPlayed = true;
		Player3.DefaultStartTag = "Player3";
		return true;
	}
	return false;
}

void UOmochaGameInstance::LogoutPlayer(const FString& PlayerId)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		Player1.bPlayed = false;
		Player1.PlayerId.Empty();
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Player2.bPlayed = false;
		Player2.PlayerId.Empty();
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Player3.bPlayed = false;
		Player3.PlayerId.Empty();
	}
}

bool UOmochaGameInstance::CheckPlayerExist(const FString& PlayerId) const
{
	if ((Player1.PlayerId.Equals(PlayerId))
		|| (Player2.PlayerId.Equals(PlayerId))
		|| (Player3.PlayerId.Equals(PlayerId))) {
		return true;
	}
	return false;
}

bool UOmochaGameInstance::ReconnectPlayer(const FPlayerInformation& PlayerInformation)
{
	if (Player1.PlayerId.Equals(PlayerInformation.PlayerId)) {
		Player1.bPlayed = true;
		Player1.PlayerController = PlayerInformation.PlayerController;
		Player1.DefaultStartTag = "Player1";
		Player1.Winner = false;
		return true;
	}
	if (Player2.PlayerId.Equals(PlayerInformation.PlayerId)) {
		Player2.bPlayed = true;
		Player2.PlayerController = PlayerInformation.PlayerController;
		Player2.DefaultStartTag = "Player2";
		Player2.Winner = false;
		return true;
	}
	if (Player3.PlayerId.Equals(PlayerInformation.PlayerId)) {
		Player3.bPlayed = true;
		Player3.PlayerController = PlayerInformation.PlayerController;
		Player3.DefaultStartTag = "Player3";
		Player3.Winner = false;
		return true;
	}
	return false;
}

void UOmochaGameInstance::AllPlayerBroadcastLobbyLogin(const FString& PlayerId)
{
	int32 PlayerNumber = -1;
	FString PlayerName = FString();
	FString PlayerSteamId = FString();

	if (Player1.PlayerId.Equals(PlayerId)) {
		PlayerNumber = 0;
		PlayerName = Player1.PlayerName;
		PlayerSteamId = Player1.PlayerId;
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		PlayerNumber = 1;
		PlayerName = Player2.PlayerName;
		PlayerSteamId = Player2.PlayerId;
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		PlayerNumber = 2;
		PlayerName = Player3.PlayerName;
		PlayerSteamId = Player3.PlayerId;
	}
	if (PlayerNumber == -1) {
		return;
	}
	if (Player1.PlayerController) {
		Player1.PlayerController->Client_PlayerLogin(PlayerNumber, PlayerName, PlayerSteamId);
	}
	if (Player2.PlayerController) {
		Player2.PlayerController->Client_PlayerLogin(PlayerNumber, PlayerName, PlayerSteamId);
	}
	if (Player3.PlayerController) {
		Player3.PlayerController->Client_PlayerLogin(PlayerNumber, PlayerName, PlayerSteamId);
	}
}

void UOmochaGameInstance::AllPlayerBroadcastLobbyCharacter(const FString& PlayerId, const ECharacterType Character)
{
	const int32 PlayerNumber = GetPlayerNumber(PlayerId);
	if (Player1.PlayerController) {
		Player1.PlayerController->Client_PlayerCharacterChanged(PlayerNumber, Character);
	}
	if (Player2.PlayerController) {
		Player2.PlayerController->Client_PlayerCharacterChanged(PlayerNumber, Character);
	}
	if (Player3.PlayerController) {
		Player3.PlayerController->Client_PlayerCharacterChanged(PlayerNumber, Character);
	}
}

void UOmochaGameInstance::AllPlayerBroadcastLobbyReady(const FString& PlayerId, const bool Ready)
{
	const int32 PlayerNumber = GetPlayerNumber(PlayerId);
	if (Player1.PlayerController) {
		Player1.PlayerController->Client_PlayerReady(PlayerNumber, Ready);
	}
	if (Player2.PlayerController) {
		Player2.PlayerController->Client_PlayerReady(PlayerNumber, Ready);
	}
	if (Player3.PlayerController) {
		Player3.PlayerController->Client_PlayerReady(PlayerNumber, Ready);
	}
}

void UOmochaGameInstance::AllPlayerBroadcastPlayGameOver()
{
	if (Player1.PlayerController) {
		Player1.PlayerController->Client_GameOver(Player1.Winner);
	}
	if (Player2.PlayerController) {
		Player2.PlayerController->Client_GameOver(Player2.Winner);
	}
	if (Player3.PlayerController) {
		Player3.PlayerController->Client_GameOver(Player3.Winner);
	}
}

void UOmochaGameInstance::AllPlayerBroadcastScoreState(const FString& PlayerId, const EPlayerRestartState& State)
{
	const int32 PlayerNumber = GetPlayerNumber(PlayerId);
	if (Player1.PlayerController) {
		Player1.PlayerController->Client_PlayerRestartState(PlayerNumber, State);
	}
	if (Player2.PlayerController) {
		Player2.PlayerController->Client_PlayerRestartState(PlayerNumber, State);
	}
	if (Player3.PlayerController) {
		Player3.PlayerController->Client_PlayerRestartState(PlayerNumber, State);
	}
}

void UOmochaGameInstance::AllPlayerBroadcastChatMessage(const FString& Message)
{
	if (Player1.PlayerController)
	{
		Player1.PlayerController->Client_AddChatMessage(Message);
	}
	if (Player2.PlayerController)
	{
		Player2.PlayerController->Client_AddChatMessage(Message);
	}
	if (Player3.PlayerController)
	{
		Player3.PlayerController->Client_AddChatMessage(Message);
	}
}

void UOmochaGameInstance::SavePlayerState(const FString& PlayerId, const FGameplayTag& PlayerState)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		Player1.State = PlayerState;
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Player2.State = PlayerState;
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Player3.State = PlayerState;
	}
}

void UOmochaGameInstance::SavePlayerCharacterClass(const FString& PlayerId,
                                                   const TSubclassOf<AOmochaPlayerCharacter>& CharacterClass)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		Player1.PlayerCharacterClass = CharacterClass;
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Player2.PlayerCharacterClass = CharacterClass;
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Player3.PlayerCharacterClass = CharacterClass;
	}
}

TSubclassOf<AOmochaPlayerCharacter> UOmochaGameInstance::GetPlayerCharacterClass(const FString& PlayerId)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		return Player1.PlayerCharacterClass;
	}
	if (Player2.PlayerId.Equals(PlayerId)) {
		return Player2.PlayerCharacterClass;
	}
	if (Player3.PlayerId.Equals(PlayerId)) {
		return Player3.PlayerCharacterClass;
	}
	return nullptr;
}

void UOmochaGameInstance::SavePlayerAttributes(const FString& PlayerId, const FSavedAttributes& SavedAttributes)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		Player1.SavedAttributes = SavedAttributes;
		Player1.bAttributesSaved = true;
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Player2.SavedAttributes = SavedAttributes;
		Player2.bAttributesSaved = true;
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Player3.SavedAttributes = SavedAttributes;
		Player3.bAttributesSaved = true;
	}
}

FSavedAttributes UOmochaGameInstance::GetPlayerAttributes(const FString& PlayerId) const
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		return Player1.SavedAttributes;
	}
	if (Player2.PlayerId.Equals(PlayerId)) {
		return Player2.SavedAttributes;
	}
	if (Player3.PlayerId.Equals(PlayerId)) {
		return Player3.SavedAttributes;
	}
	return FSavedAttributes();
}

FGameScoreBoardData UOmochaGameInstance::GetPlayerScoreData(const FString& PlayerId) const
{
	FGameScoreBoardData Data = FGameScoreBoardData();
	if (Player1.PlayerId.Equals(PlayerId)) {
		Data.Winner = Player1.Winner;
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Data.Winner = Player2.Winner;
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Data.Winner = Player3.Winner;
	}
	return Data;
}

FDataTableRowHandle UOmochaGameInstance::GetEquippedWeaponRow(const FString& PlayerId) const
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		return Player1.EquippedWeaponRow;
	}
	if (Player2.PlayerId.Equals(PlayerId)) {
		return Player2.EquippedWeaponRow;
	}
	if (Player3.PlayerId.Equals(PlayerId)) {
		return Player3.EquippedWeaponRow;
	}
	return FDataTableRowHandle();
}

void UOmochaGameInstance::InitGameAnalystic()
{
	UGameAnalytics::initialize(
		"cea1ee90f2453943ccadc178ab2429ea",
		"e1a5d6550a14bd43731837afb916e3ec2af1a639"
	);

#if WITH_EDITOR
	UGameAnalytics::setEnabledInfoLog(true);
	UGameAnalytics::setEnabledVerboseLog(true);
#endif
}

void UOmochaGameInstance::InitializeAllOfPlayerInfos()
{
	Player1 = FPlayerInformation();
	Player2 = FPlayerInformation();
	Player3 = FPlayerInformation();
}

void UOmochaGameInstance::EmptyPlayerAbilities(const FString& PlayerId)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		Player1.SavedAbilities.Empty();
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Player2.SavedAbilities.Empty();
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Player3.SavedAbilities.Empty();
	}
}

void UOmochaGameInstance::SavePlayerAbility(const FString& PlayerId, const FSavedAbility& SavedAbility)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		Player1.SavedAbilities.AddUnique(SavedAbility);
		Player1.bAbilitiesSaved = true;
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Player2.SavedAbilities.AddUnique(SavedAbility);
		Player2.bAbilitiesSaved = true;
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Player3.SavedAbilities.AddUnique(SavedAbility);
		Player3.bAbilitiesSaved = true;
	}
}

void UOmochaGameInstance::SavePlayerWeaponRow(const FString& PlayerId, const FDataTableRowHandle& WeaponRow)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		Player1.EquippedWeaponRow = WeaponRow;
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Player2.EquippedWeaponRow = WeaponRow;
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Player3.EquippedWeaponRow = WeaponRow;
	}
}

TArray<FSavedAbility> UOmochaGameInstance::GetPlayerAbilities(const FString& PlayerId) const
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		return Player1.SavedAbilities;
	}
	if (Player2.PlayerId.Equals(PlayerId)) {
		return Player2.SavedAbilities;
	}
	if (Player3.PlayerId.Equals(PlayerId)) {
		return Player3.SavedAbilities;
	}
	return TArray<FSavedAbility>();
}

void UOmochaGameInstance::ForAlarmPrepareForNextLevel(const bool bReady) const
{
	if (Player1.PlayerController) {
		Player1.PlayerController->Client_GetPrepareForNextLevel(bReady);
	}
	if (Player2.PlayerController) {
		Player2.PlayerController->Client_GetPrepareForNextLevel(bReady);
	}
	if (Player3.PlayerController) {
		Player3.PlayerController->Client_GetPrepareForNextLevel(bReady);
	}
}

FString UOmochaGameInstance::GetUniqueId(const APlayerState* PlayerState)
{
	if (IsValid(PlayerState)) {
		const FUniqueNetIdRepl& UniqueIdWrapper = PlayerState->GetUniqueId();
		const TSharedPtr<const FUniqueNetId> UniqueIdPtr = UniqueIdWrapper.GetUniqueNetId();
		if (UniqueIdPtr.IsValid()) {
			return UniqueIdPtr->ToString();
		}
	}
	return FString();
}

void UOmochaGameInstance::SetWinner(const FString& PlayerId)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		Player1.Winner = true;
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Player2.Winner = true;
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Player3.Winner = true;
	}
}

void UOmochaGameInstance::SetLooser(const FString& PlayerId)
{
	if (Player1.PlayerId.Equals(PlayerId)) {
		Player1.Winner = false;
	}
	else if (Player2.PlayerId.Equals(PlayerId)) {
		Player2.Winner = false;
	}
	else if (Player3.PlayerId.Equals(PlayerId)) {
		Player3.Winner = false;
	}
}

void UOmochaGameInstance::NeverWin()
{
	Player1.Winner = false;
	Player2.Winner = false;
	Player3.Winner = false;
}

void UOmochaGameInstance::UpdateMainTainData(int32 NewLevel, float NewXP)
{
	MainTainTeamLevel = NewLevel;
	MainTainTeamXP = NewXP;

	SaveLevelData();
}

void UOmochaGameInstance::GetMainTainLevelData(int32& OutLevel, float& OutXP) const
{
	OutLevel = MainTainTeamLevel;
	OutXP = MainTainTeamXP;
}

void UOmochaGameInstance::ShowMainTainLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("=== MainTain LEVEL ==="));
	UE_LOG(LogTemp, Warning, TEXT("MainTain Level: %d"), MainTainTeamLevel);
	UE_LOG(LogTemp, Warning, TEXT("MainTain XP: %lf"), MainTainTeamXP);
}

void UOmochaGameInstance::ResetMainTainLevel()
{
	MainTainTeamLevel = 1;
	MainTainTeamXP = 0;
}

void UOmochaGameInstance::LoadLevelData()
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance: Level data loaded"));
}

void UOmochaGameInstance::SaveLevelData()
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance: Level data saved"));
}
