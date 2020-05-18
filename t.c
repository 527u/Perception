#include <reg51.h>
#include <intrins.h>
#include <stdio.h>
#define uchar unsigned char
#define uint unsigned int
#define out P1
sbit L1 = P2^0;
sbit L2 = P2^1;
sbit L3 = P2^2;
sbit L4 = P2^3;
sbit RS=P3^2;
sbit RW=P3^1;
sbit E=P3^0;
sbit DQ=P3^3;
sbit DQ2=P3^4;
sbit sound = P3^5;
uchar keyval;

void delay1 (time);
void delay2(uint j);
void led_delay(void);
void forward(void);
void backward(void);	 
void alter(void);
void flash(void);
void key_scan(void);
void check_busy(void);
void write_command(uchar com);
void write_data(uchar dat);
void string(uchar ad,uchar *s);
void lcd_initial(void);
void init(void);
void initsound(void);
void delay5(uchar n);
void init_ds18b20(void);
void init_humds18b20(void);
uchar readbyte(void);
uchar readhumbyte(void);
void writebyte(uchar dat);
void writehumbyte(uchar dat);
float retemp(void);
float rehum(void);
void temperature(void);
void humidity(void);
void monophonic(void);
void monophonic2(void);
void monophonic3(void);
void monprint(void);
void monprint2(void);
void monprint3(void);

//主程序
void main ()
{	
	keyval = 7;
	lcd_initial();
	while (1)
	{
		key_scan();
		switch(keyval)
		{
			case 0:
				forward();
				break;
			case 1:
				backward();
				break;
			case 2:
				alter();
				break;
			case 3:
				flash();
				break;
			case 4:
				temperature();
				break;
			case 5:
				humidity();
				break;
			case 6:
				initsound();
				while(keyval != 15)
				{
					key_scan();
					switch(keyval)
					{
						case 0:	
							monprint();
							monophonic();
							break;
						case 1:	
							monprint2();
							monophonic2();
							break;
						case 2:
							monprint3();	
							monophonic3();
							break;
						case 14:
							initsound();
							keyval = 7;
							break;
					}
				}				
				break;
			case 15:
				init();
				keyval = 7;
				break;	
		}
	}
}


void delay1 (time)//延时子程序1
{
	uint i;
	for (i = 0; i < time; i++)
	{}
}

void delay2(uint j)//延时子程序2
{
	uchar i = 250;
	for(;j>0;j--)
	{
		while(--i);
		i=249;
		while(--i);
		i=250;
	}
}

void led_delay(void)  //流水灯延时函数
{
	unsigned char i, j;
	for (i = 0; i < 220; i++)
		for (j = 0; j < 220; j++)
		;
}

void forward(void)	//正向流水点亮LED函数
{
	write_command(0x01);//清屏
 	string(0x80,"0 forward");
	string(0xC0,"15 quit");
	P0 = 0xfe;
	led_delay();
	P0 = 0xfd;
	led_delay();
	P0 = 0xfb;
	led_delay();
	P0 = 0xf7;
	led_delay();
	P0 = 0xef;
	led_delay();
	P0 = 0xdf;
	led_delay();
	P0 = 0xbf;
	led_delay();
	P0 = 0x7f;
	led_delay();
}

void backward(void)	  //反向流水点亮LED函数
{
	write_command(0x01);//清屏
 	string(0x80,"1 backward");
	string(0xC0,"15 quit");
	P0 = 0x7f;
	led_delay(); 
	P0 = 0xbf;
	led_delay(); 
	P0 = 0xdf;
	led_delay();  
	P0 = 0xef;
	led_delay();   
	P0 = 0xf7;
	led_delay(); 
	P0 = 0xfb;
	led_delay();  
	P0 = 0xfd;
	led_delay();
	P0 = 0xfe;
	led_delay();	
}

void alter(void)   //交替点亮高4位与低4位LED的函数
{
	write_command(0x01);//清屏
 	string(0x80,"2 alter");
	string(0xC0,"15 quit");
	P0 = 0x0f;
	led_delay();
	P0 = 0xf0;
	led_delay();
}

