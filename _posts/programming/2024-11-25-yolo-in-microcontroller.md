---
layout: post
title: Running YOLOv5 on a Microcontroller 
category: programming
description: Learn how to run YOLOv5 on a microcontroller. Optimize the model for performance and memory constraints to run object detection on edge devices.
---


Running YOLOv5 on a microcontroller is no small task, given the model's computational requirements. In this post, I’ll share how I successfully ran a simplified YOLOv5 model on an ESP32 microcontroller and optimized it for performance and memory constraints.

* 
{:toc}

# YOLOv5 on Embedded Devices: The Challenge

YOLOv5 from [Ultralytics](https://docs.ultralytics.com/) follows a Backbone, Neck, and Head architecture:

- The Backbone is responsible for extracting features from the input image.
- The Neck combines features from different levels of the backbone.
- The Head generates bounding boxes and class predictions based on the features extracted by the backbone and neck.

![YOLOv5 Architecture](/images/posts/yolov5-model.png)

A more detailed architecture can be found in the [YOLOv5 documentation](https://docs.ultralytics.com/yolov5/tutorials/architecture_description/#1-model-structure).

The model is quite complex and the smallest version from Ultralytics is a few MBs.
However, the ESP32 microcontroller has just 4MB of flash memory and 8MB of PSRAM, making it impossible to run the full model without optimizations.

# Use Case: Screw Type Detection

I focused on detecting screw types (e.g., "Big," "Medium," "Black") using an ESP-EYE module. The setup had fixed conditions:

- Images were 96x96 pixels.
- The camera and screws were at a fixed distance.

![Screws](/images/posts/yolov5-sampledata.png)

These constraints simplified the problem, making it easier to reduce the model size.

# How I Simplified the Model

- **Reduce Input Size**: Input images were resized to 96x96 pixels, reducing the parameters and memory usage.
- **Simplify the Backbone**: I reduced the number of layers and filters using trial and error, maintaining acceptable accuracy while cutting computational demands.
- **Simplify the Head**: Removed multi-scale predictions since the object sizes were fixed, reducing parameters drastically.
- **Quantize the Model**: Converted weights and activations to uint8, significantly reducing memory usage.

In the picture below, I marked the parts I removed from the original YOLOv5 model:

![YOLOv5 Simplified Architecture](/images/posts/yolov5-model-modified.png)

The simplified model had fewer layers and filters, making it suitable for the ESP32 microcontroller.

![YOLOv5 Simplified Architecture](/images/posts/yolov5-model-final.png)

**Result**: After these optimizations, the model shrank to a mere **23KB** with quantization:

```
 model_format       :   uint8
 params #           :   23,066 items (23.70 KiB)
 ----------------------------------------------------------
 input 1/1          :   27.00 KiB, (1,96,96,3)
 output 1/1         :   3.38 KiB, (1,432,8)
 macc               :   6,532,528
 weights (ro)       :   24,276 B (23.71 KiB)
 activations (rw)   :   36,864 B (36.00 KiB) (1 segment)
 ram (total)        :   67,968 B (66.38 KiB) = 36,864 + 27,648 + 3,456
```

You can reproduce these results using my [project repository](https://github.com/daleonpz/POC_CV_tinyml).

# Lessons Learned

- **Fixed Scenarios Simplify the Problem**: The controlled environment (fixed camera and object size) allowed me to remove unnecessary complexity.
- **Quantization is Key**: Converting the model to uint8 saved both memory and computational resources, critical for microcontrollers.
- **Iterative Testing Works**: Simplifying the backbone and head required experimentation, balancing size with accuracy.

# Conclusion

By reducing input size, simplifying the architecture, and quantizing the model, I was able to run YOLOv5 on a resource-constrained ESP32 microcontroller. This process demonstrates how deep learning can extend to edge devices, even with strict memory and processing limits. I hope this inspires you to explore similar optimizations for your embedded projects!
