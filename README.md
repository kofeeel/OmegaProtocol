# Omega Protocol

<div align="center">
  <a href="https://store.steampowered.com/app/3891070/Omega_Protocol_Demo/" target="_blank">
    <img src="https://github.com/kofeeel/OmegaProtocol/blob/main/Image/omegaimage.png" width="800"/>
  </a>
  

</div>

## 개요
* "3명 중 한명만 강해질 수 있다! 전략적 판단으로 스테이지를 돌파하라!" 쿼터뷰 로그라이크 협동 액션!! 우주를 항해하던 세 명의 우주 경찰이 행성 충돌 사고로 우주 해적선에 불시착하면서 벌어지는 유쾌한 탈출기
* 어린아이들이 장난감 하나를 가지고 놀기 위해 다투는 모습에서 기획되었습니다.
* 강한 힘을 1명의 플레이어만 사용할 수 있다는 제한된 환경 속에서 3명의 플레이어가 협동과 경쟁을 통해 스테이지를 돌파하는 게임입니다.
* 장르: 로그라이트, PvE, 온라인, 협동, 쿼터뷰 슈팅게임
* 개발기간 : 2025.6.13 ~ 2025.9.9
* 플랫폼: Steam (Demo 출시)
  
## 플레이
<a href="https://store.steampowered.com/app/3891070/Omega_Protocol_Demo/" target="_blank">
  <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/83/Steam_icon_logo.svg/512px-Steam_icon_logo.svg.png" alt="Steam" width="100"/>
</a>
<a href="https://youtu.be/Iia-4RZ8x-8?si=UYuBpHaU8vrV6uMW" target="_blank">
  <img src="https://i.namu.wiki/i/96PzWjU0X4PJWSDG6rRFFgG3dkGLIw06-YMpHg_CVHnURSIHuxA9sF9CrJoXsZISwWeo19Y3LgIQnL1krbrcOg.svg" alt="플레이 영상" width="100"/>
</a>


## 사용 기술 스택
#### 개발 환경
* **Unreal Engine 5.5** (C++/Blueprint)
* **Rider**
* **Perforce** (버전 관리)

#### 핵심 프레임워크
* **Gameplay Ability System (GAS)** - 전투/스킬 시스템
* **Enhanced Input System** - 입력 처리
* **Steam Online Subsystem** - 멀티플레이
* **UMG** - UI 시스템
  
#### 렌더링
* **Lumen** (Global Illumination)
* **Niagara/ Cascade Particle** (VFX)
* **Material Editor** (Shader)

#### 데이터 관리
* **DataAsset/DataTable**
* **GameplayTag System**

#### 최적화 & 빌드
* **Profiler** (성능 분석)
* **Custom Collision Channels**
* **Shipping Build Configuration**

## 제작자
|<img src="https://github.com/leebo155.png" width=240>|<img src="https://github.com/shng6815.png" width="240">|<img src="https://github.com/kofeeel.png" width=240>|<img src="https://github.com/Baekbanjang.png" width=240>|<img src="https://github.com/chaseunggeun.png" width=240>|
|:--:|:--:|:--:|:--:|:--:|
|[Bohyeong Lee](https://github.com/leebo155)|[SEO HUIYEONG](https://github.com/shng6815)|[Hasimu](https://github.com/kofeeel)|[Baekbanjang](https://github.com/Baekbanjang)|[chaseunggeun](https://github.com/chaseunggeun)|
|PD/개발|사운드/개발|기획/개발|시네마틱/개발|PM/개발|

## 담당 업무 및 핵심 구현 내용

이 프로젝트에서 **데이터 중심의 전투 시스템과 캐릭터 시스템 아키텍처 설계 및 구현**을 담당했습니다.

### 주요 담당 업무

* **GAS 기반 전투 시스템 설계 및 구현** - 데미지 구조체와 커스텀 GameplayEffectContext를 활용한 안정적인 데이터 전달 파이프라인 구축
* **데이터 기반 캐릭터 및 콘텐츠 시스템** - 데이터 에셋과 테이블을 활용한 확장 가능한 시스템 구축. 적,캐릭터, 스킬관련 정보를 테이블로 밸런싱 가능하도록 구현 
* **데미지 시스템** - 데미지 계산 클래스 하나로 복잡한 계산을 중앙에서 처리, DamageAbility를 만들어 데미지계산을 추상화
* **네임드 몬스터** - BehaviorTree와 DataAsset을 연동한 AI. 
* **지오메트리 페이드 시스템** - 오파시티 마스킹 노드를 제어해 플레이어 시야를 가리는 오브젝트를 마스킹 
* **맵 충돌처리** - 커스텀 콜리전 프로필 생성
* **스토리/캐릭터/적/스킬 기획 및 밸런싱**

### 핵심 구현 내용

#### 1. GAS 기반 전투 시스템

##### 설계 배경
프로젝트 초기에는 밸런싱을 고려하지 않고 개발이 진행되었습니다. 이로 인해 스킬이 증가할수록 중앙 관리가 어려워지고, 밸런싱을 위해서는 각 어빌리티 블루프린트에 직접 접근해야 하는 비효율적인 상황이 발생했습니다. 

이러한 문제를 해결하기 위해 공통 속성들을 중앙집중식으로 관리할 수 있는 데미지 시스템을 설계했습니다. 스킬 최대/최소 범위, 데미지 수치, 넉백 강도 등 스킬 커스터마이징에 필요한 모든 정보를 `FDamageEffectParams` 구조체로 통합하고, 모든 데미지 어빌리티가 이를 상속받아 일관된 파라미터 생성이 가능하도록 구현했습니다.

**통합 데미지 처리 파이프라인**

**통합 데미지 처리 파이프라인**
- `FDamageEffectParams` 구조체를 통한 데미지 정보 일관성 있는 전달
- 모든 데미지 어빌리티가 `UDamageGameplayAbility`를 상속받아 `MakeDamageEffectParamsFromClassDefaults()` 메서드로 데이터 테이블 기반 자동 파라미터 생성
- `ApplyDamageEffect()` 단일 메서드로 모든 데미지 처리 추상화

**데미지 계산 시스템**
- 스킬 데이터별 기본 데미지 + 스킬 계수 + 치명타 + 회피율 + 랜덤 편차를 포함한 복합 계산
- `ApplyBuildBasedDamage()`를 통한 빌드 시스템과 연동된 추가 데미지 (최대 체력 비례 데미지 등)
- 쉴드 시스템과 방어력 관통 로직 처리

**커스텀 GameplayEffectContext 활용**
- 넉백 방향/크기, 디버프 타입/확률, 치명타 여부 등의 추가 정보를 데이터 손실 없이 어빌리티→계산클래스→적용까지 전달
- `DetermineDebuff()`와 `ApplyKnockback()` 메서드를 통한 상태이상과 넉백의 분리된 처리

#### 2. 데이터 기반 캐릭터 시스템

**DataTable 기반 초기화**
- `ApplyAttributesFromRowName()` 메서드를 통해 캐릭터별 스탯을 DataTable에서 로드하여 적용
- `AssignTagSetByCallerMagnitude()` 패턴으로 GameplayTag와 수치값을 매핑하여 유연한 속성 관리
  
**데이터 중심 스킬 시스템**
- `FOmochaSkillData` 구조체에 데미지, 쿨타임, 넉백정보 통합 관리
- `SkillDataTable->FindRow<>()`를 통해 런타임에 스킬 데이터를 조회하여 데미지 파라미터에 값을 오버라이드
- 기획자가 엑셀/CSV로 관리 가능한 밸런스 튜닝 환경

**성능 최적화 고려사항**
- 필수 Null 체크를 통한 안정성 확보 (`check()` 매크로 활용)
- DataTable 조회 결과 캐싱을 통한 중복 접근 최소화
- `LoadObject<>()` 사용 시 하드 레퍼런스 관리로 메모리 효율성 확보

**팀 협업 친화적 구조**
- 코드 수정 없이 DataTable/DataAsset 수정만으로 콘텐츠 추가 가능
- 명확한 인터페이스 분리로 프로그래머-기획자 간 의존성 최소화
- `FGameplayTag` 기반 시스템으로 enum 수정 없이도 확장 가능

#### 3. 시각적 피드백 시스템

* **데미지 플로터** - 나이아가라시스템을 이용한 데미지 플로터
* **상태이상 및 피격효과** - 데미지 타입별 피격효과, 넉백, 상태이상 효과, 사망시 디졸브 효과 
* **디버프 시스템** - GameplayEffect와 커스텀 GameplayCue를 활용한 다양한 디버프 효과
  
#### 4. UI-로직 연동 및 AI

**WidgetController 패턴 구현**
- UMG와 GAS를 효율적으로 연동하기 위해 WidgetController 패턴 도입
- `Attribute` 변경 시 UI가 자동으로 업데이트되는 반응형 UI 시스템 구축
- UI와 게임 로직 간의 의존성 최소화

**인게임 UI 구현**
- 데미지 플로터, 스킬 아이콘, 미니맵 등 전투에 필수적인 UI 요소들을 구현

**적 AI 설계**
- `BehaviorTree`와 `OmochaEnemyDataAsset`을 연동하여, 적의 등급과 클래스에 따라 다른 행동 패턴과 스킬셋을 가지는 AI 시스템 설계
- 어빌리티를 5개 가지고 있는 네임드 몬스터 구현. 가장 가깝거나, 가장 먼 플레이어 위치에 따라 다른 로직을 가진 행동트리 설계

## 기술적 강점 및 성과

### 기술 역량

**Gameplay Ability System (GAS)**
- `AbilityTask`, `GameplayEffect`, `GameplayCue` 등 GAS의 구성 요소를 깊이 있게 이해하고 활용
- 복잡한 전투 시스템을 모듈화하여 구현 가능
- 커스텀 `GameplayEffectContext`와 `ExecutionCalculation`을 통해 데이터 손실 없이 안정적으로 정보를 처리하는 데 강점

**데이터 관리**
- `DataTable`과 `DataAsset`을 활용하여 게임의 핵심 요소를 데이터화하고, 기획 친화적이며 확장성 높은 아키텍처를 설계하는 데 능숙

**태그 기반 시스템**
- `GameplayTag`를 활용하여 어빌리티, 상태, 인풋, 아군 판별 등 게임의 핵심 로직을 유연하고 직관적으로 관리하는 시스템 구축

**머티리얼 및 이펙트 연동**
- 다이나믹 머티리얼 인스턴스를 활용한 피격 효과, 오파시티 마스킹을 이용한 디졸브 효과 등 `GameplayCue`를 통해 프로그래밍과 아트를 유기적으로 연결하는 작업에 익숙

### 프로젝트 성과

- **개발 효율성**: 데이터 테이블 기반 시스템으로 새로운 스킬 추가 작업 시간 대폭 단축
- **밸런싱 생산성**: 코드 수정 없이 실시간 밸런스 조정 가능한 환경 구축
- **시스템 안정성**: 커스텀 컨텍스트를 통한 데이터 무결성 보장

### 프로젝트 기여도

**아키텍처 설계자**로서 GAS 기반 전투 시스템의 전체 구조를 설계하고, 데이터 중심 접근법을 통해 팀 전체의 개발 생산성과 유지보수성을 크게 향상시켰습니다. 특히 복잡한 GAS 시스템을 추상화하여 팀원들이 쉽게 콘텐츠를 추가할 수 있는 환경을 구축했습니다. 
또한 피격, 사망, 디버프 등의 시각적 피드백 시스템을 구현하여 게임의 완성도를 높였습니다.


## 영상
<a href="https://drive.google.com/file/d/1PXTkZG2929e6r2iOkmVmSGP7RowUyHFd/view?usp=sharing" target="_blank">
  <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/12/Google_Drive_icon_%282020%29.svg/2295px-Google_Drive_icon_%282020%29.svg.png" alt="영상" width="100"/>
</a>

## 스크린샷
![로비](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/1.png?raw=true)
![레벨 이동](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/2.png?raw=true)
![전투 1](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/3.png?raw=true)
![스크린샷 4](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/4.png?raw=true)
![게임오버 / 재시작](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/5.png?raw=true)
![러스티 3인 플레이](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/6.png?raw=true)
![스크린샷 4](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/8.png?raw=true)
![스크린샷 4](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/9.png?raw=true)
