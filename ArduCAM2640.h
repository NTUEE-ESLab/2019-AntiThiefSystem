/*
  ArduCAM.h - Arduino library support for CMOS Image Sensor
  Copyright (C)2011-2015 ArduCAM.com. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ArduCAM.com. You can find the latest version of the library at
  http://www.ArduCAM.com

  Now supported controllers:
        -   OV7670
        -   MT9D111
        -   OV7675
        -   OV2640
        -   OV3640
        -   OV5642
        -   OV7660
        -   OV7725
        - MT9M112       
        - MT9V111
        - OV5640        
        - MT9M001           
                
    We will add support for many other sensors in next release.
    
  Supported MCU platform
        -   Theoretically support all Arduino families
        -   Arduino UNO R3          (Tested)
        -   Arduino MEGA2560 R3     (Tested)
        -   Arduino Leonardo R3     (Tested)
        -   Arduino Nano            (Tested)
        -   Arduino DUE             (Tested)
        - Arduino Yun               (Tested)        
        -   Raspberry Pi            (Tested)
        

  If you make any modifications or improvements to the code, I would appreciate
  that you share the code with me so that I might include it in the next release.
  I can be contacted through http://www.ArduCAM.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*------------------------------------
    Revision History:
    2012/09/20  V1.0.0  by Lee  first release 
    2012/10/23  V1.0.1  by Lee  Resolved some timing issue for the Read/Write Register  
    2012/11/29  V1.1.0  by Lee  Add support for MT9D111 sensor
    2012/12/13  V1.2.0  by Lee  Add support for OV7675 sensor
    2012/12/28  V1.3.0  by Lee  Add support for OV2640,OV3640,OV5642 sensors
    2013/02/26  V2.0.0  by Lee  New Rev.B shield hardware, add support for FIFO control 
                                                            and support Mega1280/2560 boards 
    2013/05/28  V2.1.0  by Lee  Add support all drawing functions derived from UTFT library             
    2013/08/24  V3.0.0  by Lee  Support ArudCAM shield Rev.C hardware, features SPI interface and low power mode.
                                Support almost all series of Arduino boards including DUE.  
    2014/03/09  V3.1.0  by Lee  Add the more impressive example sketches. 
                                Optimise the OV5642 settings, improve image quality.
                                Add live preview before JPEG capture.
                                Add play back photos one by one after BMP capture.
    2014/05/01  V3.1.1  by Lee  Minor changes to add support Arduino IDE for linux distributions.   
    2014/09/30  V3.2.0  by Lee  Improvement on OV5642 camera dirver.            
    2014/10/06  V3.3.0  by Lee  Add OV7660,OV7725 camera support.           
    2015/02/27  V3.4.0  by Lee  Add the support for Arduino Yun board, update the latest UTFT library for ArduCAM.          
    2015/06/09  V3.4.1  by Lee  Minor changes and add some comments     
    2015/06/19  V3.4.2  by Lee  Add support for MT9M112 camera.         
    2015/06/20  V3.4.3  by Lee  Add support for MT9V111 camera.         
    2015/06/22  V3.4.4  by Lee  Add support for OV5640 camera.                                      
    2015/06/22  V3.4.5  by Lee  Add support for MT9M001 camera.                                                             
--------------------------------------*/

/*
Parts of this software is related to https://os.mbed.com/users/dflet/ work.
*/


#pragma once

#include <mbed.h>
#include "arducam_regs.h"

/****************************************************/
/* Sensor related definition                                                */
/****************************************************/
/****************************************************/
/* ArduChip related definition                                          */
/****************************************************/
#define RWBIT                   0x80  //READ AND WRITE BIT IS BIT[7]

#define ARDUCHIP_TEST1          0x00  //TEST register
#define ARDUCHIP_TEST2          0x01  //TEST register

#define ARDUCHIP_FRAMES         0x01  //Bit[2:0]Number of frames to be captured

#define ARDUCHIP_MODE           0x02  //Mode register
#define MCU2LCD_MODE            0x00
#define CAM2LCD_MODE            0x01
#define LCD2MCU_MODE            0x02

#define ARDUCHIP_TIM            0x03  //Timming control
#define HREF_LEVEL_MASK         0x01  //0 = High active ,       1 = Low active
#define VSYNC_LEVEL_MASK        0x02  //0 = High active ,       1 = Low active
#define LCD_BKEN_MASK           0x04  //0 = Enable,             1 = Disable
#define DELAY_MASK              0x08  //0 = no delay,           1 = delay one clock
#define MODE_MASK               0x10  //0 = LCD mode,           1 = FIFO mode
#define FIFO_PWRDN_MASK         0x20  //0 = Normal operation,   1 = FIFO power down
#define LOW_POWER_MODE          0x40  //0 = Normal mode,        1 = Low power mode

