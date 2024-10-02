chrome.runtime.onInstalled.addListener(() => {
  chrome.contextMenus.create({
    id: "bSun0000.play-external-potplayer",
    title: "Play with PotPlayer",
    contexts: ["link", "video", "audio"]
  });
  
  // Set up an alarm to wake up the service worker periodically
  chrome.alarms.create('keepAlive', { periodInMinutes: 0.1 });
});

chrome.contextMenus.onClicked.addListener((info, tab) => {
	const _url = info.linkUrl || info.srcUrl || info.pageUrl;
	const message = { url: _url };
	try {
		chrome.runtime.sendNativeMessage("play_in_potplayer", message, response => {
			const lasterror = chrome.runtime.lastError;
		});
	} catch (err) {}
});

chrome.alarms.onAlarm.addListener((alarm) => {
  if (alarm.name === 'keepAlive') {
	  chrome.runtime.getPlatformInfo();
  }
});