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
- FreeRTOS: FreeRTOS application that contains the implementation
- COMMON: Folder containing generic files for future applications.

## Running application

Before running the example application, it is necessary to import the project. To import, follow these steps:
1) Clone the repository for the workspace to be used
2) Open STMCUBEIDE
3) File -> Import
4) Existing Projects into Workspace
5) Select the cloned project.
6) Compile and record in the STM32F407 that must be connected to the computer;
## Built With

  - [Contributor Covenant](https://www.contributor-covenant.org/) - Used
    for the Code of Conduct
  - [Creative Commons](https://creativecommons.org/) - Used to choose
    the license

## Authors

  - **Billie Thompson** - *Provided README Template* -
    [PurpleBooth](https://github.com/PurpleBooth)

See also the list of
[contributors](https://github.com/PurpleBooth/a-good-readme-template/contributors)
who participated in this project.


## Acknowledgments

  - Hat tip to anyone whose code is used
  - Inspiration
  - etc
