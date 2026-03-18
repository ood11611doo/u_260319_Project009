# u_260319_Project009

> **Unreal Engine 5** 기반 멀티플레이어 **숫자 야구 게임**

---

## 📖 게임 개요

중복 없는 1~9 사이의 숫자로 이루어진 **N자리 정답**을 맞추는 숫자 야구 게임입니다.  
플레이어들이 **턴 순서대로** 숫자를 입력하고, 스트라이크(S)·볼(B) 힌트를 받아 정답을 추리합니다.

| 결과 | 조건 |
|------|------|
| **Strike (S)** | 숫자와 자리가 모두 일치 |
| **Ball (B)** | 숫자는 일치하지만 자리가 다름 |
| **OUT** | 숫자가 하나도 일치하지 않음 |

---

## ✨ 주요 기능

- **멀티플레이어 지원** — Unreal Engine 리슨 서버 방식의 네트워크 복제
- **턴제 진행** — 플레이어 순서에 따라 자동으로 턴 전환
- **턴 타이머** — 제한 시간(기본 30초) 내 미입력 시 자동으로 시도 횟수 차감 후 다음 턴
- **시도 횟수 제한** — 플레이어별 최대 시도 횟수 초과 시 탈락(DEAD)
- **채팅 기반 입력** — UI 채팅창을 통해 숫자 입력 및 일반 채팅 동시 지원
- **게임 재시작** — 게임 종료 후 모든 플레이어가 `y` 입력 시 자동 재시작

---

## 🗂️ 프로젝트 구조

```
Source/u_260319_Project009/
├── u_260319_Project009.h           # 유틸리티 함수 모음 (FunctionLib)
├── u_260319_Project009.cpp         # 모듈 진입점
├── u_260319_Project009.Build.cs    # 빌드 의존 모듈 설정
│
├── Public/
│   ├── ProjectGameMode.h           # 게임 모드 헤더
│   ├── ProjectGameState.h          # 게임 스테이트 헤더
│   ├── Player/
│   │   ├── ProjectPawn.h           # 플레이어 폰 헤더
│   │   ├── ProjectPlayerController.h  # 플레이어 컨트롤러 헤더
│   │   └── ProjectPlayerState.h    # 플레이어 스테이트 헤더
│   └── UI/
│       └── ChatWidget.h            # 채팅 위젯 헤더
│
└── Private/
    ├── ProjectGameMode.cpp
    ├── ProjectGameState.cpp
    ├── Player/
    │   ├── ProjectPawn.cpp
    │   ├── ProjectPlayerController.cpp
    │   └── ProjectPlayerState.cpp
    └── UI/
        └── ChatWidget.cpp
```

---

## 📄 스크립트 상세 설명

### `FunctionLib` (`u_260319_Project009.h`)
프로젝트 전역에서 사용하는 정적 유틸리티 함수 모음입니다.

| 함수 | 설명 |
|------|------|
| `MyPrintString` | 클라이언트/리슨서버에서 화면 출력, 서버에서 로그 출력 |
| `GetNetModeString` | 현재 NetMode를 `Client` / `Server` / `StandAlone` 문자열로 반환 |
| `GetRoleString` | `LocalRole / RemoteRole` 문자열 반환 (디버그용) |

---

### `AProjectGameMode` (`ProjectGameMode`)
게임의 핵심 로직을 담당합니다. **서버에서만 존재**합니다.

| 함수 | 설명 |
|------|------|
| `GenerateAnswer()` | 중복 없는 N자리 정답 생성 |
| `CorResult()` | 입력값과 정답을 비교해 `XS YB` 또는 `OUT` 문자열 반환 |
| `ChatMessageStr()` | 채팅 입력 수신 — 유효한 추측이면 판정, 아니면 일반 채팅으로 전파 |
| `TryCountInc()` | 해당 플레이어의 시도 횟수 증가 |
| `AdvanceTurn()` | 다음 유효한 플레이어로 턴 전환, 없으면 게임 종료 |
| `ResultGame()` | 승리/패배 처리 및 재시작 대기 상태 전환 |
| `ResetGame()` | 정답 재생성, 시도 횟수 초기화 후 게임 재시작 |
| `StartTurnTimer()` | 턴 타이머 시작 (1초 간격 감소) |
| `OnTurnTimeExpired()` | 타이머 만료 시 시도 차감 후 다음 턴으로 강제 진행 |
| `UpdateAllPlayerStatuses()` | 모든 플레이어의 상태(`YourTurn` / `Waiting` / `NotReady`) 갱신 |
| `UpdateRestartUI()` | 재시작 준비 인원 현황 UI 업데이트 |
| `ChangeNotify / ChangeNotifyMore / ChangeNotifyTime` | 모든 플레이어의 알림 텍스트 복제 변수 갱신 |

