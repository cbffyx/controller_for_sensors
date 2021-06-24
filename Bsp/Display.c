#include "main.h"
#include "Display.h"
#include "Ap_29demo.h"	
#include "qr_encode.h"
#include "qr_consts.h"

void driver_delay_xms(unsigned long xms)	
{
	unsigned long i = 0 , j=0;

	for(j=0;j<xms;j++)
	{
		for(i=0; i<256*5; i++);
//		for(i=0; i<128; i++);
	}
}

void DisplayGPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = EPD_W21_RST_Pin;    //abcd OE ST LT0输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//
  GPIO_Init(EPD_W21_RST_Port, &GPIO_InitStructure);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = EPD_W21_MOSI_Pin | EPD_W21_CLK_Pin |EPD_W21_CS_Pin |EPD_W21_DC_Pin;    //abcd OE ST LT0输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = EPD_W21_BUSY_Pin;    //abcd OE ST LT0输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(EPD_W21_BUSY_Port, &GPIO_InitStructure);
}

void Epaper_Spi_WriteByte(unsigned char TxData)
{				   			 
	unsigned char TempData;
	unsigned char scnt;
	TempData=TxData;

  EPD_W21_CLK_0;  
	for(scnt=0;scnt<8;scnt++)
	{ 
		if(TempData&0x80)
		  EPD_W21_MOSI_1 ;
		else
		  EPD_W21_MOSI_0 ;
		EPD_W21_CLK_1;  
	  EPD_W21_CLK_0;  
		TempData=TempData<<1;
  }
}

/*
void Epaper_Spi_ReadByte(unsigned char *RxData)
{				   			 
	unsigned char TempData;
	unsigned char scnt;
	TempData=TxData;

  EPD_W21_CLK_0;  
	for(scnt=0;scnt<8;scnt++)
	{ 
		if(TempData&0x80)
		  EPD_W21_MOSI_1 ;
		else
		  EPD_W21_MOSI_0 ;
		EPD_W21_CLK_1;  
	  EPD_W21_CLK_0;  
		TempData=TempData<<1;
  }
}
*/


void Epaper_READBUSY(void)
{ 
  while(1)
  {	 //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
  }  
}



void Epaper_Write_Command(unsigned char cmd)
{
	EPD_W21_CS_1;
	EPD_W21_CS_0;
	EPD_W21_DC_0;  // D/C#   0:command  1:data

	Epaper_Spi_WriteByte(cmd);
	EPD_W21_CS_1;
}



void Epaper_Write_Data(unsigned char data)
{
	EPD_W21_CS_1;
	EPD_W21_CS_0;
	EPD_W21_DC_1;  // D/C#   0:command  1:data

	Epaper_Spi_WriteByte(data);
	EPD_W21_CS_1;
}


/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void EPD_HW_Init(void)
{
	EPD_W21_RST_0;     
	driver_delay_xms(100); 
	EPD_W21_RST_1; //hard reset  
	driver_delay_xms(100); 

	Epaper_READBUSY();
	Epaper_Write_Command(0x12); // soft reset
	Epaper_READBUSY();

	Epaper_Write_Command(0x74); //set analog block control       
	Epaper_Write_Data(0x54);
	Epaper_Write_Command(0x7E); //set digital block control          
	Epaper_Write_Data(0x3B);

	Epaper_Write_Command(0x01); //Driver output control      
	Epaper_Write_Data(0x27);
	Epaper_Write_Data(0x01);
	Epaper_Write_Data(0x00);

	Epaper_Write_Command(0x11); //data entry mode       
	Epaper_Write_Data(0x01);

	Epaper_Write_Command(0x44); //set Ram-X address start/end position   
	Epaper_Write_Data(0x01);
	Epaper_Write_Data(0x10);    //0x0C-->(15+1)*8=128

	Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
	Epaper_Write_Data(0x27);   //0xF9-->(249+1)=250
	Epaper_Write_Data(0x01);
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0x00); 

	Epaper_Write_Command(0x3C); //BorderWavefrom
	Epaper_Write_Data(0x01);	

  Epaper_Write_Command(0x18); 
	Epaper_Write_Data(0x80);	

  Epaper_Write_Command(0x22); // //Load Temperature and waveform setting.
	Epaper_Write_Data(0XB1);	
  Epaper_Write_Command(0x20); 

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(0x01);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
	Epaper_Write_Data(0x27);
	Epaper_Write_Data(0x01);
	Epaper_READBUSY();
	
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_ALL_image(const unsigned char *datas1,const unsigned char *datas2)
{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
    for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
			Epaper_Write_Data(~(*datas1)); 
			datas1++;
   }	
    Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)
    for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
			Epaper_Write_Data(*datas2); 
			datas2++;
   }
   EPD_Update();		 
}



