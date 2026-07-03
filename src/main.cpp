#include "settings.h"

// ================= Global Objects and Variables =================
WiFiClientSecure secured_client;
UniversalTelegramBot bot(botToken, secured_client);

// Conversation state management (for commands requiring confirmation)
bool pendingReboot = false;
String pendingRebootChatId = "";
unsigned long confirmationStartTime = 0;

// ================= Polling and Timeout Settings =================
const unsigned long BOT_POLLING_INTERVAL = 300;
const unsigned long CONFIRMATION_TIMEOUT = 15000; // 15 seconds confirmation time
unsigned long lastBotPollTime = 0;

// ================= Helper Function: Mathematical Operations (using TinyExpr) =================
String evaluateExpression(String expr) {
    // te_interp function requires a const char*, so we use .c_str()
    int err;
    double result = te_interp(expr.c_str(), &err);

    if (err) {
        // If parsing fails, err will be a non-zero value, indicating the error position
        return "No result, please check your expression.";
    } else {
        // Parsing successful
        return String(result);
    }
}

WiFiUDP udp;

// ================= Helper Function: Sending WoL =================
void sendWakeOnLan() {
    byte magicPacket[102];
    for (int i = 0; i < 6; i++) magicPacket[i] = 0xFF;
    for (int i = 6; i < 102; i++) magicPacket[i] = targetMac[i % 6];
    
    udp.beginPacket(IPAddress(255, 255, 255, 255), 9);
    udp.write(magicPacket, 102);
    udp.endPacket();
}