---

### `AProjectGameState` (`ProjectGameState`)
게임 규칙 설정값을 보관하고 클라이언트에 복제합니다.

| 변수 | 기본값 | 설명 |
|------|--------|------|
| `AnswerLength` | `3` | 정답 자릿수 |
| `MaxTryCount` | `3` | 플레이어당 최대 시도 횟수 |
| `TurnTime` | `30` | 턴당 제한 시간 (초) |
| `CurrentTurnIndex` | `0` | 현재 턴 플레이어 인덱스 (복제) |

`IsCorrect(InputStr)` — 입력값의 유효성 검사 (자릿수, 중복, 0 포함 여부)

---

### `AProjectPlayerController` (`ProjectPlayerController`)
클라이언트-서버 통신의 핵심 역할을 합니다.

| 항목 | 설명 |
|------|------|
| **복제 변수** | `NotifyTxt`, `NotifyTxtMore`, `NotifyTime` — UI에 바인딩되는 알림 텍스트 |
| `SetChatMessage()` | 채팅 입력 전처리 — 자신의 턴이 아닌데 유효한 숫자 입력 시 경고 출력 |
| `ServerRPC_PrintChatMessage` | 클라이언트 → 서버로 채팅 메시지 전송 |
| `ClientRPC_PrintChatMessage` | 서버 → 클라이언트로 메시지 수신 및 화면 출력 |
| `BeginPlay()` | 로컬 컨트롤러에서만 `ChatWidget`, `NotifyWidget` 생성 및 뷰포트 추가 |

---

### `AProjectPlayerState` (`ProjectPlayerState`)
플레이어 개인 상태를 저장하고 복제합니다.

| 변수 | 설명 |
|------|------|
| `PLName` | 플레이어 이름 (`Player 1`, `Player 2`, ...) |
| `TryCount` | 현재까지 사용한 시도 횟수 |
| `MaxCount` | 최대 시도 횟수 |
| `CurrentStatus` | `NotReady` / `Waiting` / `YourTurn` |

`GetPlayerTry()` — `(1/3)` 형태의 시도 현황 문자열 반환, 탈락 시 `(DEAD)` 반환

---

### `AProjectPawn` (`ProjectPawn`)
플레이어가 빙의하는 폰입니다. `BeginPlay`와 `PossessedBy` 시점에 NetMode·Role 정보를 화면에 출력합니다 (디버그용).

---

### `UChatWidget` (`ChatWidget`)
채팅 UI 위젯입니다.

- `UEditableTextBox (ETBChatInput)` — 채팅 입력창 (BindWidget)
- Enter 입력 시 `PlayerController::SetChatMessage()` 호출 후 입력창 자동 초기화

---

## 🔄 게임 흐름

```
서버 시작
   │
   ▼
정답 생성 (BeginPlay)
   │
   ▼
플레이어 접속 (OnPostLogin) ──→ 이름/MaxCount 할당
   │
   ▼
턴 시작 (UpdateAllPlayerStatuses + StartTurnTimer)
   │
   ├─ [제한시간 초과] ──→ TryCount 차감 ──→ AdvanceTurn
   │
   └─ [채팅 입력]
         │
         ├─ 유효한 추측 + YourTurn 상태
         │       │
         │       ├─ 정답 일치 ──→ ResultGame (승리)
         │       └─ 불일치     ──→ 결과 전파 ──→ AdvanceTurn
         │
         └─ 일반 채팅 ──→ 모든 플레이어에게 전파
   │
   ▼
모든 플레이어 탈락 시 ResultGame (게임오버)
   │
   ▼
재시작 대기 — 모두 'y' 입력 시 ResetGame
```

---

## ⚙️ 빌드 의존 모듈

```csharp
PublicDependencyModuleNames: 
    "Core", "CoreUObject", "Engine", "InputCore", 
    "EnhancedInput", "UMG", "Slate", "SlateCore"
```

---

## 🛠️ 개발 환경

- **Engine** : Unreal Engine 5
- **Language** : C++
- **Network** : Listen Server (Unreal Replication)

