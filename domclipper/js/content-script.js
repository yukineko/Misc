window.addEventListener('keydown', function(e){
    if(e.metaKey && e.keyCode == 13){
	var selection = window.getSelection()
	console.log(selection)
	var nodestack = []
	if(selection.rangeCount == 1){
	    var range = selection.getRangeAt(0)
	    var ancestor = range.commonAncestorContainer
	    var parent = ancestor
	    while((parent = parent.parentNode) != null){
		nodestack.push()
	    }
	    text = selection.toString()
	    console.log(text)
	    chrome.extension.sendMessage({ text: text }, function(response) {})
	} else if(selection.rangeCount > 1){
	    var text = selection.getRangeAt(0).parentElement().innerHTML
	    console.log(text)
	    chrome.extension.sendMessage({ text: text }, function(response) {})	    
	}
    }
})
