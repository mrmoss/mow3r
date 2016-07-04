function onlaunch()
{
	var width=480;
	var height=640;

	chrome.app.window.create('/window.html',
	{
		'innerBounds':
		{
			'width':width,'height':height,
			'maxWidth':width,'maxHeight':height,
			'minWidth':width,'minHeight':height
		},
		id:'main'
	});
}

chrome.app.runtime.onLaunched.addListener(onlaunch);