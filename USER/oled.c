#include "oled.h"
#include "spi.h"
#include "oledfont.h"
#include "delay.h"

/* 硬件部分 */
void SPI_OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_OLED_PORT, ENABLE );

    //片选信号初始化
    GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN|OLED_DC_PIN|OLED_RST_PIN;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(OLED_PORT, &GPIO_InitStructure);
    GPIO_SetBits(OLED_PORT,OLED_CS_PIN|OLED_DC_PIN|OLED_RST_PIN);

    SPI1_Init();           //初始化SPI
    SPI1_SetSpeed(SPI_BaudRatePrescaler_16);    //设置为18M时钟,高速模式
}  

/* 基本函数部分 */
//硬件SPI发送数据
void OLED_Write_Byte(uint8_t dat)
{
    SPI1_ReadWriteByte(dat);
}

//写入数据
void OLED_Write_Data(uint8_t dat)
{
    OLED_CS_LOW;
    OLED_DC_HIGH;
    OLED_Write_Byte(dat);
}

//写入命令
void OLED_Write_Cmd(uint8_t cmd)
{
    OLED_CS_LOW;
    OLED_DC_LOW;
    OLED_Write_Byte(cmd);
}

/* 调用部分 */
//Function:设置显示坐标
//Input:x:横坐标;y:纵坐标
//Output:无
//Return:无
//Others:无
void OLED_Set_Pos(uint8_t x,uint8_t y)
{
    OLED_Write_Cmd(0xb0+y);//列偏移
    OLED_Write_Cmd(((x&0xf0)>>4)|0x10);//行高四位地址
    OLED_Write_Cmd((x&0x0f)|0X01);//行低四位地址
}

//Function:开启OLED显示
//Input:无
//Output:无
//Return:无
//Others:无
void OLED_Display_On(void)
{
    OLED_Write_Cmd(0X8D);//设置DCDC
    OLED_Write_Cmd(0X14);//打开DCDC
    OLED_Write_Cmd(0XAF);//打开OLED
}

//Function:关闭OLED显示
//Input:无
//Output:无
//Return:无
//Others:无
void OLED_Display_Off(void)
{
    OLED_Write_Cmd(0X8D);//设置DCDC
    OLED_Write_Cmd(0X10);//关闭DCDC
    OLED_Write_Cmd(0XAE);//关闭OLED
}

//Function:清屏
//Input:无
//Output:无
//Return:无
//Others:无
void OLED_Display_Clear(void)
{
    u8 page,len;

    for(page=0;page<8;page++)
    {
        OLED_Write_Cmd(0xB0+page);//设置页地址(0--7)
        OLED_Write_Cmd(0X00);//设置显示位置—列低地址
        OLED_Write_Cmd(0X10); //设置显示位置—列高地址

        for(len=0;len<128;len++)
            OLED_Write_Data(0);//写入0;屏熄灭
    }
}
//Function:在指定位置显示一个字符,包括部分字符
    //x:0~127;y:0~63;size:选择字体 16/12 
//Input:x：横坐标;y:纵坐标;str:显示的字符
//Output:无
//Return:无
//Others:无               
void OLED_Display_Onechar(u8 x,u8 y,u8 str)
{
    u8 i=0,ret=0;
    //ret = str -32;
    ret = str - ' ';//得到偏移后的值,对ASCLL码进行一个减法.即在二维数组里找它的位置  
    if(x>Max_Column-1)
    {
        x = 0;
        y = y + 2;//针对16号的字符
    }
    if(SIZE == 16 )
    {
        OLED_Set_Pos(x,y);
        //16的字体分成两部分写入
        for(i=0;i<8;i++)
        OLED_Write_Data(F8X16[ret*16+i]);
        OLED_Set_Pos(x,y+1);
        for(i=0;i<8;i++)
            OLED_Write_Data(F8X16[ret*16+i+8]);
    }
    else{
        OLED_Set_Pos(x,y+1);
        for(i=0;i<6;i++)
            OLED_Write_Data(F6x8[ret][i]);
    }
}

//Function:显示字符串
//Input:x：横坐标;y:纵坐标;str:显示的字符串
//Output:无
//Return:无
//Others:无
void OLED_Display_String(u8 x,u8 y,u8 *str)
{
    u8 i=0;

    while(str[i]!='\0')
    {
        OLED_Display_Onechar(x,y,str[i]);
        x += 8;
        if(x>120)
        {
            x = 0;
            y += 2;
        }
        i++;
    }
}

//Function:显示中文
//Input:x：横坐标;y:纵坐标;no:显示的字的序号
//Output:无
//Return:无
//Others:无
void OLED_Display_Chinese(u8 x,u8 y,u8 no)
{
    u8 ch,addr=0;

    OLED_Set_Pos(x,y);
    for(ch=0;ch<16;ch++)//数组行列寻址
    {
        OLED_Write_Data(Hzk[2*no][ch]);//汉字是2个字节
        addr += 1;
    }
    OLED_Set_Pos(x,y+1);
    for(ch=0;ch<16;ch++)
    {
        OLED_Write_Data(Hzk[2*no+1][ch]);
        addr += 1;
    }

}

//Function:显示图片
//Input:x0,x1：横坐标;y0,y1:纵坐标;BMP:显示的图片
//注意：y1<8,页寻址模式
//Output:无
//Return:无
//Others:无
void OLED_Display_Picture(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
{
    u8 x,y;
    u32 i=0;

    if(y1%8==0)
        y = y1 / 8;
    else
        y = y1 /8 + 1;
    for(y=y0;y<y1;y++)
    {
        OLED_Set_Pos(x0,y);
        for(x=x0;x<x1;x++)
        {
            OLED_Write_Data(BMP[i++]);
        }
    }
}

//计算m的n次方
u32 OLED_Pow(u8 m,u8 n)
{
    u32 ret = 1;
    while(n--)
        ret *= m;
    return ret;
}

//Function:显示数字
//Input:x：横坐标;1:纵坐标;num:显示的数字：len：数字长度;size_num:数字字体
//Output:无
//Return:无
//Others:无
void OLED_Display_Num(u8 x,u8 y,u32 num,u8 len,u8 size_num)
{
    u8 t,temp;
    u8 enshow = 0;

    for(t=0;t<len;t++)
    {
        temp = (num/OLED_Pow(10,len-t-1))%10;//把显示的数字一位一位取出来
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_Display_Onechar(x+(size_num/2)*t,y,' ');
                continue;
            }
            else
                enshow = 1;
        }
        OLED_Display_Onechar(x+(size_num/2)*t,y,temp+'0');
    }
}

//初始化SSD1306
//命令设置与开启函数作用重叠......
void OLED_Init(void)
{
    SPI_OLED_Init();

    OLED_RST_HIGH;
    delay_ms(200);
    OLED_RST_LOW;
    delay_ms(200);
    OLED_RST_HIGH; 

    OLED_Write_Cmd(0xAE);//关闭OLED
    OLED_Write_Cmd(0x00);//设置列低位地址
    OLED_Write_Cmd(0x10);//设置列高位地址
    OLED_Write_Cmd(0x40);//设置起始行地址及映射RAM显示起始行 (0x00~0x3F)
    OLED_Write_Cmd(0x81);//对比度设置
    OLED_Write_Cmd(0xCF); // Set SEG Output Current Brightness
    OLED_Write_Cmd(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_Write_Cmd(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_Write_Cmd(0xA6);//设置显示方式;bit0:1,反相显示;0,正常显示
    OLED_Write_Cmd(0xA8);//设置驱动路数(1 to 64)
    OLED_Write_Cmd(0x3f);//--1/64 duty
    OLED_Write_Cmd(0xD3);//-设置显示偏移(0x00~0x3F)
    OLED_Write_Cmd(0x00);//-not offset
    OLED_Write_Cmd(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_Write_Cmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_Write_Cmd(0xD9);//--set pre-charge period
    OLED_Write_Cmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_Write_Cmd(0xDA);//--set com pins hardware configuration
    OLED_Write_Cmd(0x12);
    OLED_Write_Cmd(0xDB);//--set vcomh
    OLED_Write_Cmd(0x40);//Set VCOM Deselect Level
    OLED_Write_Cmd(0x20);//设置页地址模式(0x00/0x01/0x02)
    OLED_Write_Cmd(0x02);//
    OLED_Write_Cmd(0x8D);//--set Charge Pump enable/disable
    OLED_Write_Cmd(0x14);//--set(0x10) disable
    OLED_Write_Cmd(0xA4);//禁用显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)(0xa4/0xa5)
    OLED_Write_Cmd(0xA6);// 不使用反向显示 (0xa6/a7) 
    OLED_Write_Cmd(0xAF);//打开显示

    OLED_Write_Cmd(0xAF); /*display ON*/
    OLED_Display_Clear();
    OLED_Set_Pos(0,0);  
}  


































