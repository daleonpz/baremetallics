


# Install ESP-IDF Toolchain
Install IDF Toolchain unter Linux using docker: 

```sh
docker pull espressif/idf
```

To download a specific docker image tag, specify it as `espressif/idf:TAG`, for example:

```sh
docker pull espressif/idf:release-v4.0
```

More info about [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/index.html)
More info about how to use [IDF Docker](jhttps://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/tools/idf-docker-image.html#)


# Download Tensorflow Micro

Copy this [guide](https://blog.tensorflow.org/2020/08/announcing-tensorflow-lite-micro-esp32.html)


to build your project

```sh
docker run --rm -v $PWD:/project -w /project espressif/idf idf.py build

docker run --rm -v $PWD:/project -w /project -it espressif/idf

idf.py menuconfig 
idf.py build
```
to flash ESP-EYE

```sh
docker run --rm -v $PWD:/project -w /project --device=/dev/ttyUSB0 espressif/idf idf.py flash -p /dev/ttyUSB0
```

to debug 
```sh
docker run --rm -v $PWD:/project -w /project --device=/dev/ttyUSB0 -it espressif/idf
idf.py flash -p /dev/ttyUSB0
idf.py monitor
```

Ctrl-] will exit the monitor

More info about [IDF Monitor](https://demo-dijiudu.readthedocs.io/en/latest/get-started/idf-monitor.html)



# Building more examples

read `tensorflow/lite/micro/examples/<EXAMPLE_NAME>/README.md`
 

In case there are the following examples:
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

