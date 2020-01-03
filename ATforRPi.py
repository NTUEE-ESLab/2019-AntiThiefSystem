import smtplib,ssl
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email.mime.text import MIMEText
from email.utils import formatdate
from email import encoders
import time
import os
import socket
import threading
import RPi.GPIO as GPIO

# Setup GPIO pins
GPIO.setmode(GPIO.BCM);
GPIO.setup(4,GPIO.IN,pull_up_down=GPIO.PUD_UP);  # Button input
GPIO.setup(18,GPIO.OUT);                         # LED output
GPIO.setup(23,GPIO.OUT);                         # Buzzer output

HOST = '172.20.10.5'  # Standard loopback interface address
PORT = 12001          # Port to listen on
alarmon = 1           # Control the on/off of the system
alarm = 0             # Tell the alarm thread to ring the buzzer
fl = 1                # Control the working of the LED thread

# send an email with file as an attached file
def send_an_email(file):  
	toaddr = 'alarmfromrpi@gmail.com'      # To id 
	me = 'alarmfromrpi@gmail.com'          # my id
	subject = "Alarm"                      # Subject
	text = "Alarm triggered."              # Text content of the email

	msg = MIMEMultipart()  
	msg['Subject'] = subject  
	msg['From'] = me  
	msg['To'] = toaddr  

	msg.attach(MIMEText(text))
	part = MIMEBase('application', "octet-stream")  
	
	part.set_payload(file)  
	encoders.encode_base64(part)  
	part.add_header('Content-Disposition', 'attachment; filename="alarm.jpg"')  # File name and format name
	msg.attach(part)  

	try:
		m.sendmail(me, toaddr, msg.as_string())  
	except SMTPException as error:  
		print ("Error: unable to send email")  # Exception

# Switch between on\off of the system
def active(channel):
	global alarmon 
	if alarmon == 1:
		alarmon = 0
		GPIO.output(18, False);
	else:
		alarmon = 1
		GPIO.output(18, True);

# Convert bytes received to an integer
def bytes_to_int(bytes):
	result = 0
	for b in bytes:
		result = result * 10 + int(b)
	return result

# Keep waiting for notification from STM32
def waitforpics(conn):
	global alarm
	while True:
		while True:
			try:
				num = bytes_to_int(conn.recv(1024))
				break
			except:
				pass
		if alarmon == 1:
			alarm = 1
			file = b''
			while num!=0:
				data = conn.recv(16384)
				num = num - len(data)
				file = file + data
			send_an_email(file)
		else:
			while num!=0:
				data = conn.recv(16384)
				num = num - len(data)

# Flash the LED
def flash():
	while fl == 1:
		GPIO.output(18, False);
		time.sleep(0.5);
		GPIO.output(18, True);
		time.sleep(0.5);

# Control the buzzer
def alarm():
	global alarm
	while True:
		if alarm == 1:
			GPIO.output(23, True);
			time.sleep(2);
			GPIO.output(23, False);
			alarm = 0
		else:
			time.sleep(0.2)

tled = threading.Thread(target = flash)  # LED thread to flash the LED while establishing connection
tled.start()

# Setup email sending
m = smtplib.SMTP('smtp.gmail.com', 587)  # Email protocol
m.ehlo()  
m.starttls()  
m.ehlo()  
m.login(user = 'alarmfromrpi@gmail.com', password = 'dave2159')  # User id & password

# Setup socket connection
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create a socket server
s.bind((HOST, PORT))
print("Waiting for connection...")
s.listen(1)
conn, addr = s.accept()
print('Connected by', addr)

fl = 0
tled.join()  # Terminate LED thread after connection is established
GPIO.output(18, True);  # Keep the LED on while the system is working

# Setup all the threads for the system
GPIO.add_event_detect(4,GPIO.FALLING, callback=active, bouncetime=300)  # Create a thread to detect interrupt input from the button
talarm = threading.Thread(target = alarm)  # Create a thread to control the buzzer
talarm.start()
waitforpics(conn)  # Start waiting for notification from STM32 