// ================= Message Handling Logic =================
void handleNewMessages(int numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;

        bool isAuthorized = false;
        if (chat_id == ADMIN_CHAT_ID) isAuthorized = true;
        else {
            for (uint8_t i = 0; i < sizeof(ALLOWED_CHAT_ID); i++) {
                if (chat_id == (String)ALLOWED_CHAT_ID[i]) { // Use index to access elements
                    isAuthorized = true;
                    break; // Found, exit loop
                }
            }
        }
        if (!isAuthorized) {
            bot.sendMessage(chat_id, "Sorry, I am a private bot.", "");
            continue; // Block and skip all subsequent command processing
        }


        if (pendingReboot && chat_id == pendingRebootChatId) {
            if (text.equalsIgnoreCase("yes")) {
                bot.sendMessage(chat_id, "Rebooting...", "");
                delay(1000);
                ESP.restart();
            } else {
                bot.sendMessage(chat_id, "Reboot cancelled.", "");
                pendingReboot = false;
                pendingRebootChatId = "";
            }
            continue;
        }

        String cmd = text;
        String args = "";
        cmd.trim();
        if (cmd.startsWith("/") || cmd.startsWith(">")) {
            cmd = cmd.substring(1);
        }
        int spaceIndex = cmd.indexOf(' ');
        if (spaceIndex != -1) {
            args = cmd.substring(spaceIndex + 1);
            cmd = cmd.substring(0, spaceIndex);
            args.trim();
        }
        cmd.toLowerCase();
        
        // ================= Command Matching Area =================

        if (cmd == "start")
        {
            String msg = "✨ *B*lack *B*oard *B*ot at your service! ✨\nAvailable commands:\n"
                         "```\n"
                         "calc <expression>     - Calculate a basic expression\n"
                         "should                - Basically a dice to decide Yes/No\n"
                         "  aliases: decide, if, hmm, dice\n\n"
                         "server <function>     - Bot device related functions:\n"
                         "  status / sta        - Show ESP32 device status\n"
                         "  reboot / reb        - Reboot the ESP32 device (Requires confirmation)\n"
                         "  aliases: svr, srv, sv, s\n\n"
                         "ping                  - Pong ♪ (Check bot connectivity)\n"
                         "wake                  - Send Wake-on-LAN packet to a PC\n"
                         "statusRainbow         - Show status color names\n"
                         "author                - Show bot author information\n"
                         "help                  - Show this command list\n"
                         "  aliases: commands, hepp, cmds, cmd\n"
                         "```";
            bot.sendMessage(chat_id, msg, "Markdown");
        }

        if (cmd == "ping") {
            bot.sendMessage(chat_id, "Pong ♪", "");
        }
        
        else if (cmd == "calc" || cmd == "calculate") {
            if (args == "") {
                    bot.sendMessage(chat_id, "No result, please check your expression.", "");
            } else {
                String result = evaluateExpression(args); // <--- New call
                bot.sendMessage(chat_id, "Calculation Result: `" + result + "`", "Markdown");
            }
        }
        
        else if (cmd == "should" || cmd == "decide" || cmd == "if" || cmd == "hmm" || cmd == "dice") {
            const char* responses[] = {"1 / Yes", "2 / Yes", "3 / Yes", "4 / No", "5 / No", "6 / No"};
            int choice = esp_random() % 6;
            bot.sendMessage(chat_id, "Probably *" + String(responses[choice]) + "*", "Markdown");
        }
        
        else if (cmd == "server" || cmd == "svr" || cmd == "srv" || cmd == "sv" || cmd == "s") {
            args.toLowerCase();
            if (args == "status" || args == "sta") {
                unsigned long uptimeSec = millis() / 1000;
                uint32_t freeHeapKB = ESP.getFreeHeap() / 1024;
                uint32_t totalHeapKB = ESP.getHeapSize() / 1024;
                long rssi = WiFi.RSSI();
                float temp = temperatureRead(); // <--- Read internal temperature
                
                String statusMsg = "*Bot Status*\n\n";
                statusMsg += "🌡️ Internal Temp: `" + String(temp, 1) + " °C`\n";
                statusMsg += "⏱️ Uptime: `" + String(uptimeSec / 3600) + "h " + String((uptimeSec % 3600) / 60) + "m`\n";
                statusMsg += "💾 Free Memory: `" + String(freeHeapKB) + " / " + String(totalHeapKB) + " KB`\n";
                statusMsg += "📶 Wi-Fi Signal: `" + String(rssi) + " dBm`\n";
                bot.sendMessage(chat_id, statusMsg, "Markdown");
            } 
            else if (args == "reboot" || args == "reb") {
                bot.sendMessage(chat_id, "⚠️ *Warning:* You are about to reboot the ESP32.\nType `yes` within 15 seconds to confirm.", "Markdown");
                pendingReboot = true;
                pendingRebootChatId = chat_id;
                confirmationStartTime = millis();
            }
            else {
                bot.sendMessage(chat_id, "That's not a valid function for this bot. Available functions:\n```\nstatus    - Show bot device status\nreboot    - Reboot the bot device\n```", "Markdown");
            }
        }

        else if (cmd == "statusrainbow") {
            String rainbowMsg = "Color demonstration:\n\n"
                                 "🟣 puρπle\n"
                                 "🟠 orånʒ\n"
                                 "🟡 yeλλow\n"
                                 "🟢 miΔorι\n"
                                 "🔵 blau";
            bot.sendMessage(chat_id, rainbowMsg, "Markdown");
        }

        else if (cmd == "help" || cmd == "commands" || cmd == "hepp" || cmd == "cmds" || cmd == "cmd") {
            String msg = "Available commands:\n"
                         "```\n"
                         "calc <expression>     - Calculate a basic expression\n"
                         "should                - Basically a dice to decide Yes/No\n"
                         "  aliases: decide, if, hmm, dice\n\n"
                         "server <function>     - Bot device related functions:\n"
                         "  status / sta        - Show ESP32 device status\n"
                         "  reboot / reb        - Reboot the ESP32 device (Requires confirmation)\n"
                         "  aliases: svr, srv, sv, s\n\n"
                         "ping                  - Pong ♪ (Check bot connectivity)\n"
                         "wake                  - Send Wake-on-LAN packet to a PC\n"
                         "statusRainbow         - Show status color names\n"
                         "author                - Show bot author information\n"
                         "help                  - Show this command list\n"
                         "  aliases: commands, hepp, cmds, cmd\n"
                         "```";
            bot.sendMessage(chat_id, msg, "Markdown");
        }

        else if (cmd == "author") {
            String msg =
                         "Based on the Discord Bot by [theArnoll](https://github.com/theArnoll)\n"
                         "Discord Bot repo is located [here](https://github.com/theArnoll/serverboxDCutil)\n"
                         "ESP32 Telegram Bot version converted with Google Gemini, Gemini 2.5 Pro on Vertex AI and verified by theArnoll\n"
                         "Telegram Bot repo is located [here](https://github.com/theArnoll/ESP32_telegram_bot)\n"
                         ;
            bot.sendMessage(chat_id, msg, "Markdown");
        }
        
        else if (cmd == "wake") {
            if (chat_id == ADMIN_CHAT_ID)
            {
                sendWakeOnLan();
                bot.sendMessage(chat_id, "Woke your PC!", "");
            }
            else bot.sendMessage(chat_id, "No, you don't have the permission to wake author's PC.");
        }
    }
}

void checkTimeout() {
    if (pendingReboot && (millis() - confirmationStartTime > CONFIRMATION_TIMEOUT)) {
        bot.sendMessage(pendingRebootChatId, "Timeout. Reboot cancelled.", "");
        pendingReboot = false;
        pendingRebootChatId = "";
    }
}

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    
    secured_client.setInsecure();

    bot.sendMessage(ADMIN_CHAT_ID, " ✨ *B*lack *B*oard *B*ot at your service ✨", "Markdown");
}

void loop() {
    checkTimeout();

    if (millis() - lastBotPollTime > BOT_POLLING_INTERVAL) {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        while (numNewMessages) {
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastBotPollTime = millis();
    }
}
