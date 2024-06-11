# They Chat

## Overview

TheyChat is a client-server chat application developed solely by me. The application allows multiple clients to connect to a server, join chat rooms, and exchange messages. The project is built using CMake and is designed to run on POSIX systems, utilizing pthreads for multithreading and ncurses for a simple client UI. Communication between the client and server is encrypted using SSL.

## Features

- **Multithreaded Server**: Handles multiple clients concurrently using pthreads.
- **Client UI**: Simple text-based interface built with ncurses.
- **Encrypted Communication**: Ensures secure message transfer using SSL.
- **Cross-platform**: Designed to run on POSIX-compliant systems.
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
    git clone --recursive https://github.com/FrancoYudica/TheyChat.git
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
    - `THEY_CHAT_SSL`: Enable or disable SSL encryption.
      ```sh
      cmake -DTHEY_CHAT_SSL=ON ..
      ```
    - `THEY_CHAT_COMPILE_TEST`: Enable or disable tests.
      ```sh
      cmake -DTHEY_CHAT_COMPILE_TESTS=ON ..
      ```

    If `THEY_CHAT_SSL` is enabled, a valid certificate and key file should be provided. By default, the application expects the certificate and key to be located in `~/.ssl/TheyChat/` directory with filenames `certificate.pem` and `private.key` respectively.

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

- **Server**: The server application listens for incoming client connections, manages chat rooms, and handles message transfers.
- **Client**: The client application connects to the server, allowing users to join chat rooms and send and receive messages.

## Generating SSL Certificates

To generate the SSL certificate and key required for secure communication, you can use the `generate_ssl.sh` script included in the repository. This script creates the necessary files in the `~/.ssl/TheyChat/` directory. The generated certificate is a self-signed certificate, created for testing purposes.

To run the script, navigate to the directory where the script is located and execute it:

```sh
chmod +x generate_ssl.sh
./generate_ssl.sh
```
## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgements

- [ncurses](https://invisible-island.net/ncurses/)
- [OpenSSL](https://www.openssl.org/)

## Contact

If you have any questions or suggestions, feel free to contact me via GitHub.
