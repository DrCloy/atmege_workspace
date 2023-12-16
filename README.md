# RGB LED를 활용한 무드등 알람시계
Atmega128를 사용하여 임베디드 시스템 구현하기

## Profile
- 학번 : 2019920010
- 이름 : 김민교
- 과목 : 마이크로프로세서설계및실습

## Manual
### Upload Manual
- In Window Command Prompt (or Powershell)
```
<!-- Check USB BUS ID of Atmega board -->
usbipd wsl list

<!-- Connect Atmega Board to WSL -->
usbipd wsl attach -b <BUS ID of Atmega board>
```
- In WSL
```
<!-- In Workspace Directory -->
./upload.sh
```

### Device Manual
- FND가 정방향으로 보이는 상태에서 위쪽 버튼을 누르면 FND의 숫자가 1 증가, 아래쪽 버튼을 누르면 1 감소
- 아래 기능 목록에서 원하는 기능의 번호와 FND의 숫자가 일치하도록 한 뒤 양 버튼을 동시에 누르면 기능 시작
- 각 기능별로 종료 조건을 만족하면 다시 기능 선택 화면으로 돌아옴
- 기능 목록 및 사용 방법
    - FU.00 : 현재 시간 출력, 아무 버튼 누르면 기능 종료
    - FU.01 : 현재 시간 갱신, 위쪽 버튼 누르면 현재 갱신하고 있는 부분의 증가, 아래 버튼 누르면 현재 갱신하고 있는 부분의 감소, 양쪽 버튼 누르면 다음 갱신 부분으로 이동, 시간 -> 분 -> 종료 순서로 갱신 부분 변경됨, 종료 부분에서는 갱신한 시간 RTC 모듈에 반영 후 기능 종료


## Trial and Error
- timer0과 timer2의 인터럽트 충돌
    - timer0가 2ms 주기마다 인터럽트를 발생시키고, timer2가 1us 주기마다 인터럽트를 발생시키게 했을 때, timer0의 인터럽트가 발생되지 않았음
    - 이는 timer2의 인터럽트가 timer0에 비해 너무 짧은 주기로 실행되어서 timer0의 인터럽트가 발생했을 때는 이미 timer2의 인터럽트가 발생되고 있는 상태여서 timer0의 인터럽트가 무시되지 않았나 싶음
    - 알고리즘 상 1us 까지의 정확도는 필요 없고, 1ms 정도의 정확도로도 충분할 것으로 생각되어 timer2의 주기를 1us에서 1ms로 변경하여 해결
    