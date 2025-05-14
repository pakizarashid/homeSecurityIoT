# 🔐 IoT-Based Smart Home Security System
An **IoT-enabled home security system** built using Arduino Uno and Python. This project integrates motion detection, real-time alerting, and image capture to monitor home environments intelligently.



**📑 Features**

- **Motion Detection** using PIR sensor
- **Window Lock & Hazard Level Monitorin**g via potentiometers
- **Automated Door Locking **using a Servo motor
- **Visual and Audible Alerts** with LEDs and buzzer
- **Image Capture** using laptop camera (only when motion is detected)
- **Real-time Alerts** sent via Email and SMS using Python APIs
- **Periodic Status Monitoring** with customizable intervals



**📡 System Architecture**

[Arduino Uno] → [Laptop (Python script)] → [Internet (API Services)]

      ↑                  ↑                           ↑
      
   Sensors      |      Camera, Logic      |      Email & SMS Notifications
   


**🔌 Hardware Components**

- Arduino Uno
- PIR Motion Sensor
- 2x Potentiometers (Window Lock, Hazard Level)
- Servo Motor (Door Lock)
- LEDs (Status Indicators)
- Buzzer (Alarm)
- USB Connection to Laptop (for serial communication)
- Laptop Camera (for capturing images)




**💻 Software Stack**

* **Arduino C/C++** (for sensor control and serial communication)
* **Python 3** (for image capture, logic processing, and alerting)
* **Email & SMS APIs** (e.g., SMTP, Twilio)



**📝 Getting Started**

1. Upload the Arduino code to the Uno using Arduino IDE.
2. Run the Python script on your laptop (VS Studio).
3. Ensure API credentials (email/SMS) are configured.
4. System starts monitoring, capturing images and sending alerts.
