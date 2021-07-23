# DistanceSensingGlove
This is a guidance glove for visually impaired.

An ultrasonic distance sensor which located at the hand palm provides distance measurements(up to 4.0 meters for specific sensor) to Arduino Uno. Arduino drives a small unbalanced vibration motor with a simple amplifier, distance information transferred with a reverse exponential function. 

Linear signal transformation does not work very well with close quarters, it is easier to sense closer objects with a reverse exponential curve.

Original sketech has a LCD display for test purpose. 

Diagram

![Design](https://user-images.githubusercontent.com/28985966/126775627-03e1be77-84a2-4d69-a419-081964331378.png)

Implemented form

![IMG-20160424-WA0004](https://user-images.githubusercontent.com/28985966/126776200-33cff45a-42f3-4475-9dd0-4d15587f7b87.jpeg)