/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{   
  Epaper_Write_Command(0x22); 
  Epaper_Write_Data(0xC7);   
  Epaper_Write_Command(0x20); 
//  Epaper_READBUSY();   
}



void EPD_DeepSleep(void)
{  	
  Epaper_Write_Command(0x10); //enter deep sleep
  Epaper_Write_Data(0x01); 
  driver_delay_xms(100);
}



/////////////////////////////////Single display////////////////////////////////////////////////
void EPD_WhiteScreen_Red(void)

{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
  {
			Epaper_Write_Data(0xff);
  }
	
    Epaper_Write_Command(0x26);   //write RAM for red(1)/white (0)
	for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
  {
			Epaper_Write_Data(0xff);
  }
	
//	EPD_Update();
}



void EPD_WhiteScreen_Black(void)

{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
  {
			Epaper_Write_Data(0x00);
  }
	
    Epaper_Write_Command(0x26);   //write RAM for red(1)/white (0)
	for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
  {
			Epaper_Write_Data(0x00);
  }
	
//		EPD_Update();
}




void EPD_WhiteScreen_White(void)

{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
  {
			Epaper_Write_Data(0xff);
  }
	
    Epaper_Write_Command(0x26);   //write RAM for red(1)/white (0)
	for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
  {
			Epaper_Write_Data(0x00);
  }
	
//	EPD_Update();
}



//显示部分
//x_start 起始位置（列）
//y_start 起始位置（行）
//datas
//color_mode
//PART_COLUMN
//PART_LINE
//功能：在区域内显示指定颜色内容 只能符合字节（至少8行，可以1列）
///////////////////////////Part update//////////////////////////////////////////////
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,const unsigned char color_mode,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
	unsigned int i; 
	unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
	
	x_start=(x_start+8)/8;//转换为字节
	x_end=x_start+PART_LINE/8-1; 
	
	y_start1=0;
	y_start2=y_start;
	if(y_start>=256)
	{
		y_start1=y_start2/256;
		y_start2=y_start2%256;
	}
	y_end1=0;
	y_end2=y_start+PART_COLUMN-1;
	if(y_end2>=256)
	{
		y_end1=y_end2/256;
		y_end2=y_end2%256;		
	}		
	
	Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_start);    		// RAM x address start
	Epaper_Write_Data(x_end);    			// RAM x address end
	Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_start2);    	// RAM y address start
	Epaper_Write_Data(y_start1);   	 	// RAM y address start
	Epaper_Write_Data(y_end2);    		// RAM y address end
	Epaper_Write_Data(y_end1);    		// RAM y address end


	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_start); 
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
	Epaper_Write_Data(y_start2);
	Epaper_Write_Data(y_start1);

  if(color_mode==MONO)
	{
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
		for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
		 {                         
			 Epaper_Write_Data(~(* datas));
				datas++;
		 } 
	}
  if(color_mode==RED)
	{
    Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)

		for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
		 {                         
			 Epaper_Write_Data(* datas);
				datas++;
		 } 
	}

	//EPD_Update();

}

//显示部分
//x_start 起始位置（列）
//y_start 起始位置（行）
//datas
//color_mode
//PART_COLUMN
//PART_LINE
//功能：在区域内显示指定颜色内容 只能符合字节（至少8行，可以1列）
///////////////////////////Part update//////////////////////////////////////////////
void EPD_Fill_Part(unsigned int x_start,unsigned int y_start,const unsigned char data,const unsigned char color_mode,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
	unsigned int i; 
	unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
	
	x_start=(x_start+8)/8;//转换为字节
	x_end=x_start+PART_LINE/8-1; 
	
	y_start1=0;
	y_start2=y_start;
	if(y_start>=256)
	{
		y_start1=y_start2/256;
		y_start2=y_start2%256;
	}
	y_end1=0;
	y_end2=y_start+PART_COLUMN-1;
	if(y_end2>=256)
	{
		y_end1=y_end2/256;
		y_end2=y_end2%256;		
	}		
	
	Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_start);    		// RAM x address start
	Epaper_Write_Data(x_end);    			// RAM x address end
	Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_start2);    	// RAM y address start
	Epaper_Write_Data(y_start1);   	 	// RAM y address start
	Epaper_Write_Data(y_end2);    		// RAM y address end
	Epaper_Write_Data(y_end1);    		// RAM y address end


	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_start); 
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
	Epaper_Write_Data(y_start2);
	Epaper_Write_Data(y_start1);

  if(color_mode==MONO)
	{
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
		for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
		 {                         
			 Epaper_Write_Data(~(data));
		 } 
	}
  if(color_mode==RED)
	{
    Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)

		for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
		 {                         
			 Epaper_Write_Data(data);
		 } 
	}

	//EPD_Update();

}

