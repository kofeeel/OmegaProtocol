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

## 플레이
<a href="https://store.steampowered.com/app/3891070/Omega_Protocol_Demo/" target="_blank">
  <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/83/Steam_icon_logo.svg/512px-Steam_icon_logo.svg.png" alt="Steam" width="100"/>
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
* **UMG / Slate UI** - UI 시스템
  
#### 렌더링
* **Lumen** (Global Illumination)
* **Niagara/ Cascade Particle** (VFX)
* **Material Editor** (Shader)

#### 데이터 관리
* **DataAsset/DataTable**
* **Asset Manager**
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

## 담당 업무
* **GAS 기반 전투 시스템 설계 및 구현** - 데미지 구조체와 커스텀 GameplayEffectContext를 활용한 안정적인 데이터 전달 파이프라인 구축
* **데이터 기반 캐릭터 및 콘텐츠 시스템** - 데이터 에셋과 테이블을 활용한 확장 가능한 시스템 구축
* **데미지 시스템** - 복잡한 데미지 계산식 처리 
* **데미지 플로터** - 나이아가라시스템을 이용한 데미지 플로터
* **상태이상 및 피격효과** - 데미지 타입별 피격효과, 넉백, 상태이상 효과, 사망시 디졸브 
* **디버프 시스템** - GameplayEffect와 커스텀 GameplayCue를 활용한 다양한 디버프 효과 
* **플레이어와 적이 재사용 가능한 어빌리티 구조** - 투사체, 범위 공격, 이동기 등 목적에 맞는 어빌리티 기반 클래스 
* **네임드 몬스터** - BehaviorTree와 OmochaEnemyDataAsset을 연동한 AI 
* **오파시티 마스킹** - 
* **맵 충돌처리** - 커스텀 콜리전 프로필 생성
* **스토리/캐릭터/적/스킬 기획 및 밸런싱** 

## 핵심 구현 내용

#### 1. GAS 기반 전투 시스템
* **복잡한 데미지 처리 로직**: `ExecCalc_Damage` 클래스를 통해 기본 데미지, 스킬 계수, 치명타, 랜덤 편차 등을 포함한 복잡한 데미지 계산식 구현
* **상태 이상 및 넉백 시스템**: `FDamageEffectParams`와 커스텀 `FGameplayEffectContext`를 활용하여 디버프와 넉백 정보를 안정적으로 전달, 모든 스킬들은 데미지 어빌리티를 상속받아 데미지 정보를 적용
* **재사용 가능한 어빌리티 아키텍처**: 투사체(`ProjectileGameplayAbility`), 범위 공격(`TraceGameplayAbility`), 이동기(`MoveGameplayAbility`) 등 목적별 어빌리티 기반 클래스 설계

#### 2. 데이터 기반 캐릭터 시스템
* **동적 상태 전환**: 캐릭터의 '일반/오메가' 상태를 관리하는 `OmochaCharacterConfigComponent` 설계
* **데이터 중심 설계**: `DataAsset`과 `DataTable`을 활용하여 캐릭터, 적, 무기, 스킬 등을 데이터화
* **캐릭터 상태 처리**: `DynamicMaterialInstance`를 활용한 피격 효과, 디졸브 이펙트 등 `GameplayCue`와 연동

#### 3. UI-로직 연동
* **WidgetController 패턴**: UMG와 GAS를 효율적으로 연동하기 위한 패턴 구현
* **반응형 UI 시스템**: `Attribute` 변경 시 UI가 자동으로 업데이트되는 시스템 구축
* **인게임 UI**: 데미지 플로터, 스킬 아이콘, 미니맵 등 전투 필수 UI 요소 구현

## 기술적 강점
* **GAS 시스템**: AbilityTask, GameplayEffect, GameplayCue 등 GAS 구성 요소를 활용한 복잡한 전투 시스템 모듈화
* **데이터 관리**: DataTable과 DataAsset을 활용한 확장성 높은 아키텍처 설계
* **태그 기반 시스템**: GameplayTag를 활용한 유연하고 직관적인 게임 로직 관리
* **머티리얼 연동**: GameplayCue를 통한 프로그래밍과 아트의 유기적 연결

## 영상
[![영상 제목 1](http://img.youtube.com/vi/[유튜브 영상 ID 1]/0.jpg)](https://youtu.be/[유튜브 영상 ID 1]?t=0s)
[![영상 제목 2](http://img.youtube.com/vi/[유튜브 영상 ID 2]/0.jpg)](https://youtu.be/[유튜브 영상 ID 2]?t=0s)

## 스크린샷
![로비](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/1.png?raw=true)
![레벨 이동](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/2.png?raw=true)
![전투 1](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/3.png?raw=true)
![스크린샷 4](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/4.png?raw=true)
![게임오버 / 재시작](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/5.png?raw=true)
![러스티 3인 플레이](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/6.png?raw=true)
![스크린샷 4](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/8.png?raw=true)
![스크린샷 4](https://github.com/kofeeel/OmegaProtocol/blob/main/Image/9.png?raw=true)
