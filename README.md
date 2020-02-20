# robocar
stm32f103c8t6

## Setup SD card
[Download Raspberry Pi Zero WH image](https://polyuit-my.sharepoint.com/:u:/g/personal/ensylam_polyu_edu_hk/EfPde7mzcR5NiaSuWeZethsBixlZN2U6amez3VQq-VrPfw?e=RhzQuq)\
Write image to SD card. (using balenaEtcher or otherwise)\
Disable SD card reader (through device manager) and then enable it again.\
Modify the following files (in "boot" drive):

- wpa_supplicant.conf (ssid, password)
- hostname & hosts (raspberrypi -> newHostname, eg 19xxxxxxd)

Put SD card to raspberry pi zero and boot.

## SSH to raspberry pi
launch command prompt\
enter "ping -4 19xxxxxxd.local" to check ip address (Bonjour Print Service)\
SSH via putty or otherwise. (ssh pi@19xxxxxxd.local) (pwd = raspberry)

![image](DSC_0008.JPG)
[![](https://img.youtube.com/vi/ckLhx3viHC8/0.jpg)](https://youtu.be/ckLhx3viHC8)