void flash(void)   //闪烁点亮LED的函数
{
	write_command(0x01);//清屏
 	string(0x80,"3 flash");
	string(0xC0,"15 quit");
	P0 = 0xff;
	led_delay();
	P0 = 0x00;
	led_delay();
}

void key_scan(void)//键盘扫描
{
	uchar temp11;
	uchar i;
	P2 = 0xef;	 //行扫描初值
	for (i = 0; i <= 3; i++)   //按键阵行扫描，一共四行
	{
		if (L1 == 0) keyval = i * 4 + 0;
		if (L2 == 0) keyval = i * 4 + 1;
		if (L3 == 0) keyval = i * 4 + 2;
		if (L4 == 0) keyval = i * 4 + 3;
		delay1(500);
		temp11 = P2;
		temp11 = temp11 | 0x0f;
		temp11 = temp11 << 1;
		temp11 = temp11 | 0x0f;
		P2 = temp11;	 //下一行行扫描的值，将P2.0-P2.3的值左移一位
	}
}

void check_busy(void)//查忙程序
{
	uchar dt;
	do
	{
		dt=0xff;
		E=0;
		RS=0;
		RW=1;
		E=1;
		dt=out;
	}while(dt&0x80);
	E=0;
}

void write_command(uchar com)//写控制指令
{
	check_busy();
	E=0;
	RS=0;
	RW=0;
	out=com;
	E=1;
	_nop_();
	E=0;
	delay2(1);
}

void write_data(uchar dat)//输出字符串
{
	check_busy();
	E=0;
	RS=1;
	RW=0;
	out=dat;
	E=1;
	_nop_();
	E=0;
	delay2(1);
}

void string(uchar ad,uchar *s)//输出字符串
{
	write_command(ad);
	while(*s>0)
	{
		write_data(*s++);
		delay2(1);
	}
}

void lcd_initial(void)//液晶屏初始化
{
	write_command(0x38);//8位总线，双行显示，5x7的点阵字符
	write_command(0x0C);//开整体显示，光标关，无黑块
	write_command(0x06);//光标右移
	init();
	delay2(1);
}

void init(void)//主菜单
{
	write_command(0x01);//清屏
	string(0x80,"0zx 1fx 2ss 3jt");
	string(0xC0,"4tem 5hum 6sound");
}

void initsound(void)
{
	write_command(0x01);//清屏
	string(0x80,"0 500Hz 1 1kHz");
	string(0xC0,"2 2kHz 15 quit");
}				

void delay5(uchar n)//精确延时5μs子程序
{
	do
	{
		_nop_();
		_nop_();
		_nop_();
		n--;
	}
	while(n);
} 

void init_ds18b20(void)//初始化函数
{
	uchar x=0;
	DQ=0;
	delay5(120);
	DQ=1;
	delay5(16);
	delay5(80);
} 

void init_humds18b20(void)//初始化函数
{
	uchar x=0;
	DQ2=0;
	delay5(120);
	DQ2=1;
	delay5(16);
	delay5(80);
} 

uchar readbyte(void)//读取一字节函数
{
	uchar i=0;
	uchar date=0;
	for(i=8;i>0;i--)
	{
		DQ=0;
		delay5(1);
		DQ=1;  //15μs内拉释放总线
		date>>=1;
		if(DQ)
		date|=0x80;
		delay5(11);
	}
	return(date);
} 

uchar readhumbyte(void)//读取一字节函数
{
	uchar i=0;
	uchar date=0;
	for(i=8;i>0;i--)
	{
		DQ2=0;
		delay5(1);
		DQ2=1;  //15μs内拉释放总线
		date>>=1;
		if(DQ2)
		date|=0x80;
		delay5(11);
	}
	return(date);
} 

void writebyte(uchar dat)//写一字节函数  
{
	uchar i=0;
	for(i=8;i>0;i--)
	{
		DQ=0;
		DQ=dat&0x01;  //写“1”在15μs内拉低 
		delay5(12);  //写“0”拉低60μs 
		DQ=1;
		dat>>=1;
		delay5(5);
	}
}

void writehumbyte(uchar dat)//写一字节函数  
{
	uchar i=0;
	for(i=8;i>0;i--)
	{
		DQ2=0;
		DQ2=dat&0x01;  //写“1”在15μs内拉低 
		delay5(12);  //写“0”拉低60μs 
		DQ2=1;
		dat>>=1;
		delay5(5);
	}
}