void EPD_Dis_Part_mult(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA1,const unsigned char * datasA2,
	                     unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB1,const unsigned char * datasB2,
											 unsigned int PART_COLUMN,unsigned int PART_LINE)
{
	unsigned int i;  
	unsigned int x_endA,y_startA1,y_startA2,y_endA1,y_endA2;
	unsigned int x_endB,y_startB1,y_startB2,y_endB1,y_endB2;	
	
	//Data A////////////////////////////
	x_startA=(x_startA+8)/8;//转换为字节
	x_endA=x_startA+PART_LINE/8-1; 
	
	y_startA1=0;
	y_startA2=y_startA;
	if(y_startA2>=256)
	{
		y_startA1=y_startA2/256;
		y_startA2=y_startA2%256;
	}
	y_endA1=0;
	y_endA2=y_startA+PART_COLUMN-1;
	if(y_endA2>=256)
	{
		y_endA1=y_endA2/256;
		y_endA2=y_endA2%256;		
	}		
	
	Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_startA);    // RAM x address start at 00h;
	Epaper_Write_Data(x_endA);    // RAM x address end at 0fh(15+1)*8->128 
	Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_startA2);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_startA1);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_endA2);    // RAM y address end at 00h;
	Epaper_Write_Data(y_endA1);   

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_startA); 
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
	Epaper_Write_Data(y_startA2);
	Epaper_Write_Data(y_startA1);
	
	 Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(*datasA1);
			datasA1++;
   } 
	 
	 Epaper_Write_Command(0x26);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(*datasA2);
			datasA2++;
   } 
	 
	//Data B/////////////////////////////////////
	x_startB=(x_startB+8)/8;//转换为字节
	x_endB=x_startB+PART_LINE/8-1; 
	
	y_startB1=0;
	y_startB2=y_startB;
	if(y_startB2>=256)
	{
		y_startB1=y_startB2/256;
		y_startB2=y_startB2%256;
	}
	y_endB1=0;
	y_endB2=y_startB+PART_COLUMN-1;
	if(y_endB2>=256)
	{
		y_endB1=y_endB2/256;
		y_endB2=y_endB2%256;		
	}		
	
	Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_startB);    // RAM x address start at 00h;
	Epaper_Write_Data(x_endB);    // RAM x address end at 0fh(15+1)*8->128 
	Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_startB2);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_startB1);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_endB2);    // RAM y address end at 00h;
	Epaper_Write_Data(y_endB1);    

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_startB); 
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
	Epaper_Write_Data(y_startB2);
	Epaper_Write_Data(y_startB1);
	
	 Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(*datasB1);
			datasB1++;
   } 	

	 Epaper_Write_Command(0x26);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(*datasB2);
			datasB2++;
   } 		 
	 
	 EPD_Update();

}
//////////////////////////////////////////////////////////////////////////////////////

void ClearScreen(void)
{
		EPD_HW_Init(); //Electronic paper initialization
		EPD_WhiteScreen_White();  //Show all white
		EPD_DeepSleep();  //Enter deep sleep	
}

