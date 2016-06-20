function joy_t(div)
{
	if(!div)
		return null;

	this.div=div;
	this.el=document.createElement("div");
	this.div.appendChild(this.el);
	this.el.style.margin=this.el.style.padding="0px";

	this.bg_size=380/1.5;
	this.hat_size=147/1.5;
	this.dragging=false;
	this.centered=true;
	this.offset={x:0,y:0};
	this.last_pos={x:0,y:0};
	this.power={L:0,R:0};

	this.bg=document.createElement("img");
	this.el.appendChild(this.bg);
	this.bg.src="img/joy_bg.png";
	this.bg.style.margin=this.bg.style.padding="0px";
	this.bg.style.width=this.bg.style.height=this.bg_size+"px";

	this.hat=document.createElement("img");
	this.el.appendChild(this.hat);
	this.hat.src="img/joy_hat.png";
	this.hat.style.visibility="hidden";
	this.hat.style.margin=this.hat.style.padding="0px";
	this.hat.style.width=this.hat.style.height=this.hat_size+"px";
	this.hat.style.position="absolute";

	var _this=this;
	var down=function(event)
	{
		_this.dragging=true;
		_this.centered=false;
		if(!event.pageX)
			event.pageX=event.touches[0].pageX;
		if(!event.pageY)
			event.pageY=event.touches[0].pageY;
		_this.offset=
		{
			x:event.pageX-_this.el.offsetLeft-_this.bg_size/2,
			y:event.pageY-_this.el.offsetTop-_this.bg_size/2
		};
		_this.move(event.pageX-_this.offset.x,event.pageY-_this.offset.y);
	};
	var move=function(event)
	{
		if(!event.pageX)
			event.pageX=event.touches[0].pageX;
		if(!event.pageY)
			event.pageY=event.touches[0].pageY;
		if(_this.dragging)
		{
			_this.centered=false;
			_this.move(event.pageX-_this.offset.x,event.pageY-_this.offset.y);
		}
	};
	var release=function()
	{
		_this.dragging=false;
		_this.offset={x:0,y:0};
	};
	this.hat.addEventListener("mousedown",down);
	this.hat.addEventListener("touchstart",down);
	document.addEventListener("mousemove",move);
	document.addEventListener("touchmove",move);
	document.addEventListener("mouseup",release);
	document.addEventListener("touchend",release);
	setTimeout(function()
	{
		_this.move(_this.bg_size/2,_this.bg_size/2);
		_this.hat.style.visibility="visible";
	},100);
	_this.anim_interval=setInterval(function()
		{
			if(!_this.dragging&&!_this.centered)
			{
				var new_pos=
				{
					x:(_this.last_pos.x-_this.bg_size/2)*0.8+_this.bg_size/2,
					y:(_this.last_pos.y-_this.bg_size/2)*0.8+_this.bg_size/2
				};
				if(Math.abs(new_pos.x-_this.bg_size/2)<1)
					new_pos.x=_this.bg_size/2;
				if(Math.abs(new_pos.y-_this.bg_size/2)<1)
					new_pos.y=_this.bg_size/2;
				_this.move(new_pos.x,new_pos.y);
			}
		},10);
}

joy_t.prototype.destroy=function()
{
	this.div.removeChild(this.el);
	this.clearInterval(this.anim_interval);
}

joy_t.prototype.clamp=function(val,min,max)
{
	if(val>max)
		val=max;
	if(val<min)
		val=min;
	return val;
}

joy_t.prototype.clamp_mag=function(val,mag)
{
	return this.clamp(val,-mag,mag);
}

joy_t.prototype.move=function(x,y)
{
	this.last_pos={x:x,y:y};
	x-=this.bg_size/2;
	y-=this.bg_size/2;
	var max_len=this.bg_size/2*0.6;
	var dir=Math.atan2(-y,x);
	var max_x=Math.abs(Math.cos(dir)*max_len);
	var max_y=Math.abs(Math.sin(dir)*max_len);
	x=this.clamp_mag(x,max_x);
	y=this.clamp_mag(y,max_y);
	var pt=Math.floor(x/max_len*100);
	var pf=Math.floor(-y/max_len*100);
	this.power={L:this.clamp_mag(pf+pt,100),R:this.clamp_mag(pf-pt,100)};
	if(!this.centered&&this.power.L==0&&this.power.R==0)
		this.centered=true;
	this.hat.style.left=(this.bg_size-this.hat_size)/2+this.bg.offsetLeft+x+"px";
	this.hat.style.top=(this.bg_size-this.hat_size)/2+this.bg.offsetTop+y+"px";
}