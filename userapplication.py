import pygame
import time 
import os

pygame.init()

display_width = 800
display_height = 600
 
black = (0,0,0)
white = (255,255,255)
red = (200,0,0)
green = (0,200,0)
blue = (0,0,200)
bright_red = (255,0,0)
bright_green = (0,255,0)
bright_blue = (0,0,255)
 
block_color = (53,115,255)

gameDisplay = pygame.display.set_mode((display_width,display_height))
pygame.display.set_caption('Anti-Thief System')
clock = pygame.time.Clock()
pagecount = 0
page = 0
but = 0

def text_objects(text, font):
    textSurface = font.render(text, True, black)
    return textSurface, textSurface.get_rect()

def button(msg,x,y,w,h,ic,ac,action=None):
    mouse = pygame.mouse.get_pos()
    click = pygame.mouse.get_pressed()

    if x+w > mouse[0] > x and y+h > mouse[1] > y:
        pygame.draw.rect(gameDisplay, ac,(x,y,w,h))
        if click[0] == 1 and action != None:
            action() 
    else:
        pygame.draw.rect(gameDisplay, ic,(x,y,w,h))

    smallText = pygame.font.Font("freesansbold.ttf",20)
    textSurf, textRect = text_objects(msg, smallText)
    textRect.center = ( (x+(w/2)), (y+(h/2)) )
    gameDisplay.blit(textSurf, textRect)

def filetime(file):
    return time.ctime(os.path.getmtime(file))

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
    print("pgc", pagecount)
    return image

def nextpage():
    global but
    global page
    if but == 0:
        page = page+1
        picturepage()
    but = 1

def previouspage():
    global page
    global but
    if but == 0:
        page = page-1
        picturepage()
    but = 1


def startingpage():

    display1=True

    while display1:

        for event in pygame.event.get():
            print(event)
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()

        gameDisplay.fill(white)
        largeText = pygame.font.Font('freesansbold.ttf',70)
        TextSurf, TextRect = text_objects("Anti-Thief System", largeText)
        TextRect.center = ((display_width/2),(display_height/2)-100)
        gameDisplay.blit(TextSurf, TextRect)

        button('ACTIVATE',100,350,200,50,green,bright_green)
        button('INACTIVATE',500,350,200,50,red,bright_red)
        button('RECORDED PICTURES',200,450,400,50,blue,bright_blue,picturepage)


        pygame.display.update()
        clock.tick(15)

def picturepage():
    global but
    global image
    display2=True
    but = 0

    while display2:

        for event in pygame.event.get():
            print(event)
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()

        gameDisplay.fill(white)
        smallText = pygame.font.Font('freesansbold.ttf',20)
        TextSurf, TextRect = text_objects("Recorded Pictures", smallText)
        TextRect.center = ((display_width/2),(display_height/2)-250)
        gameDisplay.blit(TextSurf, TextRect)  

        button('MAIN PAGE',200,500,400,50,blue,bright_blue,startingpage)
        
        file="images/%d.jpg" % (page*2+1)
        gameDisplay.blit(image[page*2],(50,70))
        TextSurf, TextRect = text_objects(file, smallText)
        TextRect.center = (210,410)
        gameDisplay.blit(TextSurf, TextRect)

        print(page)

        if page!=0:
            button('<',50,430,100,50,green,bright_green,previouspage)
        if page!=pagecount-1:
            button('>',650,430,100,50,green,bright_green,nextpage)

        try:
            file="images/%d.jpg" % (page*2+2)
            gameDisplay.blit(image[page*2+1],(430,70))
            TextSurf, TextRect = text_objects(file, smallText)
            TextRect.center = (590,410)
            gameDisplay.blit(TextSurf, TextRect)
        except:
            print("HI")

        pygame.display.update()
        clock.tick(15) 
    
image=loadphoto()
picturepage()