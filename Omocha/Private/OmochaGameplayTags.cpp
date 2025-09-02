#include "OmochaGameplayTags.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsSettings.h"

FOmochaGameplayTags FOmochaGameplayTags::GameplayTags;

void FOmochaGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 * Character Tags
	 */
	GameplayTags.Player = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player"), FString("Player"));
	GameplayTags.Enemy = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Enemy"), FString("Enemy"));

	// Character Type Tags
	GameplayTags.Character_Type_Zenith = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Character.Type.Zenith"),
		FString("Zenith character type")
	);

	GameplayTags.Character_Type_Vulcan = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Character.Type.Vulcan"),
		FString("Vulcan character type")
	);

	GameplayTags.Character_Type_Rusty = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Character.Type.Rusty"),
		FString("Rusty character type")
	);

	// Character State Tags
	GameplayTags.State_Omega = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Omega"),
		FString("Omega state - enhanced abilities")
	);

	GameplayTags.State_Normal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Normal"),
		FString("Normal state - default abilities")
	);

	GameplayTags.State_Dead = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Dead"),
		FString("Dead state - character is dead")
	);

	GameplayTags.State_Attacking = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Attacking"),
		FString("Attacking state - default abilities")
	);
	
	GameplayTags.State_RevivingWithOmega = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.RevivingWithOmega"),
		FString("Dead state, but waiting to be revived with Omega")
	);

	/*
	 * Ability Tags
	 */
	// Input Tags 
	GameplayTags.InputTag_Ability_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Ability.Q"),
		FString("Input Tag for Q key ability")
	);

	GameplayTags.InputTag_Ability_R = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Ability.R"),
		FString("Input Tag for R key ability")
	);

	GameplayTags.InputTag_Ability_LeftShift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Ability.LeftShift"),
		FString("Input Tag for Left Shift ability")
	);

	GameplayTags.InputTag_Ability_Spacebar = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Ability.Spacebar"),
		FString("Input Tag for Spacebar ability")
	);

	GameplayTags.InputTag_Ability_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Ability.LMB"),
		FString("Input Tag for Left Mouse Button ability")
	);

	GameplayTags.InputTag_Ability_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Ability.RMB"),
		FString("Input Tag for Right Mouse Button ability")
	);

	GameplayTags.InputTag_Interact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Interact"),
		FString("Input Tag for F key interaction")
	);

	// Shield Abilities
	GameplayTags.Ability_Shield_Block = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Shield.Block"),
		FString("Block with shield")
	);
	GameplayTags.Ability_Shield_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Shield.Dash"),
		FString("Dash while blocking")
	);

	// Normal Abilities
	GameplayTags.Ability_Normal_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Skill_RMB"),
		FString("Normal RMB skill ability")
	);

	GameplayTags.Ability_Normal_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Skill_Q"),
		FString("Normal Q skill ability")
	);

	GameplayTags.Ability_Normal_Steal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Steal"),
		FString("Normal steal ability")
	);

	GameplayTags.Ability_Normal_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.HitReact"),
		FString("Normal hit reaction ability")
	);

	GameplayTags.Ability_Normal_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Attack"),
		FString("Normal attack ability")
	);

	// Normal Abilities - Zenith
	GameplayTags.Ability_Normal_Zenith_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Zenith.Attack"),
		FString("Zentih Normal Basic Attack")
	);

	GameplayTags.Ability_Normal_Zenith_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Zenith.Skill_Q"),
		FString("Zentih normal skill attack")
	);

	GameplayTags.Ability_Normal_Zenith_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Zenith.Skill_RMB"),
		FString("Zentih normal skill move")
	);

	// Normal Abilities - Vulcan
	GameplayTags.Ability_Normal_Vulcan_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Vulcan.Attack"),
		FString("Vulcan Normal Basic Attack")
	);

	GameplayTags.Ability_Normal_Vulcan_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Vulcan.Skill_Q"),
		FString("Vulcan normal skill attack")
	);

	GameplayTags.Ability_Normal_Vulcan_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Vulcan.Skill_RMB"),
		FString("Vulcan normal skill move")
	);

	// Normal Abilities - Rusty
	GameplayTags.Ability_Normal_Rusty_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Rusty.Attack"),
		FString("Rusty Normal Basic Attack")
	);

	GameplayTags.Ability_Normal_Rusty_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Rusty.Skill_Q"),
		FString("Rusty normal skill attack")
	);

	GameplayTags.Ability_Normal_Rusty_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Normal.Rusty.Skill_RMB"),
		FString("Rusty normal skill move")
	);

	// Omega Abilities - Zenith
	GameplayTags.Ability_Omega_Zenith_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Zenith.Attack"),
		FString("Zenith Omega Q skill ability")
	);

	GameplayTags.Ability_Omega_Zenith_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Zenith.Skill_Q"),
		FString("Zenith Omega attack ability")
	);

	GameplayTags.Ability_Omega_Zenith_Skill_LeftShift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Zenith.Skill_LeftShift"),
		FString("Zenith Omega Left Shift skill ability")
	);

	GameplayTags.Ability_Omega_Zenith_Skill_R = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Zenith.Skill_R"),
		FString("Zenith Omega R skill ability")
	);

	GameplayTags.Ability_Omega_Zenith_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Zenith.Skill_RMB"),
		FString("Zenith Omega RMB skill ability")
	);

	// Omega Abilities - Vulcan
	GameplayTags.Ability_Omega_Vulcan_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Vulcan.Attack"),
		FString("Vulcan Omega attack ability")
	);

	GameplayTags.Ability_Omega_Vulcan_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Vulcan.Skill_Q"),
		FString("Vulcan Omega Q skill ability")
	);

	GameplayTags.Ability_Omega_Vulcan_Skill_LeftShift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Vulcan.Skill_LeftShift"),
		FString("Vulcan Omega Left Shift skill ability")
	);

	GameplayTags.Ability_Omega_Vulcan_Skill_R = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Vulcan.Skill_R"),
		FString("Vulcan Omega R skill ability")
	);

	GameplayTags.Ability_Omega_Vulcan_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Vulcan.Skill_RMB"),
		FString("Vulcan Omega RMB skill ability")
	);

	// Omega Abilities - Rusty
	GameplayTags.Ability_Omega_Rusty_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Rusty.Attack"),
		FString("Rusty Omega attack ability")
	);

	GameplayTags.Ability_Omega_Rusty_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Rusty.Skill_Q"),
		FString("Rusty Omega Q skill ability")
	);

	GameplayTags.Ability_Omega_Rusty_Skill_E = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Rusty.Skill_E"),
		FString("Rusty Omega E skill ability")
	);

	GameplayTags.Ability_Omega_Rusty_Skill_R = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Rusty.Skill_R"),
		FString("Rusty Omega R skill ability")
	);

	GameplayTags.Ability_Omega_Rusty_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Rusty.Skill_RMB"),
		FString("Rusty Omega RMB skill ability")
	);

	// Enemy Abilities
	GameplayTags.Ability_Enemy_Zenith_Skill_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Zenith.Skill.BasicAttack"),
		FString("Zenith Skill ability")
	);

	GameplayTags.Ability_Enemy_Zenith_Skill_ChainLightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Zenith.Skill.ChainLightning"),
		FString("Zenith Enemy ChainLightning skill ability")
	);

	GameplayTags.Ability_Enemy_Zenith_Skill_Teleport = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Zenith.Skill.Teleport"),
		FString("Zenith Enemy Teleport skill ability")
	);

	GameplayTags.Ability_Enemy_Zenith_Skill_DisCharge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Zenith.Skill.DisCharge"),
		FString("Zenith Enemy DisCharge skill ability")
	);

	GameplayTags.Ability_Enemy_Zenith_Skill_Charge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Zenith.Skill.Charge"),
		FString("Zenith Enemy Charge ability")
	);

	GameplayTags.Ability_Enemy_Vulcan_Skill_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Vulcan.Skill.BasicAttack"),
		FString("Vulcan Skill ability")
	);

	GameplayTags.Ability_Enemy_Vulcan_Skill_AttackSpeedBuff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Vulcan.Skill.AttackSpeedBuff"),
		FString("Vulcan Enemy AttackSpeedBuff skill ability")
	);

	GameplayTags.Ability_Enemy_Vulcan_Skill_MoveSpeedBuff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Vulcan.Skill.MoveSpeedBuff"),
		FString("Vulcan Enemy MoveSpeedBuff skill ability")
	);

	GameplayTags.Ability_Enemy_Vulcan_Skill_Grenade = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Vulcan.Skill.Grenade"),
		FString("Vulcan Enemy Grenade skill ability")
	);

	GameplayTags.Ability_Enemy_Vulcan_Skill_LinerGrenade = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Vulcan.Skill.LinerGrenade"),
		FString("Vulcan Enemy LinerGrenade ability")
	);

	GameplayTags.Ability_Enemy_Rusty_Skill_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Rusty.Skill.BasicAttack"),
		FString("Rusty Enemy BasicAttack ability")
	);

	GameplayTags.Ability_Enemy_Rusty_Skill_Grap = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Rusty.Skill.Grap"),
		FString("Rusty Enemy Grap ability")
	);

	GameplayTags.Ability_Enemy_Rusty_Skill_BlockDash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Rusty.Skill.BlockDash"),
		FString("Rusty Enemy BlockDash ability")
	);

	GameplayTags.Ability_Enemy_Rusty_Skill_Shield = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Rusty.Skill.Shield"),
		FString("Rusty Enemy Shield ability")
	);

	GameplayTags.Ability_Enemy_Rusty_Skill_UnShield = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Rusty.Skill.UnShield"),
		FString("Rusty Enemy UnShield ability")
	);

	GameplayTags.Ability_Enemy_BlazedCannon_Skill_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.BlazedCannon.Skill.BasicAttack"),
		FString("BlazedCannon Enemy BasicAttack ability")
	);

	GameplayTags.Ability_Enemy_BlazedCannon_Skill_Back = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.BlazedCannon.Skill.Back"),
		FString("BlazedCannon Enemy Back ability")
	);

	GameplayTags.Ability_Enemy_BlazedCannon_Skill_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.BlazedCannon.Skill.Fire"),
		FString("BlazedCannon Enemy Fire ability")
	);

	GameplayTags.Ability_Enemy_BlazedCannon_Skill_BigBomb = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.BlazedCannon.Skill.BigBomb"),
		FString("BlazedCannon Enemy BigBomb ability")
	);

	GameplayTags.Ability_Enemy_BlazedCannon_Skill_KnockBackAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.BlazedCannon.Skill.KnockBackAttack"),
		FString("BlazedCannon Enemy KnockBackAttack ability")
	);

	GameplayTags.Ability_Enemy_BlazedCannon_Skill_HealBuff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.BlazedCannon.Skill.HealBuff"),
		FString("BlazedCannon Enemy HealBuff ability")
	);

	// Glitch Type Abilities
	GameplayTags.Ability_Enemy_Glitch_Skill_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Glitch.Skill.BasicAttack"),
		FString("Glitch Enemy BasicAttack ability")
	);

	GameplayTags.Ability_Enemy_Glitch_Skill_Buff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Glitch.Skill.Buff"),
		FString("Glitch Enemy Buff ability")
	);

	GameplayTags.Ability_Enemy_Glitch_Skill_Teleport = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Glitch.Skill.Teleport"),
		FString("Glitch Enemy Teleport ability")
	);

	GameplayTags.Ability_Enemy_Glitch_Skill_Gravity = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Glitch.Skill.Gravity"),
		FString("Glitch Enemy Gravity ability")
	);

	GameplayTags.Ability_Enemy_Glitch_Skill_Charge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Glitch.Skill.Charge"),
		FString("Glitch Enemy Gravity Charge")
	);

	// Screamer Type Abilities
	GameplayTags.Ability_Enemy_Screamer_Skill_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Screamer.Skill.BasicAttack"),
		FString("Screamer Enemy BasicAttack Ability")
	);

	GameplayTags.Ability_Enemy_Screamer_Skill_Tackle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Screamer.Skill.Tackle"),
		FString("Screamer Enemy Tackle Ability")
	);

	GameplayTags.Ability_Enemy_Screamer_Skill_Ambush = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Screamer.Skill.Ambush"),
		FString("Screamer Enemy Ambush Ability")
	);

	GameplayTags.Ability_Enemy_Screamer_Skill_Roar = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Screamer.Skill.Roar"),
		FString("Screamer Enemy Roar Ability")
	);

	GameplayTags.Ability_Enemy_Screamer_Skill_Jump = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Screamer.Skill.Jump"),
		FString("Screamer Enemy Jump Ability")
	);

	GameplayTags.Ability_Enemy_Screamer_Skill_Roar = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Screamer.Skill.Roar"),
		FString("Screamer Enemy Roar Ability")
	);

	GameplayTags.Ability_Enemy_Screamer_Skill_Shield = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Screamer.Skill.Shield"),
		FString("Screamer Enemy Shield Ability")
	);

	GameplayTags.Ability_Enemy_Screamer_Skill_Grab = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Enemy.Screamer.Skill.Grab"),
		FString("Screamer Enemy Grab Ability")
	);

	// Death
	GameplayTags.Ability_Death = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Death"),
		FString("Death ability")
	);

	// Omega Hit React
	GameplayTags.Ability_Omega_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.HitReact"),
		FString("Omega hit reaction ability")
	);

	// Omega Shared Abilities
	GameplayTags.Ability_Omega_Give = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Omega.Give"),
		FString("Omega Give ability")
	);

	/*
	 * Cooldown Tags
	 */
	GameplayTags.CoolDown_Ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CoolDown.Ability.Attack"),
		FString("Attack ability cooldown")
	);

	GameplayTags.CoolDown_Ability_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CoolDown.Ability.Dash"),
		FString("Dash ability cooldown")
	);

	GameplayTags.CoolDown_Ability_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CoolDown.Normal.Steal"),
		FString("Omega Steal ability cooldown")
	);

	GameplayTags.CoolDown_Ability_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CoolDown.Omega.Give"),
		FString("Omega Give ability cooldown")
	);

	// Cooldown - Normal
	GameplayTags.Cooldown_Normal_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Normal.Skill.Q"),
		FString("Norma Skill Q cooldown")
	);
	GameplayTags.Cooldown_Normal_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Normal.Skill.RMB"),
		FString("Norma Skill RMB cooldown")
	);

	// Cooldown - Omega
	GameplayTags.Cooldown_Omega_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Omega.Skill.Q"),
		FString("Omega Skill Q cooldown")
	);
	GameplayTags.Cooldown_Omega_Skill_LeftShift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Omega.Skill.LeftShift"),
		FString("Omega Skill LeftShift cooldown")
	);
	GameplayTags.Cooldown_Omega_Skill_R = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Omega.Skill.R"),
		FString("Omega Skill R cooldown")
	);
	GameplayTags.Cooldown_Omega_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Omega.Skill.RMB"),
		FString("Omega Skill RMB cooldown")
	);

	/*
	 * Animation & Mesh Tags
	 */
	// Event Montage Tags
	GameplayTags.Montage_Shoot = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Shoot"),
		FString("Shoot montage event")
	);

	GameplayTags.Montage_MeleeAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.MeleeAttack"),
		FString("Melee montage event")
	);

	GameplayTags.Montage_GetHit = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.GetHit"),
		FString("Get hit montage event")
	);

	GameplayTags.Montage_Dead = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Dead"),
		FString("Death montage event")
	);

	GameplayTags.Montage_Jump = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Jump"),
		FString("Jump montage event")
	);

	GameplayTags.Montage_Roar = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Roar"),
		FString("Roar montage event")
	);

	GameplayTags.Montage_End = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.End"),
		FString("End montage event")
	);

	GameplayTags.Montage_Attack1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack1"),
		FString("Attack1 montage event")
	);

	GameplayTags.Montage_Attack1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack2"),
		FString("Attack2 montage event")
	);

	GameplayTags.Montage_Attack1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack3"),
		FString("Attack3 montage event")
	);

	// Transform Montage Tags
	GameplayTags.Montage_Transform_Zenith = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Transform.Zenith"),
		FString("Transform to Zenith montage")
	);

	GameplayTags.Montage_Transform_Vulcan = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Transform.Vulcan"),
		FString("Transform to Vulcan montage")
	);

	GameplayTags.Montage_Transform_Rusty = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Transform.Rusty"),
		FString("Transform to Rusty montage")
	);

	// Emote Montage Tags
	GameplayTags.Montage_Emote_Dance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Emote.Dance"),
		FString("Dance emote montage")
	);

	GameplayTags.Montage_Emote_Wave = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Emote.Wave"),
		FString("Wave emote montage")
	);

	// Socket Tags
	GameplayTags.Socket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Socket.Weapon"),
		FString("Weapon socket attachment point")
	);

	GameplayTags.Socket_Arm = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Socket.Arm"),
		FString("Arm socket attachment point")
	);

	GameplayTags.Socket_Hand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Socket.Hand"),
		FString("Hand socket attachment point")
	);

	GameplayTags.Socket_HandL = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Socket.HandL"),
		FString("Left Hand socket attachment point")
	);

	GameplayTags.Socket_HandR = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Socket.HandR"),
		FString("Right Hand socket attachment point")
	);

	GameplayTags.Socket_Head = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Socket.Head"),
		FString("Head socket attachment point")
	);

	/*
	 * Attributes Tags 
	 */
	// Current Attributes
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.Health"),
		FString("Current health amount")
	);

	GameplayTags.Attributes_Vital_Omega = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.Omega"),
		FString("Current Omega amount")
	);

	GameplayTags.Attributes_Current_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.Resilience"),
		FString("Health regeneration rate")
	);

	GameplayTags.Attributes_Current_AttackDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.AttackDamage"),
		FString("Basic attack damage")
	);

	GameplayTags.Attributes_Current_SkillDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.SkillDamage"),
		FString("Skill damage multiplier")
	);

	GameplayTags.Attributes_Current_Shield = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.Shield"),
		FString("Shield defense value")
	);

	GameplayTags.Attributes_Current_MoveSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.MoveSpeed"),
		FString("Movement speed")
	);

	GameplayTags.Attributes_Current_AttackSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.AttackSpeed"),
		FString("Attack speed multiplier")
	);

	GameplayTags.Attributes_Current_CoolDownReduction = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.CoolDownReduction"),
		FString("Cooldown reduction percentage")
	);

	GameplayTags.Attributes_Current_AttackRange = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.AttackRange"),
		FString("Projectile attack range")
	);

	GameplayTags.Attributes_Current_Level = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.Level"),
		FString("Character level")
	);

	GameplayTags.Attributes_Current_XP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.XP"),
		FString("Character Current XP")
	);

	GameplayTags.Attributes_Current_KnockbackResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.KnockbackResistance"),
		FString("Knockback resistance")
	);

	GameplayTags.Attributes_Current_ChargeLevel = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.ChargeLevel"),
		FString("Charge Level")
	);

	GameplayTags.Attributes_Current_Omega = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.Omega"),
		FString("Currently it is Omega")
	);

	// Max Attributes
	GameplayTags.Attributes_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.MaxHealth"),
		FString("Maximum health amount")
	);

	GameplayTags.Attributes_Vital_MaxOmega = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.MaxOmega"),
		FString("Maximum Omega amount")
	);

	GameplayTags.Attributes_Max_MaxMoveSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Max.MaxMoveSpeed"),
		FString("Maximum movement speed")
	);

	GameplayTags.Attributes_Max_MaxAttackSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Max.MaxAttackSpeed"),
		FString("Maximum attack speed")
	);

	GameplayTags.Attributes_Max_MaxCoolDownReduction = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Max.MaxCoolDownReduction"),
		FString("Maximum cooldown reduction")
	);

	GameplayTags.Attributes_Max_MaxLevel = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Max.MaxLevel"),
		FString("Maximum character level")
	);

	GameplayTags.Attributes_Max_MaxXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Max.MaxXP"),
		FString("Maximum character XP")
	);

	// Meta Attributes
	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingXP"),
		FString("Incoming experience points")
	);

	GameplayTags.Attributes_Meta_IncomingDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingDamage"),
		FString("Incoming damage")
	);

	// Zenith
	GameplayTags.Attributes_Character_Zenith_ChainCount = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Zenith.ChainCount"),
		FString("Increases the number of chain targets for Zenith's skill.")
	);
	GameplayTags.Attributes_Character_Zenith_ChargingTimeMultiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Zenith.ChargingTimeMultiplier"),
		FString("Reduces the charging time for Zenith's skill.")
	);
	GameplayTags.Attributes_Character_Zenith_CanRotateWhileCharging = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Zenith.CanRotateWhileCharging"),
		FString("Allows Zenith to rotate while charging.")
	);

	// Vulcan
	GameplayTags.Attributes_Character_Vulcan_HasBuffRadius = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Vulcan.HasBuffRadius"),
		FString("Determines if Vulcan's buff has a radius.")
	);
	GameplayTags.Attributes_Character_Vulcan_BuffRadiusMultiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Vulcan.BuffRadiusMultiplier"),
		FString("Increases the radius of Vulcan's buff.")
	);
	GameplayTags.Attributes_Character_Vulcan_UltimateBezierCount = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Vulcan.UltimateBezierCount"),
		FString("Increases the number of Bezier curves for Vulcan's ultimate.")
	);

	// Rusty
	GameplayTags.Attributes_Character_Rusty_HookSizeMultiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Rusty.HookSizeMultiplier"),
		FString("Increases the size of Rusty's hook.")
	);
	GameplayTags.Attributes_Character_Rusty_HookSpeedMultiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Rusty.HookSpeedMultiplier"),
		FString("Increases the speed of Rusty's hook.")
	);
	GameplayTags.Attributes_Character_Rusty_UltimateGravityPull = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Rusty.UltimateGravityPull"),
		FString("Adds a gravity pull effect to Rusty's ultimate landing.")
	);
	GameplayTags.Attributes_Character_Rusty_ShieldMaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Rusty.ShieldMaxHealth"),
		FString("Amount of Shield Health.")
	);

	// Common
	GameplayTags.Attributes_Character_Common_AttackPenetrationCount = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Character.Common.AttackPenetrationCount"),
		FString("Increases the penetration count of basic attacks (excluding Rusty).")
	);
	GameplayTags.Attributes_Character_Common_MovementSkillRangeMultiplier = UGameplayTagsManager::Get().
		AddNativeGameplayTag(
			FName("Attributes.Character.Common.MovementSkillRangeMultiplier"),
			FString("Increases the range of movement skills.")
		);
	GameplayTags.Attributes_Character_Common_AttackProjectileSpeedMultiplier = UGameplayTagsManager::Get().
		AddNativeGameplayTag(
			FName("Attributes.Character.Common.AttackProjectileSpeedMultiplier"),
			FString("Increases the speed of basic attack projectiles.")
		);

	/*
	 * HitReact Tags
	 */
	GameplayTags.GameplayEvent_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.HitReact"),
		FString("Tag to Activate Hitreact")
	);

	GameplayTags.GameplayEvent_Death = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.Death"),
		FString("Tag to Activate Death")
	);
	
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effects.HitReact"),
		FString("Tag granted when hit reacting")
	);

	GameplayTags.Effects_CriticalHitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effects.CriticalHitReact"),
		FString("Tag granted when critical hit reacting")
	);

	/*
	 * Damage Tags
	 */
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),
		FString("Damage Parent")
	);

	GameplayTags.Damage_Zenith = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Zenith"),
		FString("Damage when Electric type Attacked")
	);

	GameplayTags.Damage_Vulcan = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Vulcan"),
		FString("Damage when Fire type Attacked")
	);

	GameplayTags.Damage_Rusty = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Rusty"),
		FString("Damage when Rusty type Attacked")
	);

	GameplayTags.Damage_Zinx = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Zinx"),
		FString("Damage when Zinx type Attacked")
	);

	GameplayTags.Damage_Muriel = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Muriel"),
		FString("Damage when Muriel type Attacked")
	);

	GameplayTags.Damage_Murdock = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Murdock"),
		FString("Damage when Murdock type Attacked")
	);

	GameplayTags.Damage_Shiv = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Shiv"),
		FString("Damage when Shiv type Attacked")
	);

	GameplayTags.Damage_Grappler = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Grappler"),
		FString("Damage when Grappler type Attacked")
	);

	GameplayTags.Damage_Boris = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Boris"),
		FString("Damage when Boris type Attacked")
	);

	GameplayTags.Damage_BlazedCannon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.BlazedCannon"),
		FString("Damage when BlazedCannon type Attacked")
	);

	GameplayTags.Damage_Glitch = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Glitch"),
		FString("Damage when Glitch type Attacked")
	);

	GameplayTags.Damage_Screamer = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Screamer"),
		FString("Damage when Screamer type Attacked")
	);
	/*
	 * Player Block Tags
	*/
	GameplayTags.Player_Block_Move = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.Move"),
		FString("Block move")
	);

	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputPressed"),
		FString("Block input when pressed")
	);

	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputHeld"),
		FString("Block input when held")
	);

	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputReleased"),
		FString("Block input when released")
	);

	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.CursorTrace"),
		FString("Block cursor trace functionality")
	);

	// Data
	GameplayTags.Data_SpeedReduction = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.SpeedReduction"),
		FString("Speed reduction percentage for SetByCaller")
	);

	GameplayTags.Data_SpeedReduction = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.CoolDown"),
		FString("Cooldown for SetByCaller")
	);

	GameplayTags.Data_Heal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.Heal"),
		FString("Heal for SetByCaller")
	);

	GameplayTags.Data_Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.Debuff.Duration"),
		FString("Debuff Duration")
	);

	GameplayTags.Data_Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.Debuff.Damage"),
		FString("Debuff Damage")
	);

	GameplayTags.Data_Debuff_Magnitude = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.Debuff.Magnitude"),
		FString("Debuff Magnitude")
	);
	
	GameplayTags.Data_Build_Magnitude = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.Build.Magnitude"),
		FString("Build Magnitude")
	);

	GameplayTags.Data_Build_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.Build.Duration"),
		FString("Build Duration")
	);
	
	
	// Weapon Data
	GameplayTags.Data_AttackDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.AttackDamage"),
		FString("Attack Damage for SetByCaller from Weapon")
	);
	GameplayTags.Data_AttackSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.AttackSpeed"),
		FString("Attack Speed for SetByCaller from Weapon")
	);
	GameplayTags.Data_AttackRange = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.AttackRange"),
		FString("Attack Range for SetByCaller from Weapon")
	);
	GameplayTags.Data_CriticalChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.CriticalChance"),
		FString("Critical Chance for SetByCaller from Weapon")
	);
	GameplayTags.Data_CriticalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.CriticalDamage"),
		FString("Critical Damage for SetByCaller from Weapon")
	);
	GameplayTags.Data_LifeSteal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.LifeSteal"),
		FString("Life Steal for SetByCaller from Weapon")
	);

	/*
	 * Critical/ Dodge Type Tags
	 */
	GameplayTags.Attributes_Current_CriticalChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.CriticalHitChance"),
		FString("Critical hit chance percentage")
	);

	GameplayTags.Attributes_Current_CriticalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.CriticalHitDamage"),
		FString("Critical hit damage multiplier")
	);

	GameplayTags.Attributes_Current_DodgeChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.DodgeChance"),
		FString("Dodge chance percentage")
	);

	GameplayTags.Attributes_Current_LifeSteal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Current.LifeSteal"),
		FString("LifeSteal percentage")
	);
	/*
	 * Resistance Type Tags
	 */
	// GameplayTags.Attributes_Resistance_Electric = UGameplayTagsManager::Get().AddNativeGameplayTag(
	// 	FName("Attributes.Resistance.Electric"),
	// 	FString("Electric damage resistance")
	// );
	//
	// GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
	// 	FName("Attributes.Resistance.Fire"),
	// 	FString("Fire damage resistance")
	// );
	//
	// GameplayTags.Attributes_Resistance_Rust = UGameplayTagsManager::Get().AddNativeGameplayTag(
	// 	FName("Attributes.Resistance.Rust"),
	// 	FString("Rust damage resistance")
	// );

	/*
	 * Ability Type Tags
	 */
	GameplayTags.Abilities_Type_Active = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.Active"),
		FString("Active ability type")
	);

	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.Passive"),
		FString("Passive ability type")
	);

	GameplayTags.Abilities_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.None"),
		FString("No ability type")
	);

	/*
	 * Ability Enemy Type Tags
	 */
	GameplayTags.Abilities_Type_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.Attack"),
		FString("Attack ability type")
	);

	/*
	 * Debuff Type Tags
	 */
	GameplayTags.Debuff_Status_Shock = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Status.Shock"),
		FString("Debuff Shocked")
	);

	GameplayTags.Debuff_Status_Slow = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Status.Slow"),
		FString("Debuff Slowed")
	);

	GameplayTags.Debuff_Status_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Status.Stun"),
		FString("Debuff stunned")
	);

	GameplayTags.Debuff_Dot_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Dot.Burn"),
		FString("Dot burned")
	);

	GameplayTags.Debuff_Dot_Corrosion = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Dot.Corrosion"),
		FString("Corrosion")
	);

	GameplayTags.Debuff_Type_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Type.Burn"),
		FString("Burn")
	);

	GameplayTags.Debuff_Type_Slow = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Type.Slow"),
		FString("Slow")
	);

	GameplayTags.Debuff_Type_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Type.Stun"),
		FString("Stun")
	);
	
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Chance"),
		FString("Debuff application chance"));

	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Damage"),
		FString("Debuff damage per tick"));

	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Duration"),
		FString("Debuff duration"));

	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Frequency"),
		FString("Debuff tick frequency"));

	GameplayTags.Debuff_Magnitude = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Magnitude"),
		FString("Debuff Magnitude"));
	
	GameplayTags.Skill_Multiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skill.Multiplier"),
		FString("Skill damage multiplier"));
	/*
	* Gameplay Cue Tags
	 */
	GameplayTags.GameplayCue_Movement_Teleport = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Movement.Teleport"),
		FString("Teleport effect")
	);

	GameplayTags.GameplayCue_Movement_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Movement.Dash"),
		FString("Dash effect")
	);

	GameplayTags.GameplayCue_MuzzleEffect_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.MuzzleEffect.BasicAttack"),
		FString("Basic attack muzzle effect")
	);

	GameplayTags.GameplayCue_MuzzleEffect_VulcanAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.MuzzleEffect.VulcanAttack"),
		FString("Vulcan attack muzzle effect")
	);

	GameplayTags.GameplayCue_MuzzleEffect_ZenithAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.MuzzleEffect.ZenithAttack"),
		FString("Zenith attack muzzle effect")
	);

	GameplayTags.GameplayCue_MuzzleEffect_RustyAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.MuzzleEffect.RustyAttack"),
		FString("Rusty attack muzzle effect")
	);

	GameplayTags.GameplayCue_MuzzleEffect_EnemyAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.MuzzleEffect.EnemyAttack"),
		FString("Enemy attack muzzle effect")
	);

	GameplayTags.GameplayCue_MuzzleEffect_Grenade = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.MuzzleEffect.Grenade"),
		FString("Grenade muzzle effect")
	);

	GameplayTags.GameplayCue_MuzzleEffect_ZenithGrenade = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.MuzzleEffect.ZenithGrenade"),
		FString("ZenithGrenade muzzle effect")
	);

	/*
	 * Weapon
	 */
	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_Pistol = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.Pistol"),
		FString("Pistol muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_Handgun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.Handgun"),
		FString("Handgun muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_Rifle1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.Rifle1"),
		FString("Rifle1 muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_Rifle2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.Rifle2"),
		FString("Rifle2 muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_Shotgun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.Shotgun"),
		FString("Shotgun muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_SMG = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.SMG"),
		FString("SMG muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_Sniper = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.Sniper"),
		FString("Sniper muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_Twingun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.Twingun"),
		FString("Twingun muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_LaserRifle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.LaserRifle"),
		FString("LaserRifle muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_RocketLauncher = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.RocketLauncher"),
		FString("RocketLauncher muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_GrenadeLauncher = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.GrenadeLauncher"),
		FString("GrenadeLauncher muzzle effect")
	);

	GameplayTags.GameplayCue_WeaponEffect_MuzzleEffect_Flamethrower = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.WeaponEffect.MuzzleEffect.Flamethrower"),
		FString("Flamethrower muzzle effect")
	);
	
	GameplayTags.GameplayCue_Zenith_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Zenith.Skill.Q"),
		FString("Zenith Skill Q Effect")
	);

	GameplayTags.GameplayCue_Zenith_Skill_LeftShift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Zenith.Skill.LeftShift"),
		FString("Zenith Skill LeftShift Effect")
	);

	GameplayTags.GameplayCue_Zenith_Skill_R = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Zenith.Skill.R"),
		FString("Zenith Skill R Effect")
	);

	GameplayTags.GameplayCue_Zenith_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Zenith.Skill.RMB"),
		FString("Zenith Skill RMB Effect")
	);

	GameplayTags.GameplayCue_Zenith_Skill_Chain = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Zenith.Skill.Chain"),
		FString("Zenith Skill Chain Effect")
	);
	GameplayTags.GameplayCue_Zenith_Skill_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Zenith.Skill.Lightning"),
		FString("Zenith Skill Lightning Effect")
	);

	GameplayTags.GameplayCue_Zenith_Skill_R_Muzzle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Zenith.Skill.R.Muzzle"),
		FString("Zenith Skill R_Muzzle Effect")
	);

	GameplayTags.GameplayCue_Vulcan_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Vulcan.Skill.Q"),
		FString("Vulcan Skill Q Effect")
	);

	GameplayTags.GameplayCue_Vulcan_Skill_LeftShift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Vulcan.Skill.LeftShift"),
		FString("Vulcan Skill LeftShift Effect")
	);

	GameplayTags.GameplayCue_Vulcan_Skill_R = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Vulcan.Skill.R"),
		FString("Vulcan Skill R Effect")
	);

	GameplayTags.GameplayCue_Vulcan_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Vulcan.Skill.RMB"),
		FString("Vulcan Skill RMB Effect")
	);

	GameplayTags.GameplayCue_Vulcan_Skill_BuffStart = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Vulcan.Skill.BuffStart"),
		FString("Vulcan Skill BuffStart Effect")
	);

	GameplayTags.GameplayCue_Vulcan_Skill_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Vulcan.Skill.Dash"),
		FString("Vulcan Skill Dash Effect")
	);

	GameplayTags.GameplayCue_Rusty_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Rusty.BasicAttack"),
		FString("Rusty BasicAttack Effect")
	);

	GameplayTags.GameplayCue_Rusty_OmicronBasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Rusty.OmicronBasicAttack"),
		FString("Rusty OmicronBasicAttack Effect")
	);

	GameplayTags.GameplayCue_Rusty_Skill_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Rusty.Skill.Q"),
		FString("Rusty Skill Q Effect")
	);

	GameplayTags.GameplayCue_Rusty_Skill_LeftShift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Rusty.Skill.LeftShift"),
		FString("Rusty Skill LeftShift Effect")
	);

	GameplayTags.GameplayCue_Rusty_Skill_R = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Rusty.Skill.R"),
		FString("Rusty Skill R Effect")
	);

	GameplayTags.GameplayCue_Rusty_Skill_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Rusty.Skill.RMB"),
		FString("Rusty Skill RMB Effect")
	);

	GameplayTags.GameplayCue_Enemy_Infantry_Skill_BuffStart = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Infantry.Skill.BuffStart"),
		FString("Enemy Infantry Skill BuffStart Effect")
	);

	GameplayTags.GameplayCue_Enemy_Infantry_Skill_Buff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Infantry.Skill.Buff"),
		FString("Enemy Infantry Skill Buff Effect")
	);

	GameplayTags.GameplayCue_Enemy_Skill_Chain = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Skill.Chain"),
		FString("Enemy Skill Chain Effect")
	);

	GameplayTags.GameplayCue_Enemy_Skill_Discharge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Skill.Discharge"),
		FString("Enemy Skill Discharge Effect")
	);

	GameplayTags.GameplayCue_Enemy_Skill_Charge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Skill.Charge"),
		FString("Enemy Skill Charge Effect")
	);

	GameplayTags.GameplayCue_Enemy_Skill_Teleport = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Skill.Teleport"),
		FString("Enemy Skill Teleport Effect")
	);

	GameplayTags.GameplayCue_Enemy_Rusty_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Rusty.BasicAttack"),
		FString("Enemy Rusty BasicAttack Effect")
	);

	GameplayTags.GameplayCue_Enemy_BlazeCannon_KnockBack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.BlazeCannon.KnockBack"),
		FString("Enemy BlazedCannon KnockBack Effect")
	);

	GameplayTags.GameplayCue_Enemy_BlazeCannon_Back = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.BlazeCannon.Back"),
		FString("Enemy BlazedCannon Back Effect")
	);

	GameplayTags.GameplayCue_Enemy_BlazeCannon_BigBomb = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.BlazeCannon.BigBomb"),
		FString("Enemy BlazedCannon BigBomb Effect")
	);

	GameplayTags.GameplayCue_Enemy_Glitch_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Glitch.BasicAttack"),
		FString("Enemy Glitch BasicAttack Effect")
	);

	GameplayTags.GameplayCue_Enemy_Glitch_BuffStart = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Glitch.BuffStart"),
		FString("Enemy Glitch BuffStart Effect")
	);

	GameplayTags.GameplayCue_Enemy_Glitch_Buff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Glitch.Buff"),
		FString("Enemy Glitch Buff Effect")
	);

	GameplayTags.GameplayCue_Enemy_Glitch_Charge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Glitch.Charge"),
		FString("Enemy Glitch Charge Effect")
	);

	GameplayTags.GameplayCue_Enemy_Screamer_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Screamer.BasicAttack"),
		FString("Enemy Screamer BasicAttack")
	);

	GameplayTags.GameplayCue_Enemy_Screamer_Roar = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Screamer.Roar"),
		FString("Enemy Screamer Roar")
	);

	GameplayTags.GameplayCue_Enemy_Screamer_Jump = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Screamer.Jump"),
		FString("Enemy Screamer Roar")
	);

	GameplayTags.GameplayCue_Enemy_Screamer_Ambush = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Screamer.Ambush"),
		FString("Enemy Screamer Roar")
	);

	GameplayTags.GameplayCue_Enemy_Screamer_Grab = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Screamer.Grab"),
		FString("Enemy Screamer Grab")
	);

	GameplayTags.GameplayCue_Enemy_Screamer_Tackle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Enemy.Screamer.Tackle"),
		FString("Enemy Screamer Tackle")
	);

	// Items
	GameplayTags.GameplayCue_Item_GetItemBase = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.GetItemBase"),
		FString("Item Get Item Base Effect")
	);

	GameplayTags.GameplayCue_Item_Healing = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.Healing"),
		FString("Item Healing Effect")
	);

	GameplayTags.GameplayCue_Item_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.Health"),
		FString("Item Health Effect")
	);

	GameplayTags.GameplayCue_Item_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.MaxHealth"),
		FString("Item MaxHealth Effect")
	);

	GameplayTags.GameplayCue_Item_AttackDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.AttackDamage"),
		FString("Item AttackDamage Effect")
	);

	GameplayTags.GameplayCue_Item_AttackSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.AttackSpeed"),
		FString("Item AttackSpeed Effect")
	);

	GameplayTags.GameplayCue_Item_MoveSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.MoveSpeed"),
		FString("Item MoveSpeed Effect")
	);

	GameplayTags.GameplayCue_Item_Shield = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.Shield"),
		FString("Item Shield Effect")
	);

	GameplayTags.GameplayCue_Item_Critical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.Critical"),
		FString("Item Critical Effect")
	);

	GameplayTags.GameplayCue_Item_Invulnerability = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.Invulnerability"),
		FString("Item Invulnerability Effect")
	);

	GameplayTags.GameplayCue_Item_LifeSteal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.LifeSteal"),
		FString("Item LifeSteal Effect")
	);


	GameplayTags.GameplayCue_Item_Zenith = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.Zenith"),
		FString("Item Zenith Effect")
	);

	GameplayTags.GameplayCue_Item_Vulcan = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.Vulcan"),
		FString("Item Vulcan Effect")
	);

	GameplayTags.GameplayCue_Item_Rusty = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.Rusty"),
		FString("Item Rusty Effect")
	);
	GameplayTags.GameplayCue_Item_Common = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Item.Common"),
		FString("Item Common Effect")
	);
	/*
	 * Area Cue Tag
	 */

	GameplayTags.GameplayCue_Area_HealthUp = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Area.HealthUp"),
		FString("Area HealthUp"));

	GameplayTags.GameplayCue_Area_AttackDamageUp = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Area_AttackDamageUp"),
		FString("Area.AttackDamageUp"));

	GameplayTags.GameplayCue_Area_LifeStealUp = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Area.LifeStealUp"),
		FString("Area_LifeStealUp"));

	GameplayTags.GameplayCue_Area_AttackRangeUp = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Area.AtackRangeUp"),
		FString("Area_AttackRangeUp"));

	GameplayTags.GameplayCue_Area_MoveSpeedUp = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Area.MoveSpeedUp"),
		FString("Area_AttackRangeUp"));

	GameplayTags.GameplayCue_Area_MoveSpeedDown = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Area.MoveSpeedDown"),
		FString("Area_MoveSpeedDown"));

	/*
	 * Hit React Cue Tag
	 */
	GameplayTags.GameplayCue_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.HitReact"),
		FString("Hit React Effect")
	);

	GameplayTags.GameplayCue_Death = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Death"),
		FString("Death Effect")
	);

	GameplayTags.GameplayCue_Death_Dissolve = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Death_Dissolve"),
		FString("Death Dissolve Effect")
	);

	GameplayTags.GameplayCue_Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Debuff.Burn"),
		FString("Burn Effect")
	);

	GameplayTags.GameplayCue_Debuff_Slow = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Debuff.Slow"),
		FString("Slow Effect")
	);

	GameplayTags.GameplayCue_Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Debuff.Stun"),
		FString("Stun Effect")
	);

	GameplayTags.GameplayCue_Debuff_Bleed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Debuff.Bleed"),
		FString("Bleed Effect")
	);

	GameplayTags.GameplayCue_Debuff_Marked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Debuff.Marked"),
		FString("Marked Effect")
	);
	
	/*
	 * Camera Shake Cue Tag
	 */
	GameplayTags.GameplayCue_Shake = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Shake"),
		FString("Camera Shake Effect")
	);

	// /*
	//  * Maps for quick lookups 
	//  */
	GameplayTags.GameplayCue_Rusty_Skill_DownDash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Rusty.Skill.DownDash"),
		FString("Rusty Skill DownDash Effect")
	);

	GameplayTags.GameplayCue_Rusty_Skill_Crush = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Rusty.Skill.Crush"),
		FString("Rusty Skill Crush Effect")
	);

	// // Zenith	TArray<FGameplayTag> ZenithSkills;
	// ZenithSkills.Add(GameplayTags.Ability_Omega_Zenith_Skill_Q);
	// ZenithSkills.Add(GameplayTags.Ability_Omega_Zenith_Skill_LeftShift);
	// ZenithSkills.Add(GameplayTags.Ability_Omega_Zenith_Skill_R);
	// ZenithSkills.Add(GameplayTags.Ability_Omega_Zenith_Skill_RMB);
	// GameplayTags.CharacterTypeToSkills.Add(GameplayTags.Character_Type_Zenith, ZenithSkills);
	//
	// // Vulcan 	TArray<FGameplayTag> VulcanSkills;
	// VulcanSkills.Add(GameplayTags.Ability_Omega_Vulcan_Attack);
	// VulcanSkills.Add(GameplayTags.Ability_Omega_Vulcan_Skill_Q);
	// VulcanSkills.Add(GameplayTags.Ability_Omega_Vulcan_Skill_LeftShift);
	// VulcanSkills.Add(GameplayTags.Ability_Omega_Vulcan_Skill_R);
	// VulcanSkills.Add(GameplayTags.Ability_Omega_Vulcan_Skill_RMB);
	// GameplayTags.CharacterTypeToSkills.Add(GameplayTags.Character_Type_Vulcan, VulcanSkills);
	//
	// // Rusty	TArray<FGameplayTag> RustySkills;
	// RustySkills.Add(GameplayTags.Ability_Omega_Rusty_Attack);
	// RustySkills.Add(GameplayTags.Ability_Omega_Rusty_Skill_Q);
	// RustySkills.Add(GameplayTags.Ability_Omega_Rusty_Skill_E);
	// RustySkills.Add(GameplayTags.Ability_Omega_Rusty_Skill_R);
	// RustySkills.Add(GameplayTags.Ability_Omega_Rusty_Skill_RMB);
	// GameplayTags.CharacterTypeToSkills.Add(GameplayTags.Character_Type_Rusty, RustySkills);

	/*
	 * Shield 
	 */

	GameplayTags.Shield_Status_Using = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Shield.Status.Using"),
		FString("The shield is in use.")
	);

	GameplayTags.Shield_Status_Timer = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Shield.Status.Timer"),
		FString("The shield is prepared to repair.")
	);

	GameplayTags.Shield_Status_Repaired = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Shield.Status.Repaired"),
		FString("The shield is under repair.")
	);

	GameplayTags.Shield_Status_Broken = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Shield.Status.Broken"),
		FString("The shield is broken.")
	);

	/*
	 * Sequence
	 */

	GameplayTags.Event_AbilitySequence_Next = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.AbilitySequence.Next"),
		FString("Change to Next Ability in Sequence")
	);

	/*
	 * Status
	 */

	GameplayTags.Status_Charging = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Charging"),
		FString("Be charging now.")
	);

	/*
	 * HitType
	 */
	GameplayTags.HitType_Dot = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("HitType.Dot"),
		FString("Dot Skill Hit"));
	GameplayTags.HitType_Heavy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("HitType.Heavy"),
		FString("Heavy Skill Hit"));
	GameplayTags.HitType_Normal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("HitType.Normal"),
		FString("Normal Skill Hit"));

	/*
	 * KnockBack
	 */
	GameplayTags.Knockback_Force = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Knockback.Force"),
		FString("Knockback force"));

	GameplayTags.Knockback_Critical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Knockback.Critical"),
		FString("Knockback critical"));

	GameplayTags.Knockback_Heavy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Knockback.Heavy"),
		FString("Knockback heavy"));

	GameplayTags.Knockback_Light = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Knockback.Light"),
		FString("Knockback light"));

	/*
 * Sound Level Tags
 */
	GameplayTags.Sound_Level_Lobby = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Sound.Level.Lobby"),
		FString("Lobby level sound")
	);

	GameplayTags.Sound_Level_MainMenu = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Sound.Level.MainMenu"),
		FString("Main menu level sound")
	);

	GameplayTags.Sound_Level_Stage_Normal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Sound.Level.Stage.Normal"),
		FString("Normal stage level sound")
	);

	GameplayTags.Sound_Level_Stage_Boss = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Sound.Level.Stage.Boss"),
		FString("Boss stage level sound")
	);

	/*
	 * Sound Situation Tags
	 */
	GameplayTags.Sound_Situation_Combat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Sound.Situation.Combat"),
		FString("Combat situation sound")
	);

	GameplayTags.Sound_Situation_Boss = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Sound.Situation.Boss"),
		FString("Boss situation sound")
	);

	/*
	* Skill Property Tags
	*/
	GameplayTags.Property_Range = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Property.Range"),
		FString("Range Property")
	);

	GameplayTags.Property_Hook_MaxGrabCount = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Property.Hook.MaxGrabCount"),
		FString("Hook MaxGrabCount Property")
	);

	GameplayTags.Property_Hook_CanHitWall = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Property.Hook.CanHitWall"),
		FString("Hook CanHitWall Property")
	);
	
	GameplayTags.Property_MaxHealthPercentage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Property.MaxHealthPercentage"),
		FString("MaxHealthPercentage Property")
	);

	GameplayTags.Property_ComboCount = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Property.ComboCount"),
		FString("ComboCount Property")
	);
	
	GameplayTags.Property_SkillDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Property.SkillDamage"),
		FString("SkillDamage Property")
	);
	
	/*
	* Skill Build Tags
	*/
	// Rusty(Omega)
	// Q
	GameplayTags.Build_Omega_Rusty_Skill_Q_DamageIncrease = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Q.DamageIncrease"),
		FString("Rusty Omega Skill Q Damage Increase")
	);

	GameplayTags.Build_Omega_Rusty_Skill_Q_RangeIncrease = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Q.RangeIncrease"),
		FString("Rusty Omega Skill Q Range Increase")
	);

	GameplayTags.Build_Omega_Rusty_Skill_Q_AttackDamageBuff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Q.AttackDamageBuff"),
		FString("Rusty Omega Skill Q AttackDamageBuff")
	);

	GameplayTags.Build_Omega_Rusty_Skill_Q_Slow = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Q.Slow"),
		FString("Rusty Omega Skill Q Slow")
	);

	GameplayTags.Build_Omega_Rusty_Skill_Q_MultiGrab = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Q.MultiGrab"),
		FString("Rusty Omega Skill Q Multi Grab")
	);
	
	// Shift
	GameplayTags.Build_Omega_Rusty_Skill_Shift_ComboIncrease = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Shift.ComboIncrease"),
		FString("Rusty Omega Skill Shift Combo Increase")
	);

	GameplayTags.Build_Omega_Rusty_Skill_Shift_FinalBlowBuff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Shift.FinalBlowBuff"),
		FString("Rusty Omega Skill Shift Final Blow Buff")
	);
	
	GameplayTags.Build_Omega_Rusty_Skill_Shift_Bleed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Shift.Bleed"),
		FString("Rusty Omega Skill Shift Bleed")
	);
	
	GameplayTags.Build_Omega_Rusty_Skill_Shift_RangeAndDamageUp = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Shift.RangeAndDamageUp"),
		FString("Rusty Omega Skill Shift Range And Damage Up")
	);

	GameplayTags.Build_Omega_Rusty_Skill_Shift_LifeSteal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.Shift.LifeSteal"),
		FString("Rusty Omega Skill Shift LifeSteal")
	);

	// R
	GameplayTags.Build_Omega_Rusty_R_StunAndSlow = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.R.StunAndSlow"),
		FString("Rusty Omega Skill R StunAndSlow")
	);

	GameplayTags.Build_Omega_Rusty_R_RangeIncrease = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.R.RangeIncrease"),
		FString("Rusty Omega Skill R Range Increase")
	);

	GameplayTags.Build_Omega_Rusty_R_AttackDamageAndAttackSpeedBuff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.R.AttackDamageAndAttackSpeedBuff"),
		FString("Rusty Omega Skill R AttackDamage And AttackSpeed Buff")
	);
	GameplayTags.Build_Omega_Rusty_R_DamageUp = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Omega.Rusty.Skill.R.DamageUp"),
		FString("Rusty Omega Skill R Damage Up")
	);

	// Rusty(Normal)
	// Q
	GameplayTags.Build_Normal_Rusty_Skill_Q_RangeIncrease = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Normal.Rusty.Skill.Q.RangeIncrease"),
		FString("Rusty Normal Skill Q Range Increase")
	);

	GameplayTags.Build_Normal_Rusty_Skill_Q_CanHitWallAndItem = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Normal.Rusty.Skill.Q.CanHitWallAndItem"),
		FString("Rusty Normal Skill Q CanHitWallAndItem")
	);
	
	GameplayTags.Build_Normal_Rusty_Skill_Q_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Normal.Rusty.Skill.Q.Stun"),
		FString("Rusty Normal Skill Q Stun")
	);

	GameplayTags.Build_Normal_Rusty_Skill_Q_MaxHealthPercentDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Normal.Rusty.Skill.Q.MaxHealthPercentDamage"),
		FString("Rusty Normal Skill Q MaxHealth Percent Damage")
	);
	
	GameplayTags.Build_Normal_Rusty_Skill_Q_AttackDamageAndMoveSpeedBuff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Normal.Rusty.Skill.Q.AttackDamageAndMoveSpeedBuff"),
		FString("Rusty Normal Skill Q AttackDamage And MoveSpeedBuff")
	);
	
	// Vulcan(Omega)
	// Vulcan(Normal)
	// Zenith(Omega)
	// Zenith(Normal)

	GameplayTags.Build_Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Debuff.Stun"),
		FString("Build Debuff Stun"));

	GameplayTags.Build_Debuff_Slow = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Debuff.Slow"),
		FString("Build Debuff Slow"));
	
	GameplayTags.Build_Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Build.Debuff.Burn"),
		FString("Build Debuff Burn"));
	


	GameplayTags.Property_Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Property.Debuff.Damage"),
		FString("Debuff Damage")
	);

	GameplayTags.Property_Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Property.Debuff.Duration"),
		FString("Debuff Duration")
	);

	GameplayTags.Property_Debuff_Magnitude = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Property.Debuff.Magnitude"),
		FString("Debuff Magnitude")
	);

	/*
	 * Debuff Build Tag Map
	 */
	GameplayTags.DebuffTypeToBuild.Add(
		GameplayTags.Debuff_Type_Stun,
		GameplayTags.Build_Debuff_Stun
	);
	GameplayTags.DebuffTypeToBuild.Add(
		GameplayTags.Debuff_Type_Slow,
		GameplayTags.Build_Debuff_Slow
	);
	GameplayTags.DebuffTypeToBuild.Add(
		GameplayTags.Debuff_Type_Burn,
		GameplayTags.Build_Debuff_Burn
	);
}
