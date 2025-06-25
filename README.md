# Task: FreeRTOS #2 - EmbarcaTech 2025

Author: **Luana Menezes**

Course: Technological Residency in Embedded Systems

Institution: EmbarcaTech - HBr

Campinas, June 2025

---

# FreeRTOS Audio-Reactive LED Project

This project develops an embedded real-time audio visualization system using the BitDogLab board, programmed with FreeRTOS in C language on VSCode. The system implements a **three-task pipeline architecture** that captures, processes, and visualizes audio data in real-time:

- **Audio Capture Task**: Continuously samples microphone input at 1kHz using ADC
- **Audio Processing Task**: Amplifies and processes raw audio signals  
- **LED Animation Task**: Creates visual patterns on NeoPixel matrix based on audio amplitude

The implementation demonstrates **inter-task communication**, **real-time data processing**, and **synchronized task execution** using FreeRTOS queues, semaphores, and task scheduling.

**What RTOS concepts does this project demonstrate?**
Answer: This project showcases task priorities, queue-based producer-consumer patterns, binary semaphores for synchronization, precise timing with vTaskDelayUntil(), and real-time pipeline processing - all fundamental RTOS concepts for embedded systems.

## Objective

Develop skills in embedded real-time systems programming using FreeRTOS. Upon completion, you will be able to:

- Implement multi-task pipeline architectures
- Use FreeRTOS queues for inter-task communication
- Apply binary semaphores for task synchronization
- Manage task priorities and scheduling
- Process real-time audio data streams
- Control NeoPixel LED matrices with dynamic patterns

## Bill of Materials

| Component                | BitDogLab Connection      |
|--------------------------|---------------------------|
| BitDogLab (RP2040)       | -                         |
| Microphone (Analog)      | GPIO 28 (ADC Channel 2)   |
| NeoPixel LED Matrix (5x5)| GPIO 7                    |


## Setup and Execution

1. Clone the FreeRTOS kernel:
   ```bash
   git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS-Kernel
   ```
2. Open the project in VS Code with Raspberry Pi Pico SDK environment
3. Build the project (Ctrl+Shift+B or cmake/make)
4. Connect BitDogLab via USB in boot mode (BOOTSEL button)
5. Copy the generated .uf2 file to RPI-RP2 storage
6. The system will automatically start visualizing audio input

## System Logic

### Audio Capture Task (Priority 3 - Highest):
- Samples microphone at 1kHz
- Calculates amplitude from 12-bit ADC values (centered at 2048)
- Sends amplitude data to processing queue

### Audio Processing Task (Priority 2 - Medium):  
- Receives raw amplitude from capture queue
- Amplifies signal by 4x for better visualization
- Sends processed data to LED queue
- Signals LED task via binary semaphore

### LED Animation Task (Priority 1 - Lowest):
- Waits for semaphore signal from processing task
- Maps audio amplitude to LED rows (0-5 active rows)
- Creates color gradient: Green→Yellow→Orange→Red
- Updates 5x5 NeoPixel matrix


## Project Files

- [`main.c`](main.c): Main project code with FreeRTOS three-task implementation
- [`include/neopixel.h`](include/neopixel.h): NeoPixel LED matrix control interface
- [`src/neopixel.c`](src/neopixel.c): NeoPixel driver implementation


## Features Demonstrated

- **Multi-task Pipeline**: Three synchronized tasks with different priorities
- **Inter-task Communication**: Queue-based producer-consumer patterns
- **Binary Semaphore**: Event-driven LED updates and task synchronization
- **Real-time Processing**: Precise 1kHz audio sampling with vTaskDelayUntil()
- **Hardware Control**: ADC microphone input and NeoPixel LED matrix output

## 🖼️ Project Images/Videos

https://github.com/user-attachments/assets/e5942d3a-10f6-41a6-89f5-c9946b4e845e


## 📜 License
GNU GPL-3.0.
