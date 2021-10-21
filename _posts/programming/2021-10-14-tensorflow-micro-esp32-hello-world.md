---
layout: post
title: ESP32 Tensorflow Micro Hello World
category: programming
description: hello world for tensorflow micro esp32 
---

I'm starting learing about TinyML a.k.a "Machine Learning for Embedded Systems", and I'm logging my journey. This post is about how to setup your toolchain, and deploy the "Hello World" on the [ESP-EYE](https://www.mouser.de/ProductDetail/Espressif-Systems/ESP-EYE?qs=l7cgNqFNU1iWrlpTZmwCRA==). 

Btw, I used Linux Terminal throughout the post. But I think you can easily this everything under Windows and macOS. 

* 
{:toc}


# Install ESP-IDF Toolchain
Install IDF Toolchain unter Linux using docker. If you need to know how to install docker, read [this]({% post_url programming/2019-03-19-Docker %}). 

```sh
docker pull espressif/idf
```

That's all, but there is the option to use different versions. 
To download a specific docker image version or "tag", specify it as `espressif/idf:TAG`, for example:

```sh
docker pull espressif/idf:release-v4.0
```

For more detailed info about ESP-IDF toolchain, you can read the [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/index.html).  
And if you want to dig more into IDF Docker you can follow this [link](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/tools/idf-docker-image.html#).


# Download Tensorflow Micro

clone the repository: https://github.com/espressif/tensorflow/

```sh
git clone https://github.com/espressif/tensorflow/
```

# Hello World
## Compile Hello World
Generate the "Hello World" example with the following command: 

```sh
make -f tensorflow/lite/micro/tools/make/Makefile TARGET=esp generate_hello_world_esp_project
```

Go the the example project directory

```sh
cd tensorflow/lite/micro/tools/make/gen/esp_xtensa-esp32/prj/hello_world/esp-idf
```

Then build with idf.py

```sh
docker run --rm -v $PWD:/project -w /project espressif/idf idf.py build
```

## Run Hello World

To flash your device (replace /dev/ttyUSB0 with the device serial port):

```sh
docker run --rm -v $PWD:/project -w /project --device=/dev/ttyUSB0 espressif/idf idf.py flash -p /dev/ttyUSB0
```

# Workflow Inside A Docker Container

Sometimes is useful to work directly from Docker, for example for debugging purposes. 

Let's get inside the container:
```sh
docker run --rm -v $PWD:/project -w /project --device=/dev/ttyUSB0 -it espressif/idf
```

Now that you're inside the container, you can run IDF commands directly:
```sh
idf.py build
idf.py flash -p /dev/ttyUSB0
```

If you want to see the debug output using the serial port. **Note**: you need use docker with `--device=/dev/ttyUSB0` to be able to use monitor the serial port.

```sh
idf.py monitor
```

Use Ctrl+] to exit.

You could use combine the following commands:

```sh
idf.py --port /dev/ttyUSB0 flash monitor
```

You can find More info about [IDF Monitor here](https://demo-dijiudu.readthedocs.io/en/latest/get-started/idf-monitor.html).



# Building more examples

You can more examples if you check the _README_ files under `tensorflow/lite/micro/examples/<EXAMPLE_NAME>/README.md`.
These are the following examples available at the moment:

```sh
$  tree -L 1 tensorflow/lite/micro/examples/
tensorflow/lite/micro/examples/
├── doorbell_camera
├── hello_world
├── image_recognition_experimental
├── magic_wand
├── micro_speech
├── network_tester
├── person_detection
└── person_detection_experimental

8 directories, 0 files
```

This file `tensorflow/lite/micro/examples/hello_world/README.md`, for instance, describes how to configure and build **hello_world** example. 


# Conclusion
TinyML is a new and growing field, for that reasing it's still evolving but that means that there is a lot room for improving, and most important. TinyML enables a new set of real-world applications. 

I hope you enjoy this introduction. Until next time ;)
