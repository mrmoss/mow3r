function onlaunch()
{
	var width=600;
	var height=400;

	chrome.app.window.create('/window.html',
	{
		'innerBounds':
		{
			'width':width,'height':height,
		//	'maxWidth':width,'maxHeight':height,
			'minWidth':width,'minHeight':height
		},
		id:'main'
	});

	chrome.sockets.udp.getSockets(function(socketInfos)
	{
		for(var key in socketInfos)
			chrome.sockets.udp.close(socketInfos[key].socketId);
	});
}

chrome.app.runtime.onLaunched.addListener(onlaunch);