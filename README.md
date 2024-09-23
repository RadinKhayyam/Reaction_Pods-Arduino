# Designing and Implementing Training Equipment to Improve Athletes' Reaction Time

This project involves the design and development of training equipment aimed at improving athletes' response to stimuli. The system is designed for use in rehabilitation and sports training settings.

## Project Description

The system is composed of four pods: one master pod and three slave pods, each powered by an Arduino microcontroller. The master pod communicates with a mobile device via a Bluetooth module, while the slave pods connect to the master pod using RF modules, allowing for wireless communication across the system.

### How the Pods Work

Through the mobile application, you can program the pods to turn on different colored LEDs or produce sound in any pattern you prefer. The athlete is tasked with tapping the pods when they activate. Additionally, when the athlete brings their hand close to the pods or taps them, the proximity sensors on the boards detect the movement. This allows the system to monitor and record their interaction with the pods.

After each session, the application provides a detailed report on the athlete's reaction times, allowing for real-time feedback and progress tracking.

This training equipment is inspired by commercial examples already on the market, such as **BlazePod**, which provide similar functionality for improving athletic performance through reaction training.

I designed and manufactured a custom PCB for the hardware, ensuring optimal integration of all components. Additionally, an Android application was developed to control the pods, allowing users to design and program training routines with ease.

## PCB Design

Below are images of the PCBs designed for this project:

![Master Pod PCB](img1.png)
*Master pod PCB design.*

![Slave Pod PCB](img2.png)
*Slave pod PCB design.*


