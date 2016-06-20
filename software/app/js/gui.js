function make_ionic_checkbox(div,value)
{
	if(!div)
		return null;

	var checkbox=
	{
		li:document.createElement("li"),
		label:document.createElement("label"),
		input:document.createElement("input"),
		track:document.createElement("div"),
		handle:document.createElement("div")
	};

	checkbox.li.className="item item-toggle";
	div.appendChild(checkbox.li);

	checkbox.li.appendChild(document.createTextNode(value));

	checkbox.li.appendChild(checkbox.label);
	checkbox.label.className="toggle toggle-assertive";

	checkbox.label.appendChild(checkbox.input);
	checkbox.input.type="input";

	checkbox.label.appendChild(checkbox.track);
	checkbox.track.className="track";

	checkbox.track.appendChild(checkbox.handle);
	checkbox.handle.className="handle";

	return checkbox;
}

function make_ionic_checklist(div)
{
	if(!div)
		return null;
	var ul=document.createElement("ul");
	ul.className="list";
	div.appendChild(ul);
	return ul;
}

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

	this.ui.center=document.createElement("center");
	this.el.appendChild(this.ui.center);

	this.ui.joy=new joy_t(this.ui.center);

	this.ui.log=document.createElement("textarea");
	this.el.appendChild(this.ui.log);
	this.ui.log.readOnly=true;
	this.ui.log.style.resize="none";
	this.ui.log.style.width="100%";
	this.ui.log.style.height="100px";

	this.ui.blade_en=document.getElementById("blade_en_cb");
	this.ui.blade_run=document.getElementById("blade_run_cb");
	if(this.ui.blade_en&&this.ui.blade_run)
	{
		this.ui.blade_en.onchange=function()
		{
			if(!this.checked)
				_this.ui.blade_run.checked=false;
			_this.ui.blade_run.disabled=!this.checked;
		};
	}

	this.el.appendChild(document.createElement("br"));

	this.power_interval=setInterval(function(){_this.pilot();},50);
}

gui_t.prototype.destroy=function()
{
	if(this.network)
		this.network.destroy();
	if(this.power_interval)
	{
		clearInterval(this.power_interval);
		this.power_interval=null;
	}
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

gui_t.prototype.pilot=function()
{
	//this.log("Move: "+this.ui.joy.power.L+" "+this.ui.joy.power.R);
	var pilot=this.ui.joy.power;
	pilot.flags=0;
	if(this.ui.blade_en&&this.ui.blade_en.checked)
		pilot.flags+=1;
	if(this.ui.blade_run&&this.ui.blade_run.checked)
		pilot.flags+=2;
	this.network.send(JSON.stringify(pilot));
}