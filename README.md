# ESP32 Telegram Bot

English | [繁體中文 (zh-TW)](README_zh-TW.md)

An ESP32-based Telegram bot providing various functionalities such as command execution, mathematical calculations, and Wake-on-LAN (WoL).

## Features

*   **Telegram Command Control**: Interact with your ESP32 device via Telegram messages.
*   **Secure Connection**: Utilizes `WiFiClientSecure` for secure communication.
*   **Authorization Control**: Only allowed Telegram user IDs can execute commands, enhancing device security.
*   **System Status Query**: Get ESP32 uptime, free memory, WiFi signal strength, and internal temperature.
*   **Device Reboot**: Provides a secure mechanism requiring confirmation before rebooting the ESP32.
*   **Mathematical Operations**: Supports evaluation of basic mathematical expressions.
*   **Random Decision-Making**: A simple "Yes/No" decision-making tool.
*   **Wake-on-LAN (WoL)**: Send a magic packet to wake up a PC on the local network at a specific MAC address (admin-only).
*   **Message Polling**: Periodically checks for new messages to ensure timely command responses.

## Command Overview

Here are some of the main commands available:

*   `/start`: Displays a welcome message and all available commands.
*   `/ping`: Checks if the bot is online.
*   `/calc <expression>`: Calculates a mathematical expression (e.g., `/calc 2*3+5`).
*   `/should` (or `/decide`, `/if`, `/hmm`, `/dice`): Randomly decides "Yes" or "No".
*   `/server status` (or `/server sta`): Shows the ESP32 device's status information.
*   `/server reboot` (or `/server reb`): Reboots the ESP32 device (requires confirmation).
*   `/wake`: Sends a Wake-on-LAN packet (admin-only).
*   `/author`: Displays bot author information and references.
*   `/help` (or `/commands`, `/cmds`): Shows this command list.

## Environment Setup

1.  **Hardware**: ESP32 Development Board
2.  **Development Environment**: PlatformIO

## Configuration Steps

1.  **Edit `settings.h` file**:
    *   In your project directory, please edit the `src/settings.h` file as guided below:

    ```c++
    const char* ssid = "YOUR_WIFI_NAME"
    const char* password = "YOUR_WIFI_PASSWORD"
    const String botToken = "YOUR_TELEGRAM_BOT_TOKEN (get from @BotFather)"
    const String ADMIN_CHAT_ID = "YOUR_TELEGRAM_CHAT_ID (get from @userinfobot)"
    // Optional: Add other authorized chat IDs
    // If only the administrator is allowed, you can leave it empty or delete it.
    const String ALLOWED_CHAT_ID[] = {}; 

    // For Wake-on-LAN (WoL) functionality
    byte targetMac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x23};  // Target device's MAC address
    ```
2.  **Compile and Upload**:
    Compile and upload the code to your ESP32-S3 development board.

    Tested development board: ESP32-S3-N16R8-WROOM-DevKitC, but theoretically, it should also run on ESP32-C3.

## Acknowledgments

This project is based on [my Discord bot wrote in Python](https://github.com/theArnoll/serverboxDCutil), and that's running on a Linux machine, and this version is for ESP32 and was converted with Google Gemini (Gemini 2.5 Pro on Vertex AI), edited and verified by me.