void DisplayTempHumi(float Temp,float Humi)
{
//		EPD_HW_Init(); //Electronic paper initialization	
//		EPD_WhiteScreen_White();  //Show all white
#ifndef QRIMG
//88*44	温度：第一位：x=8 y=8 第二位：x=52 y=8  单位：x=96 y=8  湿度： 第一位：x=156 y=8 第二位：x=200 y=8 单位：x=244 y=8
		EPD_Dis_Part(8,296-8,&gImage_OK[((uint8_t)(Temp+0.5)/10)*11*44],MONO,44,88); //第8行 第8列 开始显示 gImage_OK 宽度44 高度88
		EPD_Dis_Part(8,296-52,&gImage_OK[((uint8_t)(Temp+0.5)%10)*11*44],MONO,44,88); //第8行 第52列 开始显示 gImage_OK 宽度44 高度88
		EPD_Dis_Part(8,296-96,&gImage_OK[10*11*44],MONO,44,88); //第8行 第96列 开始显示 gImage_OK 宽度44 高度88
		EPD_Dis_Part(8,296-156,&gImage_OK[((uint8_t)(Humi+0.5)/10)*11*44],RED,44,88); //第8行 第156列 开始显示 gImage_OK 宽度44 高度88		
		EPD_Dis_Part(8,296-200,&gImage_OK[((uint8_t)(Humi+0.5)%10)*11*44],RED,44,88); //第8行 第200列 开始显示 gImage_OK 宽度44 高度88
		EPD_Dis_Part(8,296-244,&gImage_OK[11*11*44],RED,44,88); //第8行 第244列 开始显示 gImage_OK 宽度44 高度88
#else
		EPD_Dis_Part(0,0,&gImage_OK[((uint8_t)(Temp+0.5)/10)*7*64],MONO,56,64); //第0行 第0列 开始显示 gImage_OK 宽度56 高度64
		EPD_Dis_Part(0,296-56,&gImage_OK[((uint8_t)(Temp+0.5)%10)*7*64],MONO,56,64); //第8行 第52列 开始显示 gImage_OK 宽度44 高度88
		EPD_Dis_Part(0,296-112,&gImage_OK[10*7*64],MONO,56,64); //第8行 第96列 开始显示 gImage_OK 宽度44 高度88
		EPD_Dis_Part(65,0,&gImage_OK[((uint8_t)(Humi+0.5)/10)*7*64],RED,56,64); //第8行 第156列 开始显示 gImage_OK 宽度44 高度88		
		EPD_Dis_Part(65,296-56,&gImage_OK[((uint8_t)(Humi+0.5)%10)*7*64],RED,56,64); //第8行 第200列 开始显示 gImage_OK 宽度44 高度88
		EPD_Dis_Part(65,296-112,&gImage_OK[11*7*64],RED,56,64); //第8行 第244列 开始显示 gImage_OK 宽度44 高度88
	
#endif	
//		EPD_Update();
//		EPD_DeepSleep();  //Enter deep sleep	
}

