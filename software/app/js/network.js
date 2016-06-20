function network_t(tx,rx,auth,onerror,onreceive)
{
	if(!tx||!rx)
		return null;
	this.tx=tx;
	this.rx=rx;
	this.auth=auth;
	this.onerror=onerror;
	this.onreceive=onreceive;
	this.connected=false;

	if(!this.onerror)
		this.onerror=function(error)
		{
			throw error;
		};


	this.socket=null;
	this.group="224.0.0.1";
	var _this=this;
	chrome.sockets.udp.create({persistent:true},function(createInfo)
	{
		_this.create_cb_m(createInfo);
	});
	chrome.sockets.udp.onReceive.addListener(function(info)
	{
		_this.recv_cb_m(info);
	});
}

network_t.prototype.destroy=function()
{
	this.close();
}

network_t.prototype.send=function(data)
{
	if(this.connected)
	{
		var _this=this;
		data=CryptoJS.HmacSHA256(data,this.auth)+data;
		chrome.sockets.udp.send(this.socket.socketId,this.str2ab_m(data),
			this.group,this.tx,function(sendInfo)
			{
				_this.send_cb_m(sendInfo,data);
			});
	}
}

network_t.prototype.close=function()
{
	if(this.socket&&this.socket.socketId)
		chrome.sockets.udp.close(this.socket.socketId);
	this.socket=null;
	this.connected=false;
}


network_t.prototype.ab2str_m=function(buf)
{
	return String.fromCharCode.apply(null,new Uint8Array(buf));
}

network_t.prototype.str2ab_m=function(str)
{
	var buf=new ArrayBuffer(str.length);
	var view=new Uint8Array(buf);
	for(var ii=0;ii<str.length;++ii)
		view[ii]=str.charCodeAt(ii);
	return buf;
}

network_t.prototype.create_cb_m=function(createInfo)
{
	var _this=this;
	this.error_check_m("create");
	this.socket=createInfo;
	chrome.sockets.udp.setMulticastLoopbackMode(this.socket.socketId,true,function(result)
	{
		_this.error_check_m("mode",result);
		chrome.sockets.udp.bind(_this.socket.socketId,"0.0.0.0",_this.rx,function(result)
		{
			//_this.error_check_m("bind",result);
			//chrome.sockets.udp.setBroadcast(_this.socket.socketId,true,function(result)
			//{
				//_this.error_check_m("broadcast",result);
				chrome.sockets.udp.joinGroup(_this.socket.socketId,_this.group,function(result)
				{
					_this.error_check_m("join",result);
					_this.connected=true;
				});
			//});
		});
	});
}

network_t.prototype.recv_cb_m=function(info)
{
	this.error_check_m("recv");
	if(this.onreceive)
	{
		var auth=false;
		var data=this.ab2str_m(info.data);
		if(data.length>=64)
		{
			var hash=data.substring(0,64);
			data=data.substring(64,data.length);
			auth=(CryptoJS.HmacSHA256(data,this.auth)==hash);
		}
		this.onreceive(data,auth);
	}
}

network_t.prototype.send_cb_m=function(sendInfo,data)
{
	this.error_check_m("send");
	if(data.length!=sendInfo.bytesSent&&this.onerror)
		this.onerror("Send: Error sending data ("+sendInfo.bytesSent+"/"+data.length+" sent).");
}

network_t.prototype.error_check_m=function(state,result)
{
	if(!result)
		result=0;
	if(chrome&&chrome.runtime&&chrome.runtime.lastError&&this.onerror)
		this.onerror(state+": "+chrome.runtime.lastError.message);
	if(result<0&&this.onerror)
		this.onerror(state+": Error code "+result);
}