---
layout: post
title: Build Your Own IoT Cloud Locally with Docker Compose (For Embedded Engineers)
category: programming
description: Learn how to build a local IoT cloud using Docker Compose, MQTT, MinIO, Cassandra, and FastAPI. A step-by-step guide for embedded engineers to understand how the cloud works under the hood.
tags: [embedded, iot, docker, mqtt, cloud]
---

For many embedded engineers, “the cloud” feels abstract and kind of mysterious. It was the same for me.
IoT devices today are always connected: sending, receiving, analyzing, and showing data. But how does it *actually* work behind the scenes?

To figure it out, I built my own local cloud infrastructure. That way, I could really see how the pieces fit together.

* 
{:toc}


# My Local Cloud

I made a repo called [*IoT Cloud Infrastructure*](https://github.com/daleonpz/iot_cloud_infrastructure). It’s a local data pipeline built with Docker Compose. The idea is to recreate the kind of setup you’d see in the cloud, but run it on your laptop or a local server. That way, you can experiment without cloud providers hiding the details.

At the start, I only knew I needed:

* **Storage** (somewhere to put the data)
* **MQTT broker** (still didn’t know what it really meant at the time)
* **A way to convert raw data** into something usable
* **Access to clean data** for analysis or display

After a bunch of googling and hours reading about AWS, Azure, and databases, I ended up with this setup:

* **Docker Compose**: manages all the services as containers
* **MQTT Broker (Mosquitto)**: entry point where devices send/receive data
* **MQTT Agent (Python app)**: subscribes to topics and writes raw data to the Data Lake
* **Data Lake (MinIO)**: stores raw, unprocessed data (S3-compatible)
* **Transformation (Python apps + Airflow)**: processes data and writes it to the DB
* **Database (Cassandra)**: NoSQL DB that’s lightweight, scalable, and open-source
* **Airflow**: orchestrates data flows and schedules jobs
* **REST API (FastAPI)**: exposes processed data through a simple REST interface

Because it’s all Docker Compose, it’s easy to spin up, shut down, check logs, simulate failures, or add new pieces.

![Local IoT Cloud Architecture](/images/posts/local_iot_cloud_architecture.png)

The full code is in the repo, but here’s a quick overview of how it works:
1. **Devices** publish data to the MQTT broker (Mosquitto).
2. The **MQTT Agent** subscribes to relevant topics and saves incoming messages as files in the **Data Lake** (MinIO).
3. **Airflow** runs scheduled jobs that trigger **Transformation** scripts to process raw data from MinIO.
4. Processed data is stored in the **Database** (Cassandra).
5. The **REST API** (FastAPI) provides endpoints to access the processed data.

# What I Learned

* **Cloud ≠ magic**: At the core, it’s just a bunch of apps running on some machine.
* **Latency adds up**: Even locally, chaining MQTT → storage → transformation → DB → API introduces delays. Makes you appreciate why cloud providers obsess over optimization.
* **Visibility**: You can *see everything* — logs, errors, broken data flows. If something fails, you know exactly where.


# Conclusion

If you’re an embedded engineer wondering how the cloud works, building your own local pipeline is one of the best ways to learn. It doesn’t need to be perfect — but it will give you a much deeper understanding of how IoT data really flows.


