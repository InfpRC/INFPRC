# IRC 서버 (ft_irc)

## 소개

이 프로젝트는 C++ 98을 사용하여 직접 IRC(Internet Relay Chat) 서버를 구현하는 과제입니다. 서버는 여러 클라이언트를 처리하고, 다양한 IRC 명령어를 지원하며, TCP/IP를 통한 실시간 통신을 가능하게 합니다.

## 주요 기능

- **다중 클라이언트 지원**: 서버는 동시에 여러 클라이언트를 처리할 수 있습니다.
- **비차단 I/O**: 비차단 소켓과 `poll()` (또는 이에 상응하는 함수)를 사용하여 효율적으로 I/O 작업을 관리합니다.
- **IRC 명령어**:
    - **NICK**: 클라이언트의 닉네임 설정
    - **USER**: 클라이언트의 사용자 이름 설정
    - **JOIN**: 채널에 참여
    - **PRIVMSG**: 다른 사용자에게 개인 메시지 전송
    - **KICK**: 운영자가 클라이언트를 채널에서 강제 퇴장
    - **INVITE**: 운영자가 다른 클라이언트를 채널에 초대
    - **TOPIC**: 채널 주제 설정 또는 조회
    - **MODE**: 채널 모드 변경 (예: 초대 전용, 비밀번호 보호 등)
- **운영자와 일반 사용자**: 채널 운영자와 일반 사용자의 권한을 구분하여 구현합니다.

## 요구 사항

- **컴파일러**: 프로젝트는 `Wall -Wextra -Werror` 플래그와 함께 컴파일되어야 합니다.
- **표준**: C++ 98 표준을 준수해야 합니다.
- **Makefile**: 프로젝트에는 `all`, `clean`, `fclean`, `re` 규칙이 포함된 Makefile이 포함되어야 합니다.
- **외부 라이브러리**: Boost를 포함한 외부 라이브러리 사용은 금지됩니다.
- **파일 디스크립터**: MacOS를 포함한 모든 Unix 시스템에서 비차단 모드로 설정해야 합니다.

## 시작하기

### 컴파일

프로젝트를 컴파일하려면 다음 명령어를 실행합니다:

```
make
```

### 서버 실행

IRC 서버를 시작하려면:

```
./ircserv <port> <password>
```

- **port**: 서버가 수신 대기할 포트 번호.
- **password**: 클라이언트가 서버에 연결할 때 필요한 비밀번호.

### 서버 테스트

서버의 기능을 테스트하기 위해 다양한 IRC 클라이언트를 사용할 수 있습니다. 간단한 테스트를 위해 `nc`(netcat)를 사용하여 명령어를 수동으로 전송할 수 있습니다:

```
nc 127.0.0.1 6667
```

명령어를 여러 부분으로 나누어 전송하여 서버가 부분 데이터 전송을 올바르게 처리하는지 확인하십시오.

## MacOS 전용 지침

다른 Unix 계열 시스템과 비교하여 MacOS에서 `write()` 함수가 다르게 구현되었기 때문에, 파일 디스크립터를 다음과 같이 비차단 모드로 설정해야 합니다:

```cpp
fcntl(fd, F_SETFL, O_NONBLOCK);
```

비차단 모드를 설정할 때는 다른 플래그를 사용하는 것이 금지됩니다.

## 오류 처리

서버는 부분 데이터 수신, 낮은 대역폭 시나리오 및 기타 네트워크 관련 문제를 포함한 다양한 오류와 문제를 견고하게 처리하도록 설계되었습니다.
