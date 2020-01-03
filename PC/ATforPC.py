import pygame
import time 
import os
import socket
import threading
from pynput.keyboard import Key, Controller

# Setup pygame environment
pygame.init()
display_width = 800
display_height = 600
black = (0,0,0)
white = (255,255,255)
red = (200,0,0)
orange = (255,85,17)
green = (0,200,0)
blue = (0,0,200)
bright_red = (255,0,0)
bright_orange = (255,119,68)
bright_green = (0,255,0)
bright_blue = (0,0,255)
block_color = (53,115,255)
gameDisplay = pygame.display.set_mode((display_width,display_height))
pygame.display.set_caption('Anti-Thief System')
clock = pygame.time.Clock()
keyboard = Controller()


mutex = threading.Lock()  # Mutex for access of photos
HOST = '172.20.10.2'  # Standard loopback interface address
PORT = 12035          # Port to listen on
pic = 1               # Name for the picture to be saved
alarmon = 1           # Control the on/off of the system
alarm = 0             # Tell the alarm thread to start alarmpage
swindow = 1           # Control the on/off of the switching window function
pagecount = 0         # The number of the pages according to the number of the pictures
page = 0              # Current page
buttondown = 0        # Record if the button is pressed
connecting = 1        # Control the displaying of connecting page

# An object of black text
def text_objects(text, font):
	textSurface = font.render(text, True, black)
	return textSurface, textSurface.get_rect()

# An object of button with text
def button(msg,x,y,w,h,ic,ac,action=None):
	mouse = pygame.mouse.get_pos()
	click = pygame.mouse.get_pressed()
	global buttondown

	if x+w > mouse[0] > x and y+h > mouse[1] > y:
		pygame.draw.rect(gameDisplay, ac,(x,y,w,h))
		if buttondown == 1 and action != None:
			buttondown=0
			action() 
	else:
		pygame.draw.rect(gameDisplay, ic,(x,y,w,h))

	smallText = pygame.font.SysFont("arial",20)
	textSurf, textRect = text_objects(msg, smallText)
	textRect.center = ( (x+(w/2)), (y+(h/2)) )
	gameDisplay.blit(textSurf, textRect)

# Turn on the system
def active():
	global alarmon 
	alarmon = 1

# Turn off the system
def inactive():
	global alarmon
	alarmon = 0

# Get the last changed time of the file
def filetime(file):
	return time.ctime(os.path.getmtime(file))

# Convert bytes to an integer
def bytes_to_int(bytes):
	result = 0
	for b in bytes:
		result = result * 10 + int(b-48)
	return result

# Keep waiting for notifications and pictures from STM32
def waitforpics(conn):
	global pic
	global alarm
	while True:
		num = bytes_to_int(conn.recv(1024))
		print(num)
		if swindow == 1:
				keyboard.press(Key.alt)
				keyboard.press(Key.tab)
				keyboard.release(Key.tab)
				keyboard.release(Key.alt)
		mutex.acquire()
		if alarmon == 1:
			alarm = 1
			file = "images/%d.jpg" % (pic)
			fo = open(file, "wb+")
			while num!=0:
				data = conn.recv(16384)
				fo.seek(0, 2)
				fo.write(data)
				num = num - len(data)
			fo.close()
			print(filetime(file), file, "output done")
			pic = pic + 1
		else:
			while num!=0:
				data = conn.recv(16384)
				num = num - len(data)
		mutex.release()

# Clear all saved photos
def clear_photo():
	global page
	global pic
	i = 1
	mutex.acquire()
	while True:
		try:
			file = "images/%d.jpg" % (i)
			os.remove(file)
			i = i + 1
		except:
			break
	page = 0
	pic = 1
	mutex.release()

# Load all the saved photos
def loadphoto():
	global pagecount 
	i = 0
	image = []
	while True:
		try:
			file = "images/%d.jpg" % (i+1)
			image.append(pygame.image.load(file))
			image[i].convert()
			image[i] = pygame.transform.scale(image[i], (320, 320))
			i = i + 1
		except:
			break
	pagecount = int((i+1)/2)
	return image

# Load the alarm image
def loadalarmimage():
	file = "alarmimage/%d.jpg" % (0)
	alarmimage=pygame.image.load(file)
	alarmimage = pygame.transform.scale(alarmimage, (320, 320))
	return alarmimage

# Get to the next page
def nextpage():
	global page
	page = page+1
	picturepage()

# Get to the previous page
def previouspage():
	global page
	page = page-1
	picturepage()

# Turn on/off the switch window function
def switch():
	global swindow
	if swindow == 1:
		swindow = 0
	else:
		swindow = 1

# The main page of user interface
def startingpage():
	global buttondown
	global alarm
	display1=True

	while display1:
		buttondown=0
		if alarm == 1:
			alarmpage()
			alarm = 0
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
				quit()
			if event.type == pygame.MOUSEBUTTONDOWN:
				buttondown=1
			
		gameDisplay.fill(white)
		largeText = pygame.font.SysFont("arial",70)
		TextSurf, TextRect = text_objects("Anti-Thief System", largeText)
		TextRect.center = ((display_width/2),(display_height/2)-100)
		gameDisplay.blit(TextSurf, TextRect)

		button('ACTIVATE',100,370,200,50,green,bright_green,active)
		button('INACTIVATE',500,370,200,50,red,bright_red,inactive)
		if swindow == 1:
			swst = 'DISABLE SWITCH WINDOW' 
		else:
			swst = 'ENABLE SWITCH WINDOW'
		button(swst,260,290,280,50,orange,bright_orange,switch)
		button('RECORDED PICTURES',200,450,400,50,blue,bright_blue,picturepage)

		pygame.display.update()
		clock.tick(15)

