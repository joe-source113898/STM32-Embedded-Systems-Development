**STM32G0B1 CAN Message Handler and LCD Data Displayer**

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
3. [Hardware Requirements](#hardware-requirements)
4. [Software Requirements](#software-requirements)
5. [Getting Started](#getting-started)
6. [Usage](#usage)
7. [Contributing](#contributing)
8. [License](#license)
9. [Acknowledgments](#acknowledgments)

## Introduction

This repository contains the source code and documentation for an embedded system designed to handle CAN messages using the STM32G0B1 microcontroller and display related data on an LCD.

## Features

- **CAN Message Handling:** Efficiently processes and filters incoming CAN messages.
- **LCD Display Integration:** Displays relevant CAN message data on an LCD in real-time.
- **Error Handling:** Provides mechanisms to handle and report any CAN or LCD related errors.
- **Scalability:** Modular design allows for easy integration with other systems or additional features.

## Hardware Requirements

- STM32G0B1 Microcontroller board
- CAN transceiver module (compatible with STM32G0B1)
- LCD display (compatible with STM32G0B1)

## Software Requirements

- Visual Studio Code
- STM32 HAL libraries
- Any terminal software for debugging purposes (GDB)

## Getting Started

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/joe-source113898/STM32-Embedded-Systems-Development.git

2. **Set Up the Hardware:** Connect the STM32G0B1 board, CAN transceiver, and LCD as per the schematic provided in the docs folder.
   
3. **Install Required Software:** Ensure you have the Visual Studio Code and necessary libraries installed.

4. **Open the Project:** Import the project into Visual Studio Code.
   
5. **Compile & Flash:** Build the project and flash it onto the STM32G0B1 board.

## Usage
After flashing the firmware:

1. Power on the system.

2. Send CAN messages to the STM32G0B1 board. The board will process the messages and display relevant data on the LCD.

3. Monitor the LCD for the displayed data.

For detailed usage and configuration instructions, please refer to the user manual in the ```docs``` folder.

## Contributing
Contributions are welcome! Please read our ```CONTRIBUTING.md``` for details on how to contribute, and the process for submitting pull requests.

## License
This project is licensed under the MIT License - see the ```LICENSE.md``` file for details.

## Acknowledgments
Thanks to Diego Perez and Modular MX for their valuable resources and support.
Shout out to all the contributors who have helped improve this project.
