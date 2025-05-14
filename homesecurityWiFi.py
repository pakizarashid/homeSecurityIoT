import cv2
import os
import time
import serial
import yagmail
from twilio.rest import Client

# ----------- Email Setup ------------
sender_email = "@gmail.com"           
receiver_email = "prasheed.mscs24seecs@seecs.edu.pk"    
app_password = "" 
yag = yagmail.SMTP(user=sender_email, password=app_password)

# ----------- SMS Setup ------------
def send_sms(message):
    account_sid = ''
    auth_token = ''
    client = Client(account_sid, auth_token)

    message = client.messages.create(
        body=message,
        from_='+17',  
        to='+92310'    
    )
    print(f"📲 SMS sent! SID: {message.sid}")

# ----------- Serial Setup ------------
try:
    arduino = serial.Serial('COM12', 9600) 
    time.sleep(2)  # Wait for connection
    print("✅ Connected to Arduino on COM12\n")
except Exception as e:
    print(f"❌ Error: {e}")
    exit()

# ----------- Camera Function ------------
def capture_image():
    cap = cv2.VideoCapture(0)
    ret, frame = cap.read()
    filename = None
    if ret:
        filename = f"snapshot_{int(time.time())}.jpg"
        cv2.imwrite(filename, frame)
        print(f"📸 Snapshot saved: {filename}")
    cap.release()
    return filename

# ----------- Main Monitoring Loop ------------
alert_sent = False
last_snapshot = None
# print("🔍 Monitoring Arduino serial output...\n")

try:
    while True:
        if arduino.in_waiting > 0:
            data = arduino.readline().decode('utf-8').strip()
            print(data)

            if "Motion detected" in data:
                last_snapshot = capture_image()

            if any(x in data for x in ["DANGER", "Security Breach", "High Risk"]) and not alert_sent:
                print("🚨 Danger detected! Sending email...")

                contents = [
                    "An alert has been triggered in your Smart Home System.",
                    f"Details:\n{data}",
                    "Please check your home immediately.",
                ]

                attachments = [last_snapshot] if last_snapshot and os.path.exists(last_snapshot) else []

                yag.send(
                    to=receiver_email,
                    subject="🚨 Home Security Alert",
                    contents=contents,
                    attachments=attachments
                )

                send_sms("🚨 Home Alert! Motion or Danger detected. Check your email for snapshot.")

                print("✅ Email and SMS are sent.")
                alert_sent = True

            if "Motion stopped" in data or "System Initialized" in data:
                alert_sent = False

except KeyboardInterrupt:
    print("\n🛑 Monitoring stopped.")

finally:
    arduino.close()
    print("🔌 Serial connection closed.")
