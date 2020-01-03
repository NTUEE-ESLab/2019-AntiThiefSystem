STM32 setup:

1.The libraries required for STM32:
   http://os.mbed.com/teams/ST/code/BSP_B-L475E-IOT01/
   https://github.com/ARMmbed/mbed-os/
   https://developer.mbed.org/teams/ST/code/ST_INTERFACES/
   https://developer.mbed.org/teams/ST/code/X_NUCLEO_COMMON/
   https://github.com/ARMmbed/wifi-ism43362

2.Change the wifi SSID and password in "mbed_app.json" to yours.

3.Change the "addr" to your PC's IP adress, "addrrpi" to your RPi's IP adress.

4.Connect ArduCAM 2MP ov2640 to your STM32.

5.Power on STM32 only after PC or RPi is waiting for connection.

6.While LED1 is flashing, STM32 is establishing internet connection.
  (connecting to wifi and then connect to socket server)

7.While LED2 is on, STM32 is detecting if anyone passes through.
  If LED2 is flashing, STM32 is too far from the target surface.

8.While LED3 is on, STM32 is sending data to the server.

With PC:

1.The libraries required for ATforPC.py:
   pip install pynput
   pip install pygame

2.Change HOST(IP adress) to yours.

3.Create folders called "images" and "alarmimage".
  Download "0.jpg" and "alarm.wav" into "alarmimage".

4.Execute ATforPC.py (python ATforPC.py) to start the program.

5.Power on STM32 to connect to the program and start working.


With RPi:

1.Connect button to pin BCM 4, LED to pin BCM 18, buzzer to pin BCM 23.

2.Change HOST(IP adress) to yours.

3.Change toaddr to your email address.

4.Power on RPi to execute the program.

5.The LED will start flashing until connection with STM32 is established.

6.Power on STM32 to connect to the program and start working.

7.After connected, you can use the button to control the on/off of the system.
  The LED will represent the on/off of the system.

8.If the alarm is triggered, the buzzer will ring, 
  and you will get an email with the intruder's picture.