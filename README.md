# They Chat

## Overview

TheyChat is a client-server chat application developed solely by me. The application allows multiple clients to connect to a server, join chat rooms, exchange messages, and transfer files. The project is built using CMake and is designed to run on POSIX systems, utilizing pthreads for multithreading and ncurses for a simple client UI. Communication between the client and server is encrypted using SSL.

## Features

- **Multithreaded Server**: Handles multiple clients concurrently using pthreads.
- **Client UI**: Simple text-based interface built with ncurses.
- **Encrypted Communication**: Ensures secure message and file transfer using SSL.
- **Cross-platform**: Designed to run on POSIX-compliant systems.
- **File Transfer**: Supports sending and receiving files between clients.
- **CMake Build System**: Easy to build and manage the project.
- **MIT Licensed**: Open-source and freely available for modification and distribution.

## Getting Started

### Prerequisites

- POSIX-compliant system (Linux, macOS, etc.)
- CMake
- OpenSSL
- ncurses

### Building the Project

1. Clone the repository:
    ```sh
    git clone https://github.com/FrancoYudica/TheyChat.git
    cd TheyChat
    ```

2. Create a build directory and navigate into it:
    ```sh
    mkdir build
    cd build
    ```

3. Run CMake to configure the project:
    ```sh
    cmake ..
    ```

    **Build Options:**
    - `THEY_CHAT_COMPILE_TEST`: Enable or disable tests.
      ```sh
      cmake -DTHEY_CHAT_COMPILE_TEST=ON ..
      ```
    - `THEY_CHAT_ENABLE_SSL`: Enable or disable SSL encryption. If enabled, a valid certificate and key file (both with .pem extension) should be provided.
      ```sh
      cmake -DTHEY_CHAT_ENABLE_SSL=ON ..
      ```

4. Build the project:
    ```sh
    make
    ```

### Running the Server

1. Navigate to the build directory:
    ```sh
    cd build
    ```

2. Run the server executable:
    ```sh
    ./server
    ```

### Running the Client

1. Open a new terminal and navigate to the build directory:
    ```sh
    cd build
    ```

2. Run the client executable:
    ```sh
    ./client
    ```

## Usage

- **Server**: The server application listens for incoming client connections, manages chat rooms, and handles message and file transfers.
- **Client**: The client application connects to the server, allowing users to join chat rooms, send and receive messages, and transfer files.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgements

- [ncurses](https://invisible-island.net/ncurses/)
- [OpenSSL](https://www.openssl.org/)

## Contact

If you have any questions or suggestions, feel free to contact me via GitHub.
