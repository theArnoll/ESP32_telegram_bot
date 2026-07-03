# ESP32 Telegram Bot

[English](README.md) | 繁體中文 (zh-TW)

一個基於 ESP32 的 Telegram 機器人，提供指令執行、數學運算以及 Wake-on-LAN (WoL) 等多種功能。

## 功能特色

*   **Telegram 指令控制**: 透過 Telegram 訊息與 ESP32 設備互動。
*   **安全連接**: 使用 `WiFiClientSecure` 確保通訊安全。
*   **授權控制**: 僅允許指定的 Telegram 用戶 ID 執行指令，保護設備安全。
*   **系統狀態查詢**: 獲取 ESP32 的執行時間、可用記憶體、WiFi 訊號強度和內部溫度。
*   **設備重啟**: 提供安全機制，需確認後才能重啟 ESP32。
*   **數學運算**: 支援基本的數學表達式計算。
*   **隨機決策**: 簡單的「是/否」決策工具。
*   **Wake-on-LAN (WoL)**: 透過向特定 MAC 位址發送魔法封包來喚醒區域網路中的電腦（僅限管理員）。
*   **訊息輪詢**: 定期檢查新訊息，確保指令即時響應。

## 指令總覽

以下是可用的一些主要指令：

*   `/start`: 顯示歡迎訊息和所有可用指令。
*   `/ping`: 檢查機器人是否在線。
*   `/calc <expression>`: 計算數學表達式 (例如 `/calc 2*3+5`)。
*   `/should` (或 `/decide`, `/if`, `/hmm`, `/dice`): 隨機決定「是」或「否」。
*   `/server status` (或 `/server sta`): 顯示 ESP32 設備的狀態資訊。
*   `/server reboot` (或 `/server reb`): 重啟 ESP32 設備（需要確認）。
*   `/wake`: 發送 Wake-on-LAN 封包（僅限管理員）。
*   `/author`: 顯示機器人開發者資訊及參考來源。
*   `/help` (或 `/commands`, `/cmds`): 顯示此指令列表。

## 環境設定

1.  **硬件**: ESP32 開發板
2.  **開發環境**: PlatformIO

## 配置步驟

1.  編輯 `settings.h` 檔案:
    *   在專案目錄中，請編輯 `src/settings.h` 的檔案，指引如下

    ```c++
    const char* ssid = "你的 Wi-Fi 名稱";
    const char* password = "你的 Wi-Fi 密碼";
    const String botToken = "你 Telegram bot 的 Token (請從 @BotFather 獲取)";
    const String ADMIN_CHAT_ID = "你的 Telegram 聊天室 ID (請從 @userinfobot 獲取)";
    
    // 可選：新增其他已授權的聊天 ID
    // 如果僅允許管理員使用，可留空或刪除。
    const String ALLOWED_CHAT_ID[] = {}; 

    // 給網路喚醒裝置功能
    byte targetMac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x23}; // 目標裝置的 MAC 位址
    ```

2.  編譯與上傳:
    將程式碼編譯並上傳到您的 ESP32-S3 開發板。

    已測試過的開發板: ESP32-S3-N16R8-WROOM-DevKitC，但理論上來講究算是 ESP32-C3 也應該跑得動

## 鳴謝

這個專案基於我用 Python 編寫的 Discord bot，該機器人運行在 Linux 機器上。而這個版本是為 ESP32 設計的，並由 Google Gemini（運行在 Vertex AI 上的 Gemini 2.5 Pro）轉換，再由我編輯和驗證。