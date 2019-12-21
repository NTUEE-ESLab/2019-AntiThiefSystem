/* WiFi Example
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#include "mbed.h"
#include "TCPSocket.h"
#include "TCPServer.h"
#include "stm32l475e_iot01_accelero.h"
#include "VL53L0X.h"
#include "ArduCAM2640.h"
ArduCAM2640 g_arducam;
/* jpeg buffer */
#define IMAGE_JPG_SIZE 1024*16
uint8_t g_image_buffer[IMAGE_JPG_SIZE];
/* Setup camera */
I2C camI2C(PB_9,PB_8);
SPI camSPI(PA_7,PA_6,PA_5);
DigitalOut cam_spi_cs(PA_2);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalInOut led3(LED3);
TCPSocket socket;
SocketAddress addr("172.20.10.2",12035);
nsapi_error_t response;


Semaphore sem(0);
Thread thread;
Thread data_thread;
Thread led_1;
Thread led_2;
static DevI2C devI2c(PB_11,PB_10);
static DigitalOut shutdown_pin(PC_6_ALT0);
VL53L0X vl53(&devI2c, &shutdown_pin, PC_7);

#define WIFI_IDW0XX1    2

#if (defined(TARGET_DISCO_L475VG_IOT01A) || defined(TARGET_DISCO_F413ZH))
#include "ISM43362Interface.h"
ISM43362Interface wifi(false);

#else // External WiFi modules

#if MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1
#include "SpwfSAInterface.h"
SpwfSAInterface wifi(MBED_CONF_APP_WIFI_TX, MBED_CONF_APP_WIFI_RX);
#endif // MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1

#endif

#define SCALE_MULTIPLIER    0.004


const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}



void led_thread(){
    while(true){
        led1 = !led1;
        wait(0.1);
    }
}

void led2_thread(){
    while(true){
        led2 = !led2;
        wait(0.1);
    }
}

void send_pic(NetworkInterface *net)
{
    while(true){

        sem.wait();
        led3.output();
        led3 = 1;
        char size[64];

        socket.set_blocking(1);
        
        uint32_t image_size;
        uint32_t idx;
                        
        image_size = g_arducam.CaptureImage(g_image_buffer,IMAGE_JPG_SIZE,&idx);

        if( image_size == 0 )
        {
            printf("Photo failure %d\r\n",image_size);
        }
        int len = sprintf(size,"%d", image_size);
        response = socket.send(size, len);
        if (0 >= response){
            printf("Error seding: %d\n", response);
        }
        response = socket.send(g_image_buffer+idx, image_size);
        if (0 >= response){
            printf("Error seding: %d\n", response);
        }
        printf("sendpic\n");
        led3.input();
        
        sem.release();
    }
}

void vl53thread(){
    led2 = 1;
    vl53.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    uint32_t distance;
    uint32_t lastdistance;
    int i = 0;
    float delta;
    int status;
    while (true){
        lastdistance = distance;
        status = vl53.get_distance(&distance);
        delta = (float)((float)distance - lastdistance)/lastdistance;
        if (status == VL53L0X_ERROR_NONE) {
            led2 = 1;
            if(delta < -0.07 && i > 10){
                printf("Notify RPi: delta = %f\r\n", delta);
                sem.release();
                sem.wait();
                wait(2);
            }
            if(i < 11){
                i++;
            }
        } else {
            led2 = !led2;
            wait(0.1);

        }
    }
}


int main()
{
    //Connect to wifi
    led_1.start(led_thread);
    while (true){
        printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
        int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA2);
        if (ret != 0) {
            printf("\nConnection error\n");
        }
        else{
            printf("Success\n\n");
            printf("MAC: %s\n", wifi.get_mac_address());
            printf("IP: %s\n", wifi.get_ip_address());
            printf("Netmask: %s\n", wifi.get_netmask());
            printf("Gateway: %s\n", wifi.get_gateway());
            printf("RSSI: %d\n\n", wifi.get_rssi());
            break;
        }
    }

    //Start socket
    printf("Start server\n");
    response = socket.open(&wifi);
    if (0 != response){
        printf("Error opening: %d\n", response);
    }
    while (true){
        response = socket.connect(addr);
        if (0 != response){
            printf("Error connecting: %d\n", response);
        }
        else{
            break;
        }
        wait(0.1);
    }
    led_1.terminate();
    led1 = 0;

    //Setup arducam
    if( g_arducam.Setup( OV_RESOLUTION_CIF,92, &cam_spi_cs, &camSPI, &camI2C) == false)
    {
        printf("Arducam setup error \r\n");
        while(1);
    } 

    //Start main task
    thread.start(vl53thread);
    data_thread.start(callback(send_pic, &wifi));

    while(1){
        wait(100);
    }
    return 0;

}
