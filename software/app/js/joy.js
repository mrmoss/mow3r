function joy_t(div,onpower)
{
	if(!div)
		return null;

	this.div=div;
	this.el=document.createElement("div");
	this.div.appendChild(this.el);
	this.el.style.margin=this.el.style.padding="0px";

	this.onpower=onpower;
	this.bg_size=380/3;
	this.hat_size=147/3;
	this.dragging=false;
	this.centered=true;
	this.offset={x:0,y:0};
	this.last_pos={x:0,y:0};

	this.bg=document.createElement("img");
	this.el.appendChild(this.bg);
	this.bg.src="images/joy_bg.png";
	this.bg.style.margin=this.bg.style.padding="0px";
	this.bg.style.width=this.bg.style.height=this.bg_size+"px";

	this.hat=document.createElement("img");
	this.el.appendChild(this.hat);
	this.hat.src="images/joy_hat.png";
	this.hat.style.visibility="hidden";
	this.hat.style.margin=this.hat.style.padding="0px";
	this.hat.style.width=this.hat.style.height=this.hat_size+"px";
	this.hat.style.position="absolute";

	var _this=this;
	this.hat.onmousedown=function(event)
	{
		_this.dragging=true;
		_this.centered=false;
		_this.offset=
		{
			x:event.pageX-_this.el.offsetLeft-_this.bg_size/2,
			y:event.pageY-_this.el.offsetTop-_this.bg_size/2
		};
		_this.move(event.pageX-_this.offset.x,event.pageY-_this.offset.y);
	};
	document.addEventListener("mousemove",function(event)
	{
		if(_this.dragging)
		{
			_this.centered=false;
			_this.move(event.pageX-_this.offset.x,event.pageY-_this.offset.y);
		}
	});
	document.addEventListener("mouseup",function()
	{
		_this.dragging=false;
		_this.offset={x:0,y:0};
	});
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
	console.log(dir*180/Math.PI);
	var max_x=Math.abs(Math.cos(dir)*max_len);
	var max_y=Math.abs(Math.sin(dir)*max_len);
	x=this.clamp_mag(x,max_x);
	y=this.clamp_mag(y,max_y);
	var pt=Math.floor(x/max_len*100);
	var pf=Math.floor(-y/max_len*100);
	var power={L:this.clamp_mag(pf+pt,100),R:this.clamp_mag(pf-pt,100)};
	if(!this.centered&&power.L==0&&power.R==0)
		this.centered=true;
	if(this.onpower)
		this.onpower(power);
	this.hat.style.left=(this.bg_size-this.hat_size)/2+this.el.offsetLeft+x+"px";
	this.hat.style.top=(this.bg_size-this.hat_size)/2+this.el.offsetTop+y+"px";
}