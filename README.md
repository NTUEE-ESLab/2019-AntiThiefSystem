# Anti-Thief System #
* Detects if anyone passes by
* Takes picture and rings the alarm when someone does pass by
* Browse and clear the pictures taken through a pygame interface
* Receive email of picture taken upon detection
* Switch windows when someone passes by

## Motivation ##

For houses without steel windows, people who often forget to lock their windows at night, 
or those who would not enjoy trespassers roaming in their backyard gardens, they will 
probably be in need of an anti-thief system. This system will alarm the house owners of 
intruders and help record their faces.

## STM32 setup ##

1. The libraries required for STM32:
   * [BSP_B-L475E-IOT01](http://os.mbed.com/teams/ST/code/BSP_B-L475E-IOT01/)
   * [mbed-os](https://github.com/ARMmbed/mbed-os/)
   * [ST_INTERFACES](https://developer.mbed.org/teams/ST/code/ST_INTERFACES/)
   * [X_NUCLEO_COMMON](https://developer.mbed.org/teams/ST/code/X_NUCLEO_COMMON/)
   * [wifi-ism43362](https://github.com/ARMmbed/wifi-ism43362)

2. Change the wifi SSID and password in "mbed_app.json" to yours.

3. Change the "addr" to your PC's IP adress, "addrrpi" to your RPi's IP adress.

4. Connect ArduCAM 2MP ov2640 to your STM32.

5. Power on STM32 only after PC or RPi is waiting for connection.

6. While LED1 is flashing, STM32 is establishing internet connection.
  (connecting to wifi and then connect to socket server)

7. While LED2 is on, STM32 is detecting if anyone passes through.
  If LED2 is flashing, STM32 is too far from the target surface.

8. While LED3 is on, STM32 is sending data to the server.

## With PC ##

1. The libraries required for ATforPC.py:
   pip install pynput
   pip install pygame

2. Change HOST(IP adress) to yours.

3. Create folders called "images" and "alarmimage".
  Download "0.jpg" and "alarm.wav" into "alarmimage".

4. Execute ATforPC.py (python ATforPC.py) to start the program.

5. Power on STM32 to connect to the program and start working.


## With RPi ##

1. Connect button to pin BCM 4, LED to pin BCM 18, buzzer to pin BCM 23.

2. Change HOST(IP adress) to yours.

3. Change toaddr to your email address.

4. Power on RPi to execute the program.

5. The LED will start flashing until connection with STM32 is established.

6. Power on STM32 to connect to the program and start working.

7. After connected, you can use the button to control the on/off of the system.
  The LED will represent the on/off of the system.

8. If the alarm is triggered, the buzzer will ring, 
  and you will get an email with the intruder's picture.
  
## STM32 Details ##

1. Utilizes socket connection and semaphore

2. The main thread detects the nearest surface continuously, when the distance shortens 
  by 7%, it releases the semaphore and waits for semaphore
   
3. Create another thread to continuosly wait for the semaphore, when it gets the semaphore, 
  send out notification and picture, realease the semaphore and wait for it again after finishing
  
4. Thus, sends out a notification and a picture everytime the distance detected shortens
  by 7%

## PC Details ##

1. Utilizes socket connection and mutex

2. The main thread is processing UI

3. Create another thread to wait for notifications and pictures from STM32 with socket

4. Use mutex to protect the read/write of the photos, only one thread can access it at a time

## RPi Details ##

1. Utilizes socket connection and asynchronous inputs

2. The main thread waits for notifications and pictures from STM32 with socket

3. Create another thread to detect interrupt input from the button

4. Create another thread to control the buzzer

## Results ##

After detecting trespasser, the siren will ring and picture will be sent and shown through 
the pygame interface if STM32 is connected to the PC through wifi. If the STM32 is connected
to RPi through wifi instead, the buzzer will ring and the picture will be sent to a set email.
Results can be seen in the following video: [Demo Video](https://www.youtube.com/watch?v=UVwlfkKo5FU&t=12s)

## References ##

* https://pythonprogramming.net/pygame-python-3-part-1-intro/
* https://blog.techbridge.cc/2019/10/19/how-to-build-up-game-with-pygame-tutorial/
* https://www.st.com/en/imaging-and-photonics-solutions/vl53l0x.html 
* https://hackaday.io/project/166996-daytripper-hide-my-windows-laser-tripwire 
