chrome.extension.onMessage.addListener(function(req, sender, callback) {
  if (sender.tab) { // from a content script
    if (req.text !== undefined) {
	var properties = { url: "./html/elements.html?text=" + req.text };
      chrome.tabs.create(properties, function(tab) {});
    }
  }
});
