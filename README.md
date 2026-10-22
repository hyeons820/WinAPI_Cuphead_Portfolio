# Cuphead — WinAPI 개인 프로젝트

> [작성 예정] 프로젝트를 한 문장으로 소개합니다.

[개요](#개요) · [하이라이트](#하이라이트) · [구현 범위](#구현-범위) · [주요 구현](#주요-구현) · [문제 해결](#문제-해결) · [회고](#회고)

<!-- 초안: GIF는 파일명 기준으로 배치했습니다. 장면별 제목과 설명은 함께 확인하며 채웁니다. -->

## 개요

| 항목 | 내용 |
| --- | --- |
| 프로젝트 | Cuphead 모작 |
| 개발 형태 | 개인 프로젝트 |
| 개발 기간 | [작성 예정] |
| 개발 환경 | [작성 예정] 언어 · API · 개발 도구 |
| 구현 범위 | [작성 예정] 플레이 가능한 콘텐츠와 직접 구현한 범위 |
| 전체 시연 영상 | 추후 추가 |

## 하이라이트

<p align="center">
  <img src="docs/Images/Opening.gif" width="800" alt="프로젝트 오프닝" />
</p>

<!-- 전체 시연 영상 링크는 준비 후 이곳에도 배치합니다. -->

## 구현 범위

| 분야 | 핵심 내용 |
| --- | --- |
| 플레이어 액션 | [작성 예정] 점프 · 낙하 · 패링 등 |
| 월드 · 씬 간 데이터 | [작성 예정] 월드 이동 · 씬 전환 시 유지할 정보 |
| 상점 · 인벤토리 | [작성 예정] 구매 · 소유 · 장착 흐름 |
| 보스 | [작성 예정] 패턴 · 페이즈 · 공격 연출 |
| 런 앤 건 | [작성 예정] 스테이지 구성 · 적 · 기믹 |

## 주요 구현

[1. 플레이어 액션](#1-플레이어-액션) · [2. 월드와 씬 간 데이터](#2-월드와-씬-간-데이터) · [3. 상점과 인벤토리](#3-상점과-인벤토리) · [4. 보스](#4-보스) · [5. 런 앤 건](#5-런-앤-건)

### 1. 플레이어 액션

[작성 예정] 플레이어 조작에서 중점적으로 구현한 내용을 소개합니다.

<table>
  <tr>
    <td width="50%" align="center"><img src="docs/Images/Player_01.gif" alt="플레이어 시연 1" /><br/><sub>장면 1 · 설명 작성 예정</sub></td>
    <td width="50%" align="center"><img src="docs/Images/Player_02.gif" alt="플레이어 시연 2" /><br/><sub>장면 2 · 설명 작성 예정</sub></td>
  </tr>
  <tr>
    <td colspan="2" align="center"><img src="docs/Images/Player_03.gif" width="600" alt="플레이어 시연 3" /><br/><sub>장면 3 · 설명 작성 예정</sub></td>
  </tr>
</table>

#### 점프와 낙하 상태 분리

[작성 예정] 분리한 이유 → 상태 전환 조건 → 플레이 결과.

#### 패링

[작성 예정] 입력 조건 → 판정 구간 → 성공 시 동작.

<details>
<summary><b>세부 구현과 관련 코드</b></summary>

- [작성 예정] 상태 전환 흐름과 주요 함수.
- [작성 예정] 공개 코드의 해당 함수 링크.

</details>

### 2. 월드와 씬 간 데이터

[작성 예정] 월드와 스테이지를 오갈 때 유지해야 하는 정보와 처리 흐름을 설명합니다.

<table>
  <tr>
    <td width="50%" align="center"><img src="docs/Images/World_01.gif" alt="월드 시연 1" /><br/><sub>장면 1 · 설명 작성 예정</sub></td>
    <td width="50%" align="center"><img src="docs/Images/World_02.gif" alt="월드 시연 2" /><br/><sub>장면 2 · 설명 작성 예정</sub></td>
  </tr>
</table>

#### 월드 플레이어와 이미지 상태

[작성 예정] 이동 방향과 상태에 따른 이미지 전환.

#### 씬 전환 시 데이터 유지

[작성 예정] 유지할 데이터 → 저장 시점 → 복원 시점.

<details>
<summary><b>세부 구현과 관련 코드</b></summary>

- [작성 예정] 데이터를 보관하는 클래스와 사용하는 클래스의 역할.
- [작성 예정] 공개 코드의 해당 함수 링크.

</details>

### 3. 상점과 인벤토리

[작성 예정] 코인 획득부터 구매·장착·전투 적용까지의 흐름을 설명합니다.

<table>
  <tr>
    <td width="50%" align="center"><img src="docs/Images/Store.gif" alt="상점 시연" /><br/><sub>상점 · 설명 작성 예정</sub></td>
    <td width="50%" align="center"><img src="docs/Images/Inventory.gif" alt="인벤토리 시연" /><br/><sub>인벤토리 · 설명 작성 예정</sub></td>
  </tr>
</table>

#### 구매 처리

[작성 예정] 구매 조건 → 코인 차감 → 소유 목록과 상점 목록 갱신.

#### 장착과 전투 적용

[작성 예정] 장비 선택 → 장착 정보 반영 → 스테이지에서 사용.

<details>
<summary><b>세부 구현과 관련 코드</b></summary>

- [작성 예정] 소유 아이템과 장착 아이템을 관리하는 방식.
- [작성 예정] 공개 코드의 해당 함수 링크.

</details>

### 4. 보스

[작성 예정] 구현한 보스와 주요 전투 흐름을 소개합니다.

<table>
  <tr>
    <td width="50%" align="center"><img src="docs/Images/Boss_01.gif" alt="보스 시연 1" /><br/><sub>장면 1 · 설명 작성 예정</sub></td>
    <td width="50%" align="center"><img src="docs/Images/Boss_02.gif" alt="보스 시연 2" /><br/><sub>장면 2 · 설명 작성 예정</sub></td>
  </tr>
  <tr>
    <td width="50%" align="center"><img src="docs/Images/Boss_03.gif" alt="보스 시연 3" /><br/><sub>장면 3 · 설명 작성 예정</sub></td>
    <td width="50%" align="center"><img src="docs/Images/Boss_04.gif" alt="보스 시연 4" /><br/><sub>장면 4 · 설명 작성 예정</sub></td>
  </tr>
</table>

#### 패턴과 페이즈 전환

[작성 예정] 패턴 선택 → 진행·종료 조건 → 페이즈 전환.

#### 애니메이션과 공격 타이밍

[작성 예정] 준비 동작과 투사체 생성 시점을 연결한 방식.

<details>
<summary><b>클리어 장면</b></summary>

<p align="center">
  <img src="docs/Images/Boss_Clear.gif" width="700" alt="보스 클리어 시연" />
</p>

</details>

<details>
<summary><b>세부 구현과 관련 코드</b></summary>

- [작성 예정] 보스 상태 흐름과 대표 공격 패턴.
- [작성 예정] 공개 코드의 해당 함수 링크.

</details>

### 5. 런 앤 건

[작성 예정] 스테이지 구성과 대표 기믹을 소개합니다.

<table>
  <tr>
    <td width="50%" align="center"><img src="docs/Images/Run%26Gun_01.gif" alt="런 앤 건 시연 1" /><br/><sub>장면 1 · 설명 작성 예정</sub></td>
    <td width="50%" align="center"><img src="docs/Images/Run%26Gun_02.gif" alt="런 앤 건 시연 2" /><br/><sub>장면 2 · 설명 작성 예정</sub></td>
  </tr>
  <tr>
    <td colspan="2" align="center"><img src="docs/Images/Run%26Gun_03.gif" width="600" alt="런 앤 건 시연 3" /><br/><sub>장면 3 · 설명 작성 예정</sub></td>
  </tr>
</table>

#### 스테이지 구성과 기믹

[작성 예정] 지형 · 적 · 발판 등 구성 요소와 상호작용.

#### 스테이지 진행과 종료

[작성 예정] 시작 연출 → 진행 → 클리어 또는 사망 → 월드 복귀.

<details>
<summary><b>세부 구현과 관련 코드</b></summary>

- [작성 예정] 대표 기믹의 동작과 충돌 처리.
- [작성 예정] 공개 코드의 해당 함수 링크.

</details>

## 문제 해결

<!-- 실제로 겪은 문제만 작성합니다. 아직 측정하지 않은 수치나 검증하지 않은 결과는 넣지 않습니다. -->

### 사례 1 · 제목 작성 예정

| 구분 | 내용 |
| --- | --- |
| 문제 상황 | [작성 예정] |
| 원인 | [작성 예정] |
| 해결 방법 | [작성 예정] |
| 확인한 결과 | [작성 예정] |

## 회고

- **배운 점:** [작성 예정]
- **현재 구현의 한계:** [작성 예정]
- **개선하고 싶은 점:** [작성 예정]

## 기술 스택

| 기술 | 프로젝트에서의 활용 |
| --- | --- |
| [작성 예정] | [작성 예정] |

## 코드 공개 범위와 리소스 출처

- **공개한 코드:** [작성 예정] 직접 작성한 코드와 제공된 기반 코드의 범위.
- **빌드·실행 안내:** [작성 예정] 코드 샘플만 제공하는지, 독립적으로 실행 가능한지 명시.
- **원작 및 리소스 출처:** [작성 예정]
