# Electronic Multimodal Gameboard: Interactive Board Game with Piece Recognition and Dynamic Feedback


Link to the full thesis paper: https://mau.diva-portal.org/smash/get/diva2:1981222/FULLTEXT02.pdf  (Swedish)

Links to video demostrations:

Tic tac toe, Pvp:             https://www.youtube.com/watch?v=CgwtxI6al1w&feature=youtu.be

Tic tac toe, Player vs AI:    https://www.youtube.com/watch?v=wis4Aml1rb4

Othello, Pvp:                 https://www.youtube.com/watch?v=7IzF799TEdc

Othello,Player vs AI:         https://www.youtube.com/watch?v=cJhUXVJmr3U


# Project Overview
This repository contains the source code and documentation for my bachelor's thesis project in Computer Science Engineering at Malmö University, completed in June 2025. The thesis was originally written in Swedish, but this documentation is provided in English for wider accessibility.


# About the Project
The project focuses on developing an interactive board game system that uses RFID technology for piece recognition and provides dynamic feedback to players. The system implements both player-vs-player and player-vs-AI modes, with the AI utilizing the Minimax algorithm with Alpha-Beta pruning.
This thesis investigates how sensor technology can be utilized to create a more engaging and interactive gaming experience. Furthermore, it analyzes how dynamic feedback can be conveyed through different channels, and how the system can support multiple different games on the same physical board. The outcome is a functional prototype that demonstrates the potential of electronic game boards to renew and increase interest in traditional board games.

# Core Components
* RFID-based piece recognition system for tracking game pieces on the board
* ESP32 microcontroller implementation for processing and communication
* MQTT protocol for client-server communication
* Modular C++/Python architecture for game logic and AI
* Interactive visual feedback system using LED strips and LCD displays

# Technical Architecture
## System Design
The system follows an Internet of Things (IoT) approach, where physical game pieces interact with digital systems through RFID technology. The architecture consists of:
1. Physical Layer: Game board with embedded PNS32 RFID sensors
2. Communication Layer: ESP32 microcontrollers with Wi-Fi capabilities using MQTT
3. Application Layer: Modular software architecture handling game logic and AI

## Communication Protocol
The system uses MQTT (Message Queuing Telemetry Transport) for efficient client-server communication. This lightweight protocol is ideal for IoT applications due to its low bandwidth requirements and reliability in networks with varying levels of latency.

## AI Implementation
For player-vs-AI mode, the system implements the Minimax algorithm with Alpha-Beta pruning optimization to create a challenging opponent while maintaining reasonable computation times.

# Hardware Components
## RFID Technology
The game uses RFID (Radio Frequency Identification) technology to identify and track the position of game pieces on the board. Each game piece contains an RFID tag with a unique identifier, which is read by sensors embedded in the game board.

# Diagrams
## System overview
<img width="1179" height="675" alt="image" src="https://github.com/user-attachments/assets/298ac0f7-ef75-442a-a629-1056479ac1b7" />

## Wiring diagram for ESP32 and PN532 RFID readers connected via SPI
<img width="914" height="663" alt="image" src="https://github.com/user-attachments/assets/b2951a5f-7d3f-469b-ba54-9826902ce7ce" />

The prototype uses five ESP32 microcontrollers, each mounted on a breadboard with soldered wire connections for RFID readers, LED strips, LCDs, and buttons:

ESP A (8 PN532 readers): handles eight RFID readers via SPI.

ESP B (8 PN532 readers): handles eight RFID readers via SPI.

The system contains two such devices.

## Wiring diagram for ESP32 and five push buttons
<img width="456" height="719" alt="image" src="https://github.com/user-attachments/assets/0ab14e64-ec3c-4f36-a06d-e3e58b197e7d" />

Handles five push buttons for game selection and reset.

## Wiring diagram for ESP32 and two TFT/LCD displays
<img width="420" height="722" alt="image" src="https://github.com/user-attachments/assets/69b8a9b4-fde8-4e46-a250-723b1bd2abc8" />

Drives two TFT/LCD screens (one per player).

## Wiring diagram for ESP32 and 16 LED strips
<img width="1219" height="638" alt="image" src="https://github.com/user-attachments/assets/e70308c4-c301-485f-8d1c-7ed7bc369615" />

Controls 16 LED strips on digital outputs.

## Full prototype

<img width="1600" height="1166" alt="image" src="https://github.com/user-attachments/assets/6d619a9e-b76e-4936-852b-6584f4e28722" />

