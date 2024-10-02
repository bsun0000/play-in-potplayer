# play-in-potplayer
A Chrome extension and Native Messaging host program to pass URL links to the PotPlayer media player.

## Overview

This project uses the Native Messaging API to interface (via host program) with the PotPlayer program. The host program can:

1. Run the player (if not running) with the `/ADD` parameter, or
2. Send the CMD command directly to the existing player instance - this is much faster than running the player just to add a new URL to the existing window.

The developer of PotPlayer decided not to add this functionality to the main executable file, for some reason. Instead, it resides in PotPlayer64.dll, which is protected by a heavy virtualizer and is relatively slow to launch, especially if your system is loaded with heavy background tasks.

## How to Use

1. Download both archives from the release page and unpack them.
2. Go to the Extensions page in your Chrome browser.
3. Enable "Developer mode" by ticking the checkbox in the upper-right corner.
4. Click on the "Load unpacked extension..." button.
5. Select the directory containing the unpacked play-in-potplayer extension.
6. Copy the identifier of the extension (e.g., "ppmmpfakebjkoblnfddpkmceigomppph").

### Host Program Configuration

1. Go to the Host program folder and open `config.json` file:

```json
{
    "log_file": "C:\\Temp\\url_log.txt",
    "potplayer_path": "C:\\Program Files\\DAUM\\PotPlayer\\PotPlayerMini64.exe"
}
```

- `log_file`: Path to the log file. Every time you add a URL to the player, it will be stored here.
- `potplayer_path`: Path to PotPlayer. Can be relative if the player is added to the PATH environment variable in your system.

2. Open the `manifest.json` file:

```json
{
  "name": "play_in_potplayer",
  "description": "description",
  "path": "PlayInPotplayer.exe",
  "type": "stdio",
  "allowed_origins": ["chrome-extension://ppmmpfakebjkoblnfddpkmceigomppph/"]
}
```

- `allowed_origins`: Insert the extension identifier here, otherwise it will not work.
- `path`: You don't need to edit this. Host program should be near the manifest file OR it should be in your PATH

### Registry Configuration

1. Open the registry editor (regedit or similar).
2. Navigate to `HKEY_LOCAL_MACHINE\SOFTWARE\Google\Chrome`.
3. Add `NativeMessagingHosts` section (if missing).
4. Inside it, add `play_in_potplayer` section, and set the default value to the path containing the Host program manifest file.
   - Example: `C:\Installed\NativeMessageHosts\PlayInPot\manifest.json`

You should now have a fully working context menu item in your browser: "Play with PotPlayer".

## Additional Configuration

If you experience any issues or delays:

1. Go to `chrome://flags` (or `vivaldi://flags`).
2. Set "Force Native Host Executables to Launch Directly" to Enabled.
`#launch-windows-native-hosts-directly`
3. Restart the browser.

## Note

I have no plans to create easy-to-use installers or publish this extension in the Chrome Web Store.
However, you're welcome to fork this repository and do whatever you want with it.