#define ARDUCHIP_FIFO           0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK         0x01
#define FIFO_START_MASK         0x02
#define FIFO_RDPTR_RST_MASK     0x10
#define FIFO_WRPTR_RST_MASK     0x20

#define ARDUCHIP_GPIO           0x06  //GPIO Write Register
#define GPIO_RESET_MASK         0x01  //0 = default state,      1 =  Sensor reset IO value
#define GPIO_PWDN_MASK          0x02  //0 = Sensor power down IO value, 1 = Sensor power enable IO value

#define BURST_FIFO_READ         0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ        0x3D  //Single FIFO read operation

#define ARDUCHIP_REV            0x40  //ArduCHIP revision
#define VER_LOW_MASK            0x3F
#define VER_HIGH_MASK           0xC0

#define ARDUCHIP_TRIG           0x41  //Trigger source
#define VSYNC_MASK              0x01
#define SHUTTER_MASK            0x02
#define CAP_DONE_MASK           0x08

#define FIFO_SIZE1              0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2              0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3              0x44  //Camera write FIFO size[18:16]

#define OV2640_CHIPID_HIGH  0x0A
#define OV2640_CHIPID_LOW   0x0B
#define OV2640_CHIPID_HIGH_VALUE 0x26
#define OV2640_CHIPID_LOW_VALUE 0x42

/****************************************************/


#define OV_RESOLUTION_VGA 1
#define OV_RESOLUTION_QVGA 2
#define OV_RESOLUTION_CIF 3
#define OV_RESOLUTION_QQVGA 4

#define OV2640_I2C_ADDR_W 0x0060
#define OV2640_I2C_ADDR_R 0x0061

class ArduCAM2640
{
public:

    // ctor dtor
    ArduCAM2640(){};
    ~ArduCAM2640(){};

    // setup
    bool Setup(int resolution,int jpeg_quality,DigitalOut *cam_cs, SPI *cam_spi, I2C *cam_i2c)
    {
        uint8_t vid = 0;
        uint8_t pid = 0;
        const struct sensor_reg8 *pivot;

        _cam_cs  = cam_cs;
        _cam_spi = cam_spi;
        _cam_i2c = cam_i2c;

        _cam_spi->format(8,0);
        _cam_spi->frequency(8000000);
        _cam_i2c->frequency(100000);
        *_cam_cs = 1;

        /* check spi i2c */
        wrSensorReg8_8(0xFF,0x01);
        rdSensorReg8_8(OV2640_CHIPID_HIGH,&vid);
        rdSensorReg8_8(OV2640_CHIPID_LOW,&pid);
        if ((vid != OV2640_CHIPID_HIGH_VALUE) || (pid != OV2640_CHIPID_LOW_VALUE))
        {
            printf("Can't find OV2640 module! vid = 0x%x pid = 0x%x\r\n", vid, pid);
            return false;
        }

        /* setup resolution and jpeg quality */
        wrSensorReg8_8(0xFF,0x01);
        wrSensorReg8_8(0x12,0x80);
        wait_ms(200);

        pivot = &OV2640_JPEG_INIT[0];
        while(1)
        {
            if(pivot->reg == 0xff && pivot->val == 0xff) break;
            // quality register
            if(pivot->reg == 0x44)
            {
                wrSensorReg8_8(pivot->reg,(uint8_t)100-jpeg_quality);
            }
            else
                wrSensorReg8_8(pivot->reg,pivot->val);
            pivot++;
        }

        pivot = &OV2640_YUV422[0];
        while(1)
        {
            if(pivot->reg == 0xff && pivot->val == 0xff) break;
            wrSensorReg8_8(pivot->reg,pivot->val);
            pivot++;
        }

        pivot = &OV2640_JPEG[0];
        while(1)
        {
            if(pivot->reg == 0xff && pivot->val == 0xff) break;
            wrSensorReg8_8(pivot->reg,pivot->val);
            pivot++;
        }

        wrSensorReg8_8(0xFF,0x01);
        wrSensorReg8_8(0x15,0x00);

        switch(resolution)
        {
        case OV_RESOLUTION_VGA:
            pivot = &OV2640_640x480_JPEG[0];
            break;
        case OV_RESOLUTION_QVGA:
            pivot = &OV2640_320x240_JPEG[0];
            break;
        case OV_RESOLUTION_QQVGA:
            pivot = &OV2640_160x120_JPEG[0];
            break;
        case OV_RESOLUTION_CIF:
            pivot = &OV2640_352x288_JPEG[0];
            break;
        default:
            printf("Wrong resolution\r\n");
            return false;
        };

        while(1)
        {
            if(pivot->reg == 0xff && pivot->val == 0xff) break;
            wrSensorReg8_8(pivot->reg,pivot->val);
            pivot++;
        }

        wait_ms(2000);

        return true;
    };

    uint32_t CaptureImage(uint8_t *image_buffer, uint32_t image_buffer_size,uint32_t *jpeg_start_index)
    {
        uint32_t fifolen = 0;
        uint32_t jpeg_start,jpeg_end;

            flush_fifo();
            start_capture();
            while (!(get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)))
            {
                wait_ms(30);
            }

            fifolen = read_fifo_length() + 2;
            if(fifolen>image_buffer_size) 
            {
                printf("Arducam::CaptureImage fifo_len %d exceeds image_buffer_size %d\r\n",fifolen,image_buffer_size);
                return 0;
            }
            printf("fifo len %d\r\n",fifolen);

            image_buffer[0]=BURST_FIFO_READ;
            *_cam_cs = 0; 
            _cam_spi->write((char*)image_buffer,fifolen,(char*)image_buffer,fifolen);
            *_cam_cs = 1;

            jpeg_start = image_buffer_size;
            jpeg_end = 0;
            for(int i=0;i<fifolen-1;i++)
            {
                if(image_buffer[i] == 0xFF)
                {
                    if(image_buffer[i+1] == 0xD8 && jpeg_start == image_buffer_size)
                    {
                        jpeg_start = i;
                    }
                    else if(image_buffer[i+1] == 0xD9 && jpeg_end == 0)
                    {
                        jpeg_end = i+1;
                        break;
                    }
                }
            }

            if( jpeg_start != image_buffer_size && jpeg_end != 0 )
            {
                *jpeg_start_index = jpeg_start;
                return (jpeg_end - jpeg_start + 1);
            }   

            return 0;
    };

private:
    DigitalOut          *_cam_cs;
    SPI                 *_cam_spi;  
    I2C                 *_cam_i2c;

    //Low level SPI write operation
    int bus_write(int address, int value) 
    {
        // take the SS pin low to select the chip:
        *_cam_cs = 0;
        //  send in the address and value via SPI:
        _cam_spi->write(address);
        _cam_spi->write(value);
        // take the SS pin high to de-select the chip:
        *_cam_cs = 1;
        
        return 0;
    };

    //Low level SPI read operation
    uint8_t bus_read(int address) 
    {
        uint8_t value = 0;
        // take the SS pin low to select the chip:
        *_cam_cs = 0;
        //  send in the address and value via SPI:
        //  printf("addr = 0x%x\r\n",address);
        _cam_spi->write(address);
        value = _cam_spi->write(0x00);
        // take the SS pin high to de-select the chip:
        *_cam_cs = 1;
        return value;
    };

    //Write ArduChip internal registers
    void write_reg(uint8_t addr, uint8_t data)
    {
        bus_write(addr | 0x80, data);
    };

    //Read ArduChip internal registers
    uint8_t read_reg(uint8_t addr)
    {
        uint8_t data;
        data = bus_read(addr);
        return data;
    };

    //Reset the FIFO pointer to ZERO        
    void flush_fifo(void)
    {
        write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
    };

    //Send capture command
    void start_capture(void)
    {
        write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
    };

    //Clear FIFO Complete flag
    void clear_fifo_flag(void)
    {
        write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
    };


    //Read Write FIFO length
    //Support ArduCAM Mini only
    uint32_t read_fifo_length(void)
    {
        uint32_t len1,len2,len3=0;
        uint32_t length=0;
        len1 = read_reg(FIFO_SIZE1);    
        len2 = read_reg(FIFO_SIZE2);    
        len3 = read_reg(FIFO_SIZE3) & 0x07;
        length = ((len3 << 16) | (len2 << 8) | len1);// & 0x07ffff;
        return length;
    };

    //I2C Write 8bit address, 8bit data
    uint8_t wrSensorReg8_8(int regID, int regDat)
    {
        uint8_t buff[2];
        buff[0] = regID;
        buff[1] = regDat;
        int c = _cam_i2c->write(OV2640_I2C_ADDR_W,(char*)buff,2);
        return(1);
    };

    //I2C Read 8bit address, 8bit data
    uint8_t rdSensorReg8_8(uint8_t regID, uint8_t* regDat)
    {
        _cam_i2c->write(OV2640_I2C_ADDR_W,(char*)&regID,1);
        _cam_i2c->read(OV2640_I2C_ADDR_R,(char*)regDat,1);
        return(1);
    };

    //Get corresponding bit status
    uint8_t get_bit(uint8_t addr, uint8_t bit)
    {
        uint8_t temp;
        temp = read_reg(addr);
        temp = temp & bit;
        return temp;
    };

};

