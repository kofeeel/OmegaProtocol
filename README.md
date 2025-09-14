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

## 담당 업무
* **GAS 기반 전투 시스템 설계 및 구현** - 데미지 구조체와 커스텀 GameplayEffectContext를 활용한 안정적인 데이터 전달 파이프라인 구축
* **데이터 기반 캐릭터 및 콘텐츠 시스템** - 데이터 에셋과 테이블을 활용한 확장 가능한 시스템 구축. 적,캐릭터, 스킬관련 정보를 테이블로 밸런싱 가능하도록 구현 
* **데미지 시스템** - 데미지 계산 클래스 하나로 복잡한 계산을 중앙에서 처리, DamageAbility를 만들어 데미지계산을 추상화
* **네임드 몬스터** - BehaviorTree와 DataAsset을 연동한 AI. 
* **지오메트리 페이드 시스템** - 오파시티 마스킹 노드를 제어해 플레이어 시야를 가리는 오브젝트를 마스킹 
* **맵 충돌처리** - 커스텀 콜리전 프로필 생성
* **스토리/캐릭터/적/스킬 기획 및 밸런싱** 

## 핵심 구현 내용

#### 1. GAS 기반 전투 시스템
* **복잡한 데미지 처리 로직**: `ExecCalc_Damage` 클래스를 통해 기본 데미지, 스킬 계수, 치명타, 랜덤 편차 등을 포함한 복잡한 데미지 계산식 구현
* **상태 이상 및 넉백 시스템**: `FDamageEffectParams`와 커스텀 `FGameplayEffectContext`를 활용하여 디버프와 넉백 정보를 안정적으로 전달, 모든 스킬들은 데미지 어빌리티를 상속받아 데미지 정보를 적용

#### 2. 데이터 기반 캐릭터 시스템
* **데이터 중심 설계**: `DataAsset`과 `DataTable`을 활용하여 캐릭터, 적, 무기, 스킬 등을 데이터화
  
#### 3. 시각적 피드백 시스템
* **데미지 플로터** - 나이아가라시스템을 이용한 데미지 플로터
* **상태이상 및 피격효과** - 데미지 타입별 피격효과, 넉백, 상태이상 효과, 사망시 디졸브 효과 
* **디버프 시스템** - GameplayEffect와 커스텀 GameplayCue를 활용한 다양한 디버프 효과
  
#### 4. UI-로직 연동
* **WidgetController 패턴**: UMG와 GAS를 효율적으로 연동하기 위한 패턴 구현
* **반응형 UI 시스템**: `Attribute` 변경 시 UI가 자동으로 업데이트되는 시스템 구축
* **인게임 UI**: 데미지 플로터, 스킬 아이콘, 미니맵 등 전투 필수 UI 요소 구현


## 기술적 강점
* **GAS 시스템**: AbilityTask, GameplayEffect, GameplayCue 등 GAS 구성 요소를 활용한 복잡한 전투 시스템 모듈화
* **데이터 관리**: DataTable과 DataAsset을 활용한 확장성 높은 아키텍처 설계
* **태그 기반 시스템**: GameplayTag를 활용한 유연하고 직관적인 게임 로직 관리
* **머티리얼 연동**: GameplayCue를 통한 프로그래밍과 아트의 유기적 연결

## 영상
<a href="https://drive.google.com/file/d/1PXTkZG2929e6r2iOkmVmSGP7RowUyHFd/view?usp=sharing" target="_blank">
  <img src="data:image/png;base64,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" alt="Steam" width="100"/>
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