float retemp(void)//读取温度函数
{
	//uchar a,b,tt;
	uchar a,b;
	float f_tt;

	uint t;
	init_ds18b20();
	writebyte(0xCC);
	writebyte(0x44);
	init_ds18b20();
	writebyte(0xCC);
	writebyte(0xBE);

	a=readbyte();
	b=readbyte();
	t=b;
	t<<=8;
	t=t|a;
	f_tt=t*0.0625;  //即除16
	return(f_tt); 
}

float rehum(void)//读取湿度函数
{
	//uchar a,b,tt;
	uchar a,b;
	float f_tt;

	uint t;
	init_humds18b20();
	writehumbyte(0xCC);
	writehumbyte(0x44);
	init_humds18b20();
	writehumbyte(0xCC);
	writehumbyte(0xBE);

	a=readhumbyte();
	b=readhumbyte();
	t=b;
	t<<=8;
	t=t|a;
	f_tt=t*0.0625;  //即除16
	return(f_tt); 
}  

void temperature(void)//温度显示函数
{
	uchar wdstr[17];
	float temp;
	write_command(0x01);//清屏
	temp = retemp();
	if (temp>28||temp<10)
		monophonic();
	sprintf(wdstr, "Temperature:%.1f", temp);
	string(0x80, wdstr);   //显示第1行字符串
	string(0xC0,"4 temp 15 quit");
	delay2(100);		 //延时delay2(100);		 //延时
}

void humidity(void)//湿度显示函数
{
	uchar wdstr[17];
	float hum;
	write_command(0x01);//清屏
	hum = rehum();
	if (hum>70||hum<40)
		monophonic2();
	sprintf(wdstr, "Humidity: %.1f", hum);
	string(0x80, wdstr);   //显示第1行字符串
	string(0xC0,"5 hum 15 quit");		
	delay2(100);		 //延时delay2(100);		 //延时
}

void monprint(void)
{
	write_command(0x01);//清屏
 	string(0x80,"6.0 monophonic");
	string(0xC0,"500Hz  14 quit");		
}

void monophonic(void)//单音
{
	int num = 255;		 
	TMOD = 0x01;	 //设置T0为方式1
	TR0 = 1;		 //接通T0
	while(num)		 //无限循环
	{
		TH0 = 0xfc;	  //T0高8位置初值
		TL0 = 0x18;	  //T0低8位置初值
		do{}while(!TF0);
		sound = !sound; //查询TF0标志是否为1，如为1，说明T0溢出，则P3.5的状态求反
		TF0 = 0;	  //TF0标志重新置零
		num--;
	}
}

void monprint2(void)
{
	write_command(0x01);//清屏
 	string(0x80,"6.1 monophonic");
	string(0xC0,"1kHz   14 quit");	
}

void monophonic2(void)//单音
{
	int num = 255;
	TMOD = 0x01;	 //设置T0为方式1
	TR0 = 1;		 //接通T0
	while(num)		 //无限循环
	{
		TH0 = 0xfe;	  //T0高8位置初值
		TL0 = 0x0c;	  //T0低8位置初值
		do{}while(!TF0);
		sound = !sound; //查询TF0标志是否为1，如为1，说明T0溢出，则P3.5的状态求反
		TF0 = 0;	  //TF0标志重新置零
		num--;
	}
}

void monprint3(void)
{
	write_command(0x01);//清屏
 	string(0x80,"6.2 monophonic");
	string(0xC0,"2kHz   14 quit");	
}

void monophonic3(void)//单音
{
	int num = 255;	
	TMOD = 0x01;	 //设置T0为方式1
	TR0 = 1;		 //接通T0
	while(num)		 //无限循环
	{
		TH0 = 0xff;	  //T0高8位置初值
		TL0 = 0x06;	  //T0低8位置初值
		do{}while(!TF0);
		sound = !sound; //查询TF0标志是否为1，如为1，说明T0溢出，则P3.5的状态求反
		TF0 = 0;	  //TF0标志重新置零
		num--;
	}
}