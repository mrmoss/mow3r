function gui_t(div)
{
	if(!div)
		return null;

	this.div=div;
	this.el=document.createElement("div");
	this.div.appendChild(this.el);

	var _this=this;

	this.network=new network_t(8080,8081,"auth123",
		function(error){_this.error(error);},
		function(data,auth)
		{
			if(auth)
				_this.log("RX: "+data);
			else
				_this.log("Authentication mismatch.");
		});
	this.ui={};

	this.ui.joy=new joy_t(this.el,function(power){_this.pilot(power);});

	this.ui.log=document.createElement("textarea");
	this.el.appendChild(this.ui.log);
	this.ui.log.readOnly=true;
	this.ui.log.style.resize="none";
	this.ui.log.style.width="100%";
	this.ui.log.style.height="200px";

	this.el.appendChild(document.createElement("br"));
}

gui_t.prototype.destroy=function()
{
	if(this.network)
		this.network.destroy();
}

gui_t.prototype.log=function(message)
{
	if(message.length>0)
	{
		this.ui.log.value+=message;
		if(message[message.length-1]!="\n")
			this.ui.log.value+="\n";
		this.ui.log.scrollTop=this.ui.log.scrollHeight;
	}
}

gui_t.prototype.error=function(error)
{
	this.log(error);
	throw error;
}

gui_t.prototype.pilot=function(power)
{
	this.log("Move: "+power.x+" "+power.y);
	this.network.send(JSON.stringify(power));
}