The readers are intended to be mounted in a 4×4 grid with ~2cm spacing below the board surface. All modules are sample mounted on breadboards. The wiring diagram shows GPIO pins for each ESP32, including CS/MOSI/MISO/SCK for SPI modules and digital pins for LEDs/buttons. The buttons use internal pull-up resistors, eliminating the need for external components.

# Software Implementation
## Code Structure
The software architecture is modular and consists of several key components:
1. RFID Reader Module: C++ code for interfacing with the RFID sensors
2. Communication Module: Implementation of MQTT client for message passing
3. Game Logic Module: Rules enforcement and game state management
4. AI Module: Minimax algorithm with Alpha-Beta pruning for computer player
5. Feedback Module: Control of LED strips, LCD displays and a visual GUI for user feedback

# Game Class diagram
<img width="882" height="678" alt="image" src="https://github.com/user-attachments/assets/c62f2d9e-e0ca-4778-afac-08fc13c647c4" />

To support multiple games on the same physical board, a flexible software architecture based on inheritance and polymorphism was developed. An abstract base class 'Base Game' defines basic game interfaces and functionality that is common to all board games, such as:
- Initializing the game board
- Handling player interaction
- Updating game status
- Checking game rules and winning conditions

Each game is implemented as a subclass of the base class and inherits the common functionality. The class diagram below is an example of how a game class 'BaseTicTacToe' inherits functions from the base class, with an additional level of inheritance if you want to create the same game with multiple variants.

# Overall Class diagram

<img width="1350" height="882" alt="image" src="https://github.com/user-attachments/assets/8dd96206-90d5-4899-b2d5-2a1a38ae3f88" />


# Feedback tools

## GUI
<img width="591" height="788" alt="image" src="https://github.com/user-attachments/assets/e5f3425f-688d-41bc-8ea3-8531a7200fad" />
<img width="507" height="668" alt="image" src="https://github.com/user-attachments/assets/75748d88-2bb9-4b7b-93bc-070503db4df9" />

## LCD
<img width="1600" height="1481" alt="image" src="https://github.com/user-attachments/assets/e8b9d14a-9bad-43c6-a694-ba933ae6ed9e" />

## LED
<img width="1600" height="1200" alt="image" src="https://github.com/user-attachments/assets/80c9616b-fe15-4de7-9796-ff12e3d8eca7" />


# Project Results
The final system successfully demonstrates how traditional board games can be enhanced with digital technology while maintaining the tactile experience of physical play. The project achieved:
- Reliable piece recognition using RFID technology
- Low-latency communication between physical board and digital systems (most of the time, read thesis document 5.4.4 for commonly shown issues).
- Intuitive visual feedback through integrated displays
- Challenging AI opponent using optimized game tree search

# Prototype initial drawing
<img width="1552" height="1670" alt="image" src="https://github.com/user-attachments/assets/fb5f9535-4a76-443f-9328-f305983707d6" />
<img width="1723" height="1856" alt="image" src="https://github.com/user-attachments/assets/7d910ab5-9a46-4bd2-8934-61b46f0d8966" />

## Building the case
Making the actual case wasn't really a requirement for our thesis, however it would have felt more like an actual game board, so we attempted to. It was quite challenging at first since we didn't have much experience with Adobe Illustrator and using a laser cutter. So this is what we managed to do.

![IMG_1120](https://github.com/user-attachments/assets/c2cae343-d370-456c-9a29-be127e2f8092)
Drawing the board into cardboard first, taking the right measurements.

![IMG_1214](https://github.com/user-attachments/assets/657a0c02-49a9-4316-9509-4ace306e5324)
Drawing the necessary parts in Adobe Illustrator.

![IMG_1213](https://github.com/user-attachments/assets/7fff13b6-f33d-4ddb-b45b-1bbefc95ac1e)
Cutting them out.

# Final prototype
![IMG_2548](https://github.com/user-attachments/assets/7f962e9b-735b-414e-ad2c-10620ea6a11c)

![IMG_2546](https://github.com/user-attachments/assets/800329ad-5383-4375-875c-9f3a16ee24b1)

![IMG_2571](https://github.com/user-attachments/assets/330c8377-7f50-4fb2-b203-6580fca836d9)

We unfortunately did not have enough time to finish the board, as soldering every individual component took a lot of time (and caused a lot of error...).

# Future Work
Potential extensions to this project include:
- Support for additional board games beyond the current implementation
- Improved AI using machine learning techniques
- Mobile app integration for remote play
- Enhanced visual and audio feedback systems
