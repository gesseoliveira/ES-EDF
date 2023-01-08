# ES-EDF

This project was developed during the research of the PGGCOMP master's program at the Federal University of Bahia, located in Brazil. The main objective is to provide a practical scientific contribution applying energy saving algorithms to the DPM technique for real-time applications. The STM32F407 hardware was used as a validation platform and the real-time operating system (RTOS) was FreeRTOS, which is currently the most used RTOS for systems.

## Getting Started

These instructions will give you a copy of the project up and running on
your local machine for development and testing purposes. See deployment
for notes on deploying the project on a live system.

### Prerequisites

Requirements for the software and other tools to build, test and push 
- [STMCUBEIDE](https://www.st.com/en/development-tools/stm32cubeide.html)
- [STM32F407](https://www.st.com/en/evaluation-tools/stm32f4discovery.html#overview)

### Workspace structure

This repository must be placed inside the stmcubeide working folder. It consists of 3 folders:
- DRV: Software layer that manages microcontroller peripherals
- FreeRTOS: FreeRTOS application that contains the ES-EDFimplementation. 
- COMMON: Folder containing generic files for future applications.

## Running application

Before running the example application, it is necessary to import the project. To import, follow these steps:
1) Clone the repository for the workspace to be used
2) Open STMCUBEIDE
3) File -> Import
4) Existing Projects into Workspace
5) Select the cloned project.
6) Compile and record in the STM32F407 that must be connected to the computer;

For more information to project import, please consult: https://fastbitlab.com/microcontroller-embedded-c-programming-importing-projects-in-to-stm32cubeide-workspace/

## Related works
- https://ieeexplore.ieee.org/document/9277851/ (freertos kernel evaluation using EDF) 

