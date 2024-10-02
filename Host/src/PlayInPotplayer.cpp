/**
    Native Messaging host program
    To link Chrome browser with the PotPlayer
*/
#include <iostream>
#include <fstream>
#include <string>
#include <format>
#include <thread>
#include <chrono>
#include <filesystem>
#include <windows.h>
#include <winuser.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Configuration file
constexpr auto CONF_FILE = "config.json";
// Configuration struct with default values
struct Config {
    std::string log_file = "url_log.txt";
    std::string potplayer_path = "C:\\Program Files\\DAUM\\PotPlayer\\PotPlayerMini64.exe";
};

// Load configuration from JSON file
Config load_config() {
    Config config;
    try {
        std::ifstream config_file(CONF_FILE);
        if (config_file.is_open()) {
            json config_json;
            config_file >> config_json;

            if (config_json.contains("log_file")) config.log_file = config_json["log_file"];
            if (config_json.contains("potplayer_path")) config.potplayer_path = config_json["potplayer_path"];
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading config, using defaults: " << e.what() << std::endl;
    }
    return config;
}

// Global configuration
Config config = load_config();

// Simple logging class
class FileLogger {
private:
    std::filesystem::path filename;
    std::ofstream file;

    void ensure_file_open() {
        if (!file.is_open()) {
            try {
                file.open(filename, std::ios::app);
            }
            catch (const std::ios_base::failure& e) {
                std::cerr << std::format("Error opening log file: {}", e.what()) << std::endl;
            }
        }
    }

public:
    FileLogger(std::filesystem::path filename) : filename(std::move(filename)) {}

    void log(std::string_view url, std::string_view player) {
        ensure_file_open();
        auto now = std::chrono::system_clock::now();
        std::string log_entry = std::format("{:%FT%T} | {} | {}\n", now, url, player);
        try {
            file << log_entry << std::flush;
        }
        catch (const std::ios_base::failure& e) {
            std::cerr << std::format("Error writing to log file: {}", e.what()) << std::endl;
            file.close();
        }
    }

    ~FileLogger() {
        if (file.is_open()) {
            file.close();
        }
    }
};

// Launch player
bool launch_potplayer(const std::string& url) {
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;

    std::string command = config.potplayer_path + " /ADD \"" + url + "\"";
    if (CreateProcessA(NULL, const_cast<char*>(command.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    return false;
}

void add_to_pot_playlist(const std::string& file_path) {
    HWND hwnd = FindWindowA("PotPlayer64", NULL);
    // if player window is not found - launch it and return
    if (!hwnd && !launch_potplayer(file_path)) {
        std::cerr << "Failed to launch PotPlayer" << std::endl;
        return;
    }

    std::wstring wide_path = L"/ADD " + std::wstring(file_path.begin(), file_path.end());
    COPYDATASTRUCT cds;
    cds.dwData = 0x3E9;  // POT_CMD command
    cds.cbData = (wide_path.length() + 1) * sizeof(wchar_t);
    cds.lpData = (PVOID)wide_path.c_str();

    SendMessageW(hwnd, WM_COPYDATA, 0, (LPARAM)&cds);
}

// Function to send an empty JSON response
void send_empty_response() {
    constexpr std::array<char, 6> EMPTY_RESPONSE = {
        2, 0, 0, 0,  // 4-byte integer representing length (2)
        '{', '}'     // 2-byte empty JSON object
    };
    std::cout.write(EMPTY_RESPONSE.data(), EMPTY_RESPONSE.size());
    std::cout.flush();
}

// Entry point
int main() {
    try {
        FileLogger logger(config.log_file);

        uint32_t text_length;
        if (!std::cin.read(reinterpret_cast<char*>(&text_length), sizeof(text_length))) {
            throw std::runtime_error("Failed to read input length");
        }

        std::string text(text_length, '\0');
        if (!std::cin.read(text.data(), text_length)) {
            throw std::runtime_error("Failed to read input text");
        }

        json message = json::parse(text);
        // Send empty JSON response
        send_empty_response();

        // Log and send it to the player
        logger.log(message["url"], "potplayer");
        add_to_pot_playlist(message["url"].get<std::string>());

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        send_empty_response();  // Send empty response even in case of error
        return 1;
    }
}