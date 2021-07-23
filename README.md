# DistanceSensingGlove
This is a haptic guidance glove for visually impaired.

An ultrasonic distance sensor which located at the hand palm provides distance measurements(up to 4.0 meters for specific sensor) to Arduino Uno. Arduino drives a small unbalanced vibration motor with a simple amplifier, distance information transferred with a reverse exponential function. 

Linear signal transformation does not work very well with close quarters, it is easier to sense closer objects with a reverse exponential curve.

Added a gyro for user inputs, deactivates the haptic feed if hand switches a relaxed state.

Original sketech has a LCD display for test purpose.

Tests - Sorry for the cemara angle and motor static, 2015 was a rough year.

[![First video](https://img.youtube.com/vi/JeSxJKtVPUc/0.jpg)](https://www.youtube.com/watch?v=JeSxJKtVPUc)

[![Second video](https://img.youtube.com/vi/OgBKlKTL4fs/0.jpg)](https://www.youtube.com/watch?v=OgBKlKTL4fs)



Diagrams

![Design](https://user-images.githubusercontent.com/28985966/126775627-03e1be77-84a2-4d69-a419-081964331378.png)

![Design_Breadbord](https://user-images.githubusercontent.com/28985966/126777317-b487df19-46eb-4d40-8bfc-4f9b605bd120.png)


Implemented form

![IMG-20160424-WA0004](https://user-images.githubusercontent.com/28985966/126776200-33cff45a-42f3-4475-9dd0-4d15587f7b87.jpeg)
