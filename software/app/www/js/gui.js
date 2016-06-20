function gui_t(div)
{
	if(!div)
		return null;

	this.div=div;
	this.el=document.createElement("div");
	this.div.appendChild(this.el);

	var _this=this;
	this.status={heartbeat:0,ardubeat:0};

	this.network=new network_t(8080,8081,"auth123",
		function(error){_this.error(error);},
		function(data,auth)
		{
			if(auth)
			{
				try
				{
					data=JSON.parse(data);
					if(data.heartbeat)
						_this.status.heartbeat=data.heartbeat;
					if(data.ardubeat)
						_this.status.ardubeat=data.ardubeat;
				}
				catch(error)
				{}
				return;
			}
			_this.log("Authentication mismatch.");
		});

	this.ui={};

	this.ui.center=document.createElement("center");
	this.el.appendChild(this.ui.center);

	this.ui.joy=new joy_t(this.ui.center);
	this.ui.joy.disabled=true;

	this.ui.heartbeat=
	{
		good:document.getElementById("heartbeat_good"),
		bad:document.getElementById("heartbeat_bad"),
		count:-1,
		connected:false
	};
	this.ui.ardubeat=
	{
		good:document.getElementById("ardubeat_good"),
		bad:document.getElementById("ardubeat_bad"),
		count:-1,
		connected:false
	};

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

	this.power_interval=setInterval(function()
	{
		_this.pilot();
	},50);
	this.beat_interval=setInterval(function()
	{
		_this.set_beat(_this.ui.heartbeat,_this.status.heartbeat);
		_this.set_beat(_this.ui.ardubeat,_this.status.ardubeat);
	},1500);
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
	if(this.beat_interval)
	{
		clearInterval(this.beat_interval);
		this.beat_interval=null;
	}
}

gui_t.prototype.error=function(error)
{
	throw error;
}

gui_t.prototype.pilot=function()
{
	var pilot=this.ui.joy.power;
	pilot.flags=0;
	if(this.ui.blade_en&&this.ui.blade_en.checked)
		pilot.flags+=1;
	if(this.ui.blade_run&&this.ui.blade_run.checked)
		pilot.flags+=2;
	if(this.ui.heartbeat.connected&&this.ui.ardubeat.connected)
		this.network.send(JSON.stringify(pilot));
}

gui_t.prototype.set_beat=function(beat,new_count)
{
	if(beat)
	{
		var old_count=beat.count;
		beat.connected=(new_count!=beat.count);
		beat.count=new_count;
		if(old_count!=-1)
			this.update_beat_ui(beat);
	}
}

gui_t.prototype.update_beat_ui=function(beat)
{
	if(beat)
	{
		if(beat.connected)
		{
			if(beat.bad)
				beat.bad.style.visibility="hidden";
			if(beat.good)
				beat.good.style.visibility="visible";
			this.ui.blade_en.disabled=false;
			this.ui.joy.disabled=false;
		}
		else
		{
			if(beat.bad)
				beat.bad.style.visibility="visible";
			if(beat.good)
				beat.good.style.visibility="hidden";
			this.ui.blade_en.checked=false;
			this.ui.blade_en.disabled=true;
			this.ui.blade_run.checked=false;
			this.ui.blade_run.disabled=true;
			this.ui.joy.disabled=true;
		}

	}
}