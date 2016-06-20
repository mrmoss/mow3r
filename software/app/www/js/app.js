var div;
var gui;

angular.module('starter', ['ionic']).run(function($ionicPlatform)
{
	$ionicPlatform.ready(function()
	{
		if(window.cordova&&window.cordova.plugins.Keyboard)
		{
			cordova.plugins.Keyboard.hideKeyboardAccessoryBar(true);
			cordova.plugins.Keyboard.disableScroll(true);
		}
		if(window.StatusBar)
			StatusBar.styleDefault();

		chrome.sockets.udp.getSockets(function(socketInfos)
		{
			for(var key in socketInfos)
				chrome.sockets.udp.close(socketInfos[key].socketId);
		});

		div=document.getElementById("content");
		gui=new gui_t(div);
	});
});
