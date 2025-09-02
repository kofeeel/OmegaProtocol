<br>
<div align=center>
    <a href="https://store.steampowered.com/app/3891070/Omega_Protocol_Demo/" target="_blank" rel="noopener noreferrer">
<img width="1204" height="676" alt="Omega Protocol Demo Banner" src="https://github.com/user-attachments/assets/6de90700-834b-474f-a176-a341eb9a8068" />
</a>
</div>
<br>

Omega Protocol
UE5.5 GAS 기반 3인 협동 로그라이트 탑다운 슈터
🎮 플레이
<a href="https://store.steampowered.com/app/3891070/Omega_Protocol_Demo/" target="_blank" rel="noopener noreferrer">
  <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/83/Steam_icon_logo.svg/512px-Steam_icon_logo.svg.png" alt="Steam" width="100"/>
</a>

📖 개요
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

🛠️ 사용 기술
Engine: Unreal Engine 5.5 (C++, 블루프린트)

Core Logic: Gameplay Ability System (GAS)

Network: Steam Online Subsystem

UI: UMG (WidgetController Pattern)

AI: Behavior Tree

VCS: Perforce

Collaboration: Notion, Excel, Figma, Discord

🧑‍💻 제작자
<img src="[팀원1 GitHub 프로필 이미지 URL]" width=240>	<img src="[팀원2 GitHub 프로필 이미지 URL]" width=240>	<img src="[팀원3 GitHub 프로필 이미지 URL]" width=240>	<img src="[팀원4 GitHub 프로필 이미지 URL]" width=240>
[팀원1 이름]	[팀원2 이름]	[팀원3 이름]	[팀원4 이름]
[팀원1 역할]	[팀원2 역할]	[팀원3 역할]	[팀원4 역할]

Sheets로 내보내기
🙋 담당 업무 및 핵심 구현 내용
이 프로젝트에서 저는 데이터 중심의 전투 시스템과 캐릭터 시스템 아키텍처 설계 및 구현을 총괄했습니다.

1. GAS 기반 전투 시스템 설계 및 구현
복잡한 데미지 처리 로직 구현: ExecCalc_Damage 클래스를 통해 기본 데미지, 스킬 계수, 치명타, 랜덤 편차 등을 포함한 복잡한 데미지 계산식을 구현했습니다.

상태 이상 및 넉백 시스템: FDamageEffectParams와 커스텀 FGameplayEffectContext를 활용하여 디버프(스턴, 슬로우 등)와 넉백 정보를 어빌리티에서 계산 클래스까지 안정적으로 전달하는 파이프라인을 구축했습니다.

재사용 가능한 어빌리티 아키텍처: 투사체(ProjectileGameplayAbility), 범위 공격(TraceGameplayAbility), 이동기(MoveGameplayAbility) 등 목적에 맞는 어빌리티 기반 클래스를 설계하여, 다양한 스킬을 빠르고 일관되게 개발할 수 있는 환경을 만들었습니다.

2. 데이터 기반 캐릭터 및 콘텐츠 시스템
동적 상태 전환 시스템: 캐릭터의 핵심 메카닉인 '일반/오메가' 상태를 관리하는 OmochaCharacterConfigComponent를 설계했습니다. 이 컴포넌트는 상태에 따라 캐릭터의 외형(메쉬, 머티리얼), 스탯, 어빌리티 목록을 동적으로 교체하여, 단일 애셋으로 다양한 캐릭터 상태를 표현할 수 있게 했습니다.

데이터 중심 설계: DataAsset과 DataTable을 적극 활용하여 캐릭터, 적, 무기, 스킬, 사운드 등 대부분의 게임 요소를 데이터화했습니다. 이를 통해 코드 수정 없이 기획자가 직접 콘텐츠를 추가하고 밸런스를 조정할 수 있는 유연한 구조를 마련했습니다.

캐릭터 상태 처리: 피격 시 DynamicMaterialInstance를 활용한 머티리얼 변화, 죽음 시 디졸브(Dissolve) 이펙트 등 캐릭터의 주요 상태 변화에 따른 시각적 피드백을 GameplayCue와 연동하여 구현했습니다.

3. UI-로직 연동 및 AI
WidgetController 패턴 구현: UMG와 GAS를 효율적으로 연동하기 위해 WidgetController 패턴을 도입했습니다. 이를 통해 Attribute 변경 시 UI가 자동으로 업데이트되는 반응형 UI 시스템을 구축하고, UI와 게임 로직 간의 의존성을 최소화했습니다.

인게임 UI 구현: 데미지 플로터, 스킬 아이콘, 미니맵 등 전투에 필수적인 UI 요소들을 구현했습니다.

적 AI 설계: BehaviorTree와 OmochaEnemyDataAsset을 연동하여, 적의 등급과 클래스에 따라 다른 행동 패턴과 스킬셋을 가지는 AI 시스템을 설계했습니다.

💡 기술적 강점 및 포부
기술 역량
데이터 관리: DataTable과 DataAsset을 활용하여 게임의 핵심 요소를 데이터화하고, 이를 통해 기획 친화적이며 확장성 높은 아키텍처를 설계하는 데 능숙합니다.

Gameplay Ability System (GAS): AbilityTask, GameplayEffect, GameplayCue 등 GAS의 구성 요소를 깊이 있게 이해하고 있으며, 이를 활용해 복잡한 전투 시스템을 모듈화하여 구현할 수 있습니다. 특히, 커스텀 GameplayEffectContext와 ExecutionCalculation을 통해 데이터 손실 없이 안정적으로 정보를 처리하는 데 강점이 있습니다.

태그 기반 시스템: GameplayTag를 활용하여 어빌리티, 상태, 인풋, 아군 판별 등 게임의 핵심 로직을 유연하고 직관적으로 관리하는 시스템을 구축할 수 있습니다.

머티리얼 및 이펙트 연동: 다이나믹 머티리얼 인스턴스를 활용한 피격 효과, 오파시티 마스킹을 이용한 디졸브 효과 등 GameplayCue를 통해 프로그래밍과 아트를 유기적으로 연결하는 작업에 익숙합니다.

포부: "캐릭터의 매력을 극대화하는 개발자"
저는 전투 시스템, 호감도, 수집, 다이얼로그 시스템 등 캐릭터의 매력을 플레이어에게 온전히 전달하는 다양한 시스템 개발에 큰 흥미를 가지고 있습니다. 이번 프로젝트를 통해 다져진 GAS와 데이터 기반 설계 역량을 바탕으로, 매력적인 캐릭터들이 살아 숨 쉬는 게임을 만드는 데 기여하고 싶습니다.

🎬 영상
[![영상 제목 1](https://www.google.com/search?q=http://img.youtube.com/vi/[유튜브 영상 ID 1]/0.jpg)](https://youtu.be/[유튜브 영상 ID 1]?t=0s)
[![영상 제목 2](https://www.google.com/search?q=http://img.youtube.com/vi/[유튜브 영상 ID 2]/0.jpg)](https://youtu.be/[유튜브 영상 ID 2]?t=0s)<br>  

📸 스크린샷
<img src="[스크린샷 1 URL]">
<img src="[스크린샷 2 URL]">
<img src="[스크린샷 3 URL]">
<img src="[스크린샷 4 URL]">
