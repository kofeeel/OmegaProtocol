Omega Protocol: UE5.5 GAS 기반 3인 협동 로그라이트 탑다운 슈터
<a href="https://store.steampowered.com/app/3891070/Omega_Protocol_Demo/" target="_blank" rel="noopener noreferrer">
<img width="1204" height="676" alt="Omega Protocol Demo Banner" src="https://github.com/user-attachments/assets/6de90700-834b-474f-a176-a341eb9a8068" />
</a>

Omega Protocol은 언리얼 엔진 5.5와 게임플레이 어빌리티 시스템(GAS)을 기반으로 제작된 3인 협동 로그라이트 탑다운 슈터 게임입니다. 2개월의 개발 기간 동안 단 하나의 파워업('오메가 부스터')을 팀원 간에 실시간으로 주고받는 핵심 메카닉을 중심으로, Steamworks 연동을 통한 멀티플레이 환경과 데이터 기반의 확장 가능한 캐릭터 및 어빌리티 시스템을 구현했습니다.

1. 프로젝트 개요
공식 명칭: Omega Protocol

장르: 3인 협동 로그라이트 액션

개발 기간: 2개월

엔진: Unreal Engine 5.5

핵심 기술: C++, Gameplay Ability System (GAS), Steam Online Subsystem

핵심 특징:

하나의 파워업 공유: 단 하나의 '오메가 부스터'를 3명의 팀원 간에 실시간으로 전달하며 전략적으로 전투를 풀어나가는 시스템.

3인 협동 멀티플레이: Steamworks 연동을 통해 친구들과 함께 플레이 가능.

로그라이트 요소: 매번 새롭게 생성되는 스테이지와 예측 불가능한 적들.

3가지 고유 캐릭터: 정밀함의 'ZENITH', 화력의 'VULCAN', 방어의 'RUSTY' 등 각기 다른 철학을 가진 캐릭터.

데이터 기반 설계: DataAsset을 활용하여 캐릭터, 스킬, 아이템 등을 유연하게 확장할 수 있는 구조.

2. 사용 기술
Unreal Engine 5.5: C++ 프로그래밍, 블루프린트

Gameplay Ability System (GAS): 캐릭터 어트리뷰트, 어빌리티, 게임플레이 이펙트 등 핵심 로직 관리

Online Subsystem: Steamworks API를 활용한 세션 생성, 검색 및 참여 기능 구현

UMG (Unreal Motion Graphics): WidgetController 패턴을 활용한 UI 시스템 설계

AI: Behavior Tree와 Blackboard를 이용한 적 AI 구현

VCS: Git

3. 핵심 시스템 및 구조
3.1. 게임플레이 어빌리티 시스템 (GAS)
GAS를 프로젝트의 핵심 프레임워크로 사용하여 전투와 관련된 모든 로직을 모듈화하고 확장성을 확보했습니다.

OmochaAbilitySystemComponent: 기본 ASC를 상속받아 입력 처리, 어빌리티 관리 등 프로젝트에 특화된 기능을 추가했습니다.

OmochaAttributeSet: 캐릭터의 모든 스탯(체력, 오메가 게이지, 공격력, 방어력, 이동 속도 등)을 어트리뷰트로 정의하고, ExecCalc_Damage 클래스를 통해 복잡한 데미지 계산 로직을 구현했습니다.

다양한 GameplayAbility 구현:

ProjectileGameplayAbility: 투사체 발사 어빌리티의 기반 클래스.

TraceGameplayAbility: 범위 공격(원, 부채꼴, 사각형)을 위한 기반 클래스.

MoveGameplayAbility: 대쉬, 텔레포트 등 이동 관련 어빌리티의 기반 클래스.

SequenceSkillGameplayAbility: 여러 스킬을 순차적으로 발동시키는 시퀀스 어빌리티 구현.

AbilityTask 활용: OmochaMouseHitTask, WaitForMouseClickTask 등 커스텀 AbilityTask를 구현하여 비동기적인 타겟팅 및 입력 처리 로직을 효율적으로 관리했습니다.

3.2. 멀티플레이 및 네트워크
Steam Online Subsystem을 기반으로 안정적인 멀티플레이 환경을 구축했습니다.

OmochaSessionSubsystem: Steamworks API와 연동하여 세션 생성, 검색, 참여, 소멸 등 멀티플레이 로직을 캡슐화했습니다.

GameMode 및 GameInstance 활용:

OmochaGameInstance: 플레이어 정보, 캐릭터 클래스, 저장된 스탯 등 여러 레벨에 걸쳐 유지되어야 하는 데이터를 관리합니다.

OmochaLobbyGameMode, OmochaPlayGameMode, OmochaScoreBoardGameMode: 게임의 흐름(로비 -> 인게임 -> 결과창)에 따라 GameMode를 분리하여 각 상태에 맞는 로직을 처리하도록 설계했습니다.

데이터 복제: PlayerState, AbilitySystemComponent의 어트리뷰트 및 게임플레이 태그를 안정적으로 복제하여 모든 클라이언트가 일관된 게임 상태를 유지하도록 했습니다.

3.3. 캐릭터 시스템 (Omega / Normal 상태 전환)
데이터 기반의 유연한 캐릭터 시스템을 설계하여 Steam 페이지의 핵심 기능인 '오메가 부스터' 전달 기능을 구현했습니다.

OmochaCharacterBase: 플레이어와 AI 캐릭터가 공유하는 기본 로직(죽음, 피격 반응 등)을 정의하는 기반 클래스입니다.

OmochaCharacterConfigComponent: 캐릭터의 상태(Normal/Omega)에 따라 어빌리티, 외형, 스탯을 동적으로 교체하는 핵심 컴포넌트입니다.

상태 전환: 이 컴포넌트를 통해 '오메가 부스터'를 받은 캐릭터는 오메가 상태가 되어 더 강력한 스킬셋과 능력치를 가집니다.

데이터 기반 설정: OmochaCharacterDataAsset을 사용하여 각 캐릭터의 상태별 메쉬, 애니메이션, 어빌리티 목록, 스탯 효과 등을 정의합니다.

OmochaWeaponComponent: 무기 장착, 교체, 및 관련 어빌리티 부여를 담당하는 컴포넌트입니다.

3.4. AI 시스템
Behavior Tree를 사용하여 적 AI의 행동 로직을 설계했습니다.

OmochaAIController / BehaviorTree: 적의 상태(Minion, Boss 등)와 클래스(Infantry, Tanker 등)에 따라 다른 행동 패턴을 가지도록 BehaviorTree를 구성했습니다.

OmochaEnemyDataAsset: 적의 등급, 클래스별로 사용할 BehaviorTree와 어빌리티 목록을 데이터 에셋으로 관리하여 AI의 다양성을 확보했습니다.

3.5. UI 시스템
WidgetController 패턴을 도입하여 UMG 위젯과 게임 로직(특히 GAS) 간의 의존성을 낮추고 데이터 바인딩을 체계화했습니다.

OmochaWidgetController: UI에 필요한 데이터를 PlayerState, AbilitySystemComponent 등에서 가져와 가공하고, UI의 변경 사항을 다시 시스템에 전달하는 중간 계층 역할을 합니다.

다양한 WidgetController: OverlayWidgetController (HUD), StatWidgetController (스탯 창) 등 각 UI에 맞는 컨트롤러를 구현했습니다.

4. 제가 담당한 역할 및 구현 내용
이 프로젝트에서 저는 데이터 중심 설계 및 전투시스템 구현을 담당했습니다.

GAS 기반 전투 시스템 설계 및 구현

AbilitySystemComponent와 AttributeSet을 커스터마이즈하여 프로젝트의 기본 전투 구조를 설계했습니다.

투사체, 범위 공격, 이동기 등 재사용 가능한 어빌리티 기반 클래스를 구현하여 생산성을 높였습니다.

ExecCalc_Damage를 통해 데미지, 치명타, 넉백, 디버프 등 복잡한 전투 공식을 구현하고 데이터 테이블과 연동하여 밸런싱이 용이하도록 설계했습니다.

데이터 기반 캐릭터 시스템 설계

OmochaCharacterConfigComponent를 중심으로 캐릭터의 상태(일반/오메가)에 따라 외형, 스탯, 스킬셋이 모두 변경되는 다이나믹한 시스템을 구현했습니다.

DataAsset을 적극적으로 활용하여 캐릭터, 적, 무기, 스킬 등의 데이터를 분리하고, 기획자가 쉽게 값을 수정하고 콘텐츠를 확장할 수 있는 구조를 만들었습니다.

UI-로직 연동을 위한 WidgetController 패턴 구현

UMG와 GAS를 효율적으로 연동하기 위해 WidgetController 패턴을 도입하고, 어트리뷰트 변화에 따라 UI가 자동으로 업데이트되는 시스템을 구축했습니다.

적 AI 및 게임플레이 메카닉 구현

Behavior Tree를 사용하여 다양한 공격 패턴을 가진 적 AI를 구현했습니다.