# The page to display all the saved pictures
def picturepage():
	global but
	global buttondown
	global alarm

	display2=True
	while display2:
		if alarm == 1:
			alarmpage()
			alarm = 0
		image=loadphoto()
		buttondown=0
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
				quit()
			if event.type == pygame.MOUSEBUTTONDOWN:
				buttondown=1

		gameDisplay.fill(white)
		smallText = pygame.font.SysFont("arial",20)
		TextSurf, TextRect = text_objects("Recorded Pictures", smallText)
		TextRect.center = ((display_width/2),(display_height/2)-250)
		gameDisplay.blit(TextSurf, TextRect)  

		button('MAIN PAGE',200,500,400,50,blue,bright_blue,startingpage)
		
		try:
			file="images/%d.jpg" % (page*2+1)
			gameDisplay.blit(image[page*2],(50,70))
			TextSurf, TextRect = text_objects(filetime(file), smallText)
			TextRect.center = (210,410)
			gameDisplay.blit(TextSurf, TextRect)
		except:
			pass

		if page!=0:
			button('<',50,430,100,50,green,bright_green,previouspage)
		if pagecount!=0 and page!=pagecount-1:
			button('>',650,430,100,50,green,bright_green,nextpage)

		try:
			file="images/%d.jpg" % (page*2+2)
			gameDisplay.blit(image[page*2+1],(430,70))
			TextSurf, TextRect = text_objects(filetime(file), smallText)
			TextRect.center = (590,410)
			gameDisplay.blit(TextSurf, TextRect)
		except:
			pass
		if pagecount!=0:
			button('Clear',20,10,150,50,red,bright_red,clear_photo)

		pygame.display.update()
		clock.tick(15) 

# The page to be showed when alarm is triggered
def alarmpage():
	global alarmimage
	display3=True
	alarmsound = pygame.mixer.music.load("alarmimage/alarm.wav")
	pygame.mixer.music.play(-1)

	while display3:
		pressed = pygame.key.get_pressed()
		return_held = pressed[pygame.K_RETURN]
		buttondown=0
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
				quit()
			if event.type == pygame.KEYDOWN:
				if event.key == pygame.K_RETURN:
					pygame.mixer.music.stop()
					return

		gameDisplay.fill(white)
		largeText = pygame.font.SysFont("arial",70)
		TextSurf, TextRect = text_objects("Intruder Alert!", largeText)
		TextRect.center = ((display_width/2),(display_height/2)+200)
		gameDisplay.blit(TextSurf, TextRect)

		gameDisplay.blit(alarmimage,((display_width/2)-160,(display_height/2)-210))
		smallText = pygame.font.SysFont("arial",20)

		pygame.display.update()
		clock.tick(15) 

# An object of white text
def text_objectswhite(text, font):
	textSurface = font.render(text, True, white)
	return textSurface, textSurface.get_rect()

# The page to be showed while connecting
def connectingpage():
	display4=True
	counter=0

	while display4:
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
				quit()
		if connecting == 0:
			return

		gameDisplay.fill(black)

		if counter<10:
			largeText = pygame.font.Font('freesansbold.ttf',70)
			TextSurf, TextRect = text_objectswhite("Connecting", largeText)
			TextRect.center = ((display_width/2),(display_height/2))
			gameDisplay.blit(TextSurf, TextRect)

		
		if counter>=10 and counter<20:
			TextSurf, TextRect = text_objectswhite("Connecting.", largeText)
			TextRect.center = ((display_width/2),(display_height/2))
			gameDisplay.blit(TextSurf, TextRect)

		
		if counter>=20 and counter<30:
			TextSurf, TextRect = text_objectswhite("Connecting..", largeText)
			TextRect.center = ((display_width/2),(display_height/2))
			gameDisplay.blit(TextSurf, TextRect)

		
		if counter>=30 and counter<40:
			TextSurf, TextRect = text_objectswhite("Connecting...", largeText)
			TextRect.center = ((display_width/2),(display_height/2))
			gameDisplay.blit(TextSurf, TextRect)

		if counter==39:
			counter=0

		counter=counter+1

		pygame.display.update()
		clock.tick(15) 


alarmimage = loadalarmimage()                                 # Load the alarm image
tc = threading.Thread(target = connectingpage)                # Create a thread to show connecting page
tc.start()

# Setup socket connection
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
print("Waiting for connection...")
s.listen()
conn, addr = s.accept()
print('Connected by', addr)

tw = threading.Thread(target = waitforpics, args = (conn, ))  # Create a thread to wait for notifications and pictures from STM32
tw.start()

connecting = 0                                                # Stop the connecting page
startingpage()                                                # Start the main page of the user interface 