void DisplayDebug(float Temp1,float Humi1,float Temp2,float Humi2,float Temp3,float Humi3,float pressure)
{
//		EPD_HW_Init(); //Electronic paper initialization	
//		EPD_WhiteScreen_White();  //Show all white
//88*44	温度：第一位：x=8 y=8 第二位：x=52 y=8  单位：x=96 y=8  湿度： 第一位：x=156 y=8 第二位：x=200 y=8 单位：x=244 y=8
		EPD_Fill_Part(96,296-4,0,RED,256,32);
		EPD_Fill_Part(96,296-256-4,0,RED,32,32);
		EPD_Fill_Part(96,296-4,0,MONO,256,32);
		EPD_Fill_Part(96,296-256-4,0,MONO,32,32);

/*	
		EPD_Dis_Part(96,296-8*1,&gImage_AB_16[29*16],RED,8,16); //T
		EPD_Dis_Part(96,296-8*2,&gImage_AB_16[40*16],RED,8,16); //e
		EPD_Dis_Part(96,296-8*3,&gImage_AB_16[48*16],RED,8,16); //m
		EPD_Dis_Part(96,296-8*4,&gImage_AB_16[51*16],RED,8,16); //p
		EPD_Dis_Part(96,296-8*5,&gImage_AB_16[1*16],RED,8,16); //1
		EPD_Dis_Part(96,296-8*6,&gImage_AB_16[62*16],RED,8,16); //:  
		EPD_Dis_Part(96,296-8*7,&gImage_AB_16[((uint8_t)(Temp1)/10)*16],RED,8,16); //:
		EPD_Dis_Part(96,296-8*8,&gImage_AB_16[((uint8_t)(Temp1)%10)*16],RED,8,16); //:
		EPD_Dis_Part(96,296-8*9,&gImage_AB_16[65*16],RED,8,16); //.
		EPD_Dis_Part(96,296-8*10,&gImage_AB_16[((uint8_t)(Temp1*10)%10)*16],RED,8,16); //:
	
		EPD_Dis_Part(96,296-90-8*1,&gImage_AB_16[29*16],RED,8,16); //T
		EPD_Dis_Part(96,296-90-8*2,&gImage_AB_16[40*16],RED,8,16); //e
		EPD_Dis_Part(96,296-90-8*3,&gImage_AB_16[48*16],RED,8,16); //m
		EPD_Dis_Part(96,296-90-8*4,&gImage_AB_16[51*16],RED,8,16); //p
		EPD_Dis_Part(96,296-90-8*5,&gImage_AB_16[2*16],RED,8,16); //2
		EPD_Dis_Part(96,296-90-8*6,&gImage_AB_16[62*16],RED,8,16); //:  
		EPD_Dis_Part(96,296-90-8*7,&gImage_AB_16[((uint8_t)(Temp2)/10)*16],RED,8,16); //:
		EPD_Dis_Part(96,296-90-8*8,&gImage_AB_16[((uint8_t)(Temp2)%10)*16],RED,8,16); //:
		EPD_Dis_Part(96,296-90-8*9,&gImage_AB_16[65*16],RED,8,16); //.
		EPD_Dis_Part(96,296-90-8*10,&gImage_AB_16[((uint8_t)(Temp2*10)%10)*16],RED,8,16); //:
	
		EPD_Dis_Part(96,296-180-8*1,&gImage_AB_16[25*16],RED,8,16); //P
		EPD_Dis_Part(96,296-180-8*2,&gImage_AB_16[53*16],RED,8,16); //r
		EPD_Dis_Part(96,296-180-8*3,&gImage_AB_16[40*16],RED,8,16); //e
		EPD_Dis_Part(96,296-180-8*4,&gImage_AB_16[54*16],RED,8,16); //s
		EPD_Dis_Part(96,296-180-8*5,&gImage_AB_16[54*16],RED,8,16); //s
		EPD_Dis_Part(96,296-180-8*6,&gImage_AB_16[62*16],RED,8,16); //:  
		EPD_Dis_Part(96,296-180-8*7,&gImage_AB_16[((uint16_t)(pressure)/1000)*16],RED,8,16); //:
		EPD_Dis_Part(96,296-180-8*8,&gImage_AB_16[((uint16_t)(pressure/100)%10)*16],RED,8,16); //:
		EPD_Dis_Part(96,296-180-8*9,&gImage_AB_16[((uint16_t)(pressure/10)%10)*16],RED,8,16); //.
		EPD_Dis_Part(96,296-180-8*10,&gImage_AB_16[((uint16_t)(pressure)%10)*16],RED,8,16); //:
	
		
		EPD_Dis_Part(112,296-8*1,&gImage_AB_16[17*16],RED,8,16); //H
		EPD_Dis_Part(112,296-8*2,&gImage_AB_16[56*16],RED,8,16); //u
		EPD_Dis_Part(112,296-8*3,&gImage_AB_16[48*16],RED,8,16); //m
		EPD_Dis_Part(112,296-8*4,&gImage_AB_16[44*16],RED,8,16); //i
		EPD_Dis_Part(112,296-8*5,&gImage_AB_16[1*16],RED,8,16); //1
		EPD_Dis_Part(112,296-8*6,&gImage_AB_16[62*16],RED,8,16); //:  
		EPD_Dis_Part(112,296-8*7,&gImage_AB_16[((uint8_t)(Humi1)/10)*16],RED,8,16); //:
		EPD_Dis_Part(112,296-8*8,&gImage_AB_16[((uint8_t)(Humi1)%10)*16],RED,8,16); //:
		EPD_Dis_Part(112,296-8*9,&gImage_AB_16[65*16],RED,8,16); //.
		EPD_Dis_Part(112,296-8*10,&gImage_AB_16[((uint8_t)(Humi1*10)%10)*16],RED,8,16); //:

		EPD_Dis_Part(112,296-90-8*1,&gImage_AB_16[17*16],RED,8,16); //H
		EPD_Dis_Part(112,296-90-8*2,&gImage_AB_16[56*16],RED,8,16); //u
		EPD_Dis_Part(112,296-90-8*3,&gImage_AB_16[48*16],RED,8,16); //m
		EPD_Dis_Part(112,296-90-8*4,&gImage_AB_16[44*16],RED,8,16); //i
		EPD_Dis_Part(112,296-90-8*5,&gImage_AB_16[2*16],RED,8,16); //2
		EPD_Dis_Part(112,296-90-8*6,&gImage_AB_16[62*16],RED,8,16); //:  
		EPD_Dis_Part(112,296-90-8*7,&gImage_AB_16[((uint8_t)(Humi2)/10)*16],RED,8,16); //:
		EPD_Dis_Part(112,296-90-8*8,&gImage_AB_16[((uint8_t)(Humi2)%10)*16],RED,8,16); //:
		EPD_Dis_Part(112,296-90-8*9,&gImage_AB_16[65*16],RED,8,16); //.
		EPD_Dis_Part(112,296-90-8*10,&gImage_AB_16[((uint8_t)(Humi2*10)%10)*16],RED,8,16); //:
*/
		EPD_Dis_Part(96,296-8*1,&gImage_AB_16[29*16],RED,8,16); //T
		EPD_Dis_Part(96,296-8*2,&gImage_AB_16[1*16],RED,8,16); //1
		EPD_Dis_Part(96,296-8*3,&gImage_AB_16[62*16],RED,8,16); //:  
		EPD_Dis_Part(96,296-8*4,&gImage_AB_16[((uint8_t)(Temp1)/10)*16],RED,8,16); //十位
		EPD_Dis_Part(96,296-8*5,&gImage_AB_16[((uint8_t)(Temp1)%10)*16],RED,8,16); //个位
		EPD_Dis_Part(96,296-8*6,&gImage_AB_16[65*16],RED,8,16); //.
		EPD_Dis_Part(96,296-8*7,&gImage_AB_16[((uint8_t)(Temp1*10)%10)*16],RED,8,16); //
		EPD_Dis_Part(96,296-8*8,&gImage_AB_16[((uint8_t)(Temp1*100)%10)*16],RED,8,16); //
		EPD_Dis_Part(96,296-8*9,&gImage_AB_16[((uint8_t)(Temp1*1000)%10)*16],RED,8,16); //
		EPD_Dis_Part(96,296-8*10,&gImage_AB_16[((uint8_t)(Temp1*10000)%10)*16],RED,8,16); //
	
		EPD_Dis_Part(96,296-90-8*1,&gImage_AB_16[29*16],RED,8,16); //T
		EPD_Dis_Part(96,296-90-8*2,&gImage_AB_16[2*16],RED,8,16); //2
		EPD_Dis_Part(96,296-90-8*3,&gImage_AB_16[62*16],RED,8,16); //:  
		EPD_Dis_Part(96,296-90-8*4,&gImage_AB_16[((uint8_t)(Temp2)/10)*16],RED,8,16); //十位
		EPD_Dis_Part(96,296-90-8*5,&gImage_AB_16[((uint8_t)(Temp2)%10)*16],RED,8,16); //个位
		EPD_Dis_Part(96,296-90-8*6,&gImage_AB_16[65*16],RED,8,16); //. 
		EPD_Dis_Part(96,296-90-8*7,&gImage_AB_16[((uint8_t)(Temp2*10)%10)*16],RED,8,16); //
		EPD_Dis_Part(96,296-90-8*8,&gImage_AB_16[((uint8_t)(Temp2*100)%10)*16],RED,8,16); //
		EPD_Dis_Part(96,296-90-8*9,&gImage_AB_16[((uint8_t)(Temp2*1000)%10)*16],RED,8,16); //
		EPD_Dis_Part(96,296-90-8*10,&gImage_AB_16[((uint8_t)(Temp2*10000)%10)*16],RED,8,16); //
	
		EPD_Dis_Part(96,296-180-8*1,&gImage_AB_16[29*16],RED,8,16); //T
		EPD_Dis_Part(96,296-180-8*2,&gImage_AB_16[3*16],RED,8,16); //3
		EPD_Dis_Part(96,296-180-8*3,&gImage_AB_16[62*16],RED,8,16); //: 
		EPD_Dis_Part(96,296-180-8*4,&gImage_AB_16[((uint8_t)(Temp3)/10)*16],RED,8,16); //十位
		EPD_Dis_Part(96,296-180-8*5,&gImage_AB_16[((uint8_t)(Temp3)%10)*16],RED,8,16); //个位
		EPD_Dis_Part(96,296-180-8*6,&gImage_AB_16[65*16],RED,8,16); //. 
		EPD_Dis_Part(96,296-180-8*7,&gImage_AB_16[((uint8_t)(Temp3*10)%10)*16],RED,8,16); // 
		EPD_Dis_Part(96,296-180-8*8,&gImage_AB_16[((uint8_t)(Temp3*100)%10)*16],RED,8,16); //
		EPD_Dis_Part(96,296-180-8*9,&gImage_AB_16[((uint8_t)(Temp3*1000)%10)*16],RED,8,16); //
		EPD_Dis_Part(96,296-180-8*10,&gImage_AB_16[((uint8_t)(Temp3*10000)%10)*16],RED,8,16); //
	
		
		EPD_Dis_Part(112,296-8*1,&gImage_AB_16[17*16],RED,8,16); //H
		EPD_Dis_Part(112,296-8*2,&gImage_AB_16[1*16],RED,8,16); //1
		EPD_Dis_Part(112,296-8*3,&gImage_AB_16[62*16],RED,8,16); //:  
		EPD_Dis_Part(112,296-8*4,&gImage_AB_16[((uint8_t)(Humi1)/10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-8*5,&gImage_AB_16[((uint8_t)(Humi1)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-8*6,&gImage_AB_16[65*16],RED,8,16); //. 
		EPD_Dis_Part(112,296-8*7,&gImage_AB_16[((uint8_t)(Humi1*10)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-8*8,&gImage_AB_16[((uint8_t)(Humi1*100)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-8*9,&gImage_AB_16[((uint8_t)(Humi1*1000)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-8*10,&gImage_AB_16[((uint8_t)(Humi1*10000)%10)*16],RED,8,16); //

		EPD_Dis_Part(112,296-90-8*1,&gImage_AB_16[17*16],RED,8,16); //H
		EPD_Dis_Part(112,296-90-8*2,&gImage_AB_16[2*16],RED,8,16); //2
		EPD_Dis_Part(112,296-90-8*3,&gImage_AB_16[62*16],RED,8,16); //:  
		EPD_Dis_Part(112,296-90-8*4,&gImage_AB_16[((uint8_t)(Humi2)/10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-90-8*5,&gImage_AB_16[((uint8_t)(Humi2)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-90-8*6,&gImage_AB_16[65*16],RED,8,16); //. 
		EPD_Dis_Part(112,296-90-8*7,&gImage_AB_16[((uint8_t)(Humi2*10)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-90-8*8,&gImage_AB_16[((uint8_t)(Humi2*100)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-90-8*9,&gImage_AB_16[((uint8_t)(Humi2*1000)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-90-8*10,&gImage_AB_16[((uint8_t)(Humi2*10000)%10)*16],RED,8,16); //

		EPD_Dis_Part(112,296-180-8*1,&gImage_AB_16[17*16],RED,8,16); //H
		EPD_Dis_Part(112,296-180-8*2,&gImage_AB_16[3*16],RED,8,16); //3
		EPD_Dis_Part(112,296-180-8*3,&gImage_AB_16[62*16],RED,8,16); //: 
		EPD_Dis_Part(112,296-180-8*4,&gImage_AB_16[((uint8_t)(Humi3)/10)*16],RED,8,16); //十位
		EPD_Dis_Part(112,296-180-8*5,&gImage_AB_16[((uint8_t)(Humi3)%10)*16],RED,8,16); //个位
		EPD_Dis_Part(112,296-180-8*6,&gImage_AB_16[65*16],RED,8,16); //. 
		EPD_Dis_Part(112,296-180-8*7,&gImage_AB_16[((uint8_t)(Humi3*10)%10)*16],RED,8,16); // 
		EPD_Dis_Part(112,296-180-8*8,&gImage_AB_16[((uint8_t)(Humi3*100)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-180-8*9,&gImage_AB_16[((uint8_t)(Humi3*1000)%10)*16],RED,8,16); //
		EPD_Dis_Part(112,296-180-8*10,&gImage_AB_16[((uint8_t)(Humi3*10000)%10)*16],RED,8,16); //
	
//		EPD_Update();
//		EPD_DeepSleep();  //Enter deep sleep	
}

//////////////////////////////////////////////////////////////////////////////////////

void Display_Init(void)
{
		DisplayGPIO_Init();
		EPD_HW_Init(); //Electronic paper initialization	
		EPD_WhiteScreen_White();  //Show all white
		//DisplayTempHumi(18,42);
		//DisplayDebug(18.2,42.3,18.6,43.3,18.6,43.3,1001);	
//88*44	温度：第一位：x=8 y=8 第二位：x=52 y=8  单位：x=96 y=8  湿度： 第一位：x=156 y=8 第二位：x=200 y=8 单位：x=244 y=8
		//EPD_Dis_Part(8,296-8,&gImage_OK[(m/10)*11*44],MONO,44,88); //第8行 第8列 开始显示 gImage_OK 宽度44 高度88
		//EPD_Dis_Part(8,296-52,&gImage_OK[(m%10)*11*44],MONO,44,88); //第8行 第52列 开始显示 gImage_OK 宽度44 高度88
		//EPD_Dis_Part(8,296-96,&gImage_OK[10*11*44],MONO,44,88); //第8行 第96列 开始显示 gImage_OK 宽度44 高度88
		//EPD_Dis_Part(8,296-156,&gImage_OK[(k/10)*11*44],RED,44,88); //第8行 第156列 开始显示 gImage_OK 宽度44 高度88		
		//EPD_Dis_Part(8,296-200,&gImage_OK[(k%10)*11*44],RED,44,88); //第8行 第200列 开始显示 gImage_OK 宽度44 高度88
		//EPD_Dis_Part(8,296-244,&gImage_OK[11*11*44],RED,44,88); //第8行 第244列 开始显示 gImage_OK 宽度44 高度88
		//显示底下24*24的 江苏智周万物科技有限公司 楷体
#ifndef 	QRIMG
		EPD_Dis_Part(104,296-4,DisLogo,MONO,256,24); //第104行 第4列 开始显示 gImage_BottomLine 宽度256 高度24	因为宽度大于256 要分两次显示  
		EPD_Dis_Part(104,296-4-256,&DisLogo[256*3],MONO,32,24); //第104行 第256列 开始显示 gImage_BottomLine 宽度32 高度24
#endif
//		EPD_Update();
//		EPD_DeepSleep();  //Enter deep sleep	
    //driver_delay_xms(3000);		

/*
		EPD_HW_Init(); //Electronic paper initialization	
		EPD_ALL_image(gImage_1_BW,gImage_1_RED);	//Refresh the picture in full screen
		EPD_DeepSleep();  //Enter deep sleep	
    driver_delay_xms(3000);		

//////////////////////Partial refresh digital presentation///////////////////

		for(k=0;k<2;k++) 
			{
				m=k*2;	
				EPD_HW_Init(); //Electronic paper initialization				
				EPD_Dis_Part(32,231,Num_MONO[m],MONO,56,56); //x,y,DATA,Resolution 56*56		
				EPD_DeepSleep();  //Enter deep sleep				
				driver_delay_xms(1000);			

				EPD_HW_Init(); //Electronic paper initialization
				EPD_Dis_Part(32,175,Num_RED[m+1],RED,56,56); //x,y,DATA,Resolution 56*56		
				EPD_DeepSleep();  //Enter deep sleep				
				driver_delay_xms(1000);				
			}

		for(k=0;k<2;k++) 
			{
				m=k*2;	
				EPD_HW_Init(); //Electronic paper initialization				
				EPD_Dis_Part_mult(32,231,ABC_MONO[m],ABC_RED[m], //x,y,DATA,Resolution 56*56		
													32,175,ABC_MONO[m+1],ABC_RED[m+1],
													56,56);
				EPD_DeepSleep();  //Enter deep sleep					
				driver_delay_xms(1000);				
			}			

////////////////////////////////////////////////////////////////////////	
		//Clear screen
*/		
}

uint8_t Getbit(uint8_t *dat,uint16_t count)
{
	uint16_t xbyte,ybit;
	uint8_t result;
	xbyte=count/8;
	result=dat[xbyte];
	ybit=count-((uint16_t)xbyte*8);
	result&=((0x01)<<(7-ybit));
	return result;
}

void Setbit(uint8_t *dat,uint16_t count)
{
	uint16_t xbyte,ybit;

	xbyte=count/8;
	ybit=count-((uint16_t)xbyte*8);
	dat[xbyte]|=((0x01)<<(7-ybit));
}

void QRGenerator(char *input)
{
	volatile int side;
	uint8_t bitdata[QR_MAX_BITDATA];
	uint8_t QRIMGDATA[2048];
	uint16_t i,j,tatalbitD=0,tatalbitS=0;
	
	//remove newline
	if (input[strlen(input)-1]=='\n')
	{
		input[strlen(input)-1]=0;
	}
	side=qr_encode(QR_LEVEL_H,0,input,0,bitdata);
	if (side==21)
	{
		
	}
	if (side==25)
	{
		//清除原图片
		memset(QRIMGDATA,0,2048);
		//左空2列
		tatalbitD+=256;
		//循环填充
		for (j=0;j<25;j++)
		{
			//上空两格
			tatalbitD++;
			tatalbitD++;
			//填充图片
			for (i=0;i<25;i++)
			{
				if (Getbit(bitdata,tatalbitS))
				{
					Setbit(QRIMGDATA,tatalbitD);tatalbitD++;
					Setbit(QRIMGDATA,tatalbitD);tatalbitD++;
					Setbit(QRIMGDATA,tatalbitD);tatalbitD++;
					Setbit(QRIMGDATA,tatalbitD);tatalbitD++;
					Setbit(QRIMGDATA,tatalbitD);tatalbitD++;
				}
				else
				{
					tatalbitD+=5;
				}
				tatalbitS++;
			}
			//下空一格
			tatalbitD++;
			//2+25*5+1=128
			for (i=0;i<4;i++)
			{
				memcpy(&QRIMGDATA[(j*5+i+3)*16],&QRIMGDATA[(j*5+i+2)*16],16);
				tatalbitD+=128;
			}
		}//for (j=0;j<25;j++)
		EPD_Dis_Part(0,128,QRIMGDATA,MONO,128,128); 
	}//if (side==25)
}


