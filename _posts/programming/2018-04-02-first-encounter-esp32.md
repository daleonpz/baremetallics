---
layout: post
title: First encounter with ESP32
category: programming
tags: [esp32, debug]
---

I had some issues working with the ESP32. I will list all the issues I had and their solutions. For the record I use Manjaro 17.1.7 - 64bits.

# Toolchain related 
- ESP32 uses python scripts to configure, compile and flash the chip. If the scripts don't work it's most likely due to  python3 is the default application for the executable `python`. I should be python 2.7.

```sh
$ python --version
Python 2.7.14
```

- Fail to write to "serial port" `/dev/ttyUSB0` using python. The problem is the library `serial`. When importing the library in python 

```python
import serial
```

You should have installed `pyserial` instead of `serial`.

```sh
$ pip install pyserial
```

- Add your `$USER` to `uupc` group in order to have permission to write through `/dev/ttyUSB0`.  In arch linux:

```sh
$ usermod -a -G uucp $USER
```

# Library related
- Assign not enough stack memory to a task when using FreeRTOS. Check `configMINIMAL_STACK_SIZE` in the ESP32 documentation.

```c
// For ESP-IDF applications
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

BaseType_t xTaskCreate( 
        TaskFunction_t pvTaskCode, 
        const char * const pcName, 
        unsigned short usStackDepth, 
        void *pvParameters, 
        UBaseType_t uxPriority, 
        TaskHandle_t *pxCreatedTask 
    );
```

- Use `main` instead of `app_main`

```c
// Wrong
int main(){
    ...
}

// Correct
void app_main(){
    ...
}
```


-  Forget initialize the flash. Quoted from "Explore Embedded", _the ESP32 Modules run code from an external flash. Unless you're using directly the ESP32 chip, the module/board maker will taker of the flash initialization and access._ [link](https://exploreembedded.com/wiki/Hello_World_with_ESP32_Explained#RTOS)

```c
void app_main(){
    ESP_ERROR_CHECK( nvs_flash_init() );
    ...
}
````
 
- Use `vTaskStartScheduler();`.  `vTaskStartScheduler()` call is not required, as scheduler is already started before `app_main()` call

