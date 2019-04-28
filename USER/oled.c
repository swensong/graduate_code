#include "oled.h"
#include "spi.h"
#include "oledfont.h"
#include "delay.h"

/* Ӳ������ */
void SPI_OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_OLED_PORT, ENABLE );

    //Ƭѡ�źų�ʼ��
    GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN|OLED_DC_PIN|OLED_RST_PIN;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�����������
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(OLED_PORT, &GPIO_InitStructure);
    GPIO_SetBits(OLED_PORT,OLED_CS_PIN|OLED_DC_PIN|OLED_RST_PIN);

    SPI1_Init();           //��ʼ��SPI
    SPI1_SetSpeed(SPI_BaudRatePrescaler_16);    //����Ϊ18Mʱ��,����ģʽ
}  

/* ������������ */
//Ӳ��SPI��������
void OLED_Write_Byte(uint8_t dat)
{
    SPI1_ReadWriteByte(dat);
}

//д������
void OLED_Write_Data(uint8_t dat)
{
    OLED_CS_LOW;
    OLED_DC_HIGH;
    OLED_Write_Byte(dat);
}

//д������
void OLED_Write_Cmd(uint8_t cmd)
{
    OLED_CS_LOW;
    OLED_DC_LOW;
    OLED_Write_Byte(cmd);
}

/* ���ò��� */
//Function:������ʾ����
//Input:x:������;y:������
//Output:��
//Return:��
//Others:��
void OLED_Set_Pos(uint8_t x,uint8_t y)
{
    OLED_Write_Cmd(0xb0+y);//��ƫ��
    OLED_Write_Cmd(((x&0xf0)>>4)|0x10);//�и���λ��ַ
    OLED_Write_Cmd((x&0x0f)|0X01);//�е���λ��ַ
}

//Function:����OLED��ʾ
//Input:��
//Output:��
//Return:��
//Others:��
void OLED_Display_On(void)
{
    OLED_Write_Cmd(0X8D);//����DCDC
    OLED_Write_Cmd(0X14);//��DCDC
    OLED_Write_Cmd(0XAF);//��OLED
}

//Function:�ر�OLED��ʾ
//Input:��
//Output:��
//Return:��
//Others:��
void OLED_Display_Off(void)
{
    OLED_Write_Cmd(0X8D);//����DCDC
    OLED_Write_Cmd(0X10);//�ر�DCDC
    OLED_Write_Cmd(0XAE);//�ر�OLED
}

//Function:����
//Input:��
//Output:��
//Return:��
//Others:��
void OLED_Display_Clear(void)
{
    u8 page,len;

    for(page=0;page<8;page++)
    {
        OLED_Write_Cmd(0xB0+page);//����ҳ��ַ(0--7)
        OLED_Write_Cmd(0X00);//������ʾλ�á��е͵�ַ
        OLED_Write_Cmd(0X10); //������ʾλ�á��иߵ�ַ

        for(len=0;len<128;len++)
            OLED_Write_Data(0);//д��0;��Ϩ��
    }
}
//Function:��ָ��λ����ʾһ���ַ�,���������ַ�
    //x:0~127;y:0~63;size:ѡ������ 16/12 
//Input:x��������;y:������;str:��ʾ���ַ�
//Output:��
//Return:��
//Others:��               
void OLED_Display_Onechar(u8 x,u8 y,u8 str)
{
    u8 i=0,ret=0;
    //ret = str -32;
    ret = str - ' ';//�õ�ƫ�ƺ��ֵ,��ASCLL�����һ������.���ڶ�ά������������λ��  
    if(x>Max_Column-1)
    {
        x = 0;
        y = y + 2;//���16�ŵ��ַ�
    }
    if(SIZE == 16 )
    {
        OLED_Set_Pos(x,y);
        //16������ֳ�������д��
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

//Function:��ʾ�ַ���
//Input:x��������;y:������;str:��ʾ���ַ���
//Output:��
//Return:��
//Others:��
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

//Function:��ʾ����
//Input:x��������;y:������;no:��ʾ���ֵ����
//Output:��
//Return:��
//Others:��
void OLED_Display_Chinese(u8 x,u8 y,u8 no)
{
    u8 ch,addr=0;

    OLED_Set_Pos(x,y);
    for(ch=0;ch<16;ch++)//��������Ѱַ
    {
        OLED_Write_Data(Hzk[2*no][ch]);//������2���ֽ�
        addr += 1;
    }
    OLED_Set_Pos(x,y+1);
    for(ch=0;ch<16;ch++)
    {
        OLED_Write_Data(Hzk[2*no+1][ch]);
        addr += 1;
    }

}

//Function:��ʾͼƬ
//Input:x0,x1��������;y0,y1:������;BMP:��ʾ��ͼƬ
//ע�⣺y1<8,ҳѰַģʽ
//Output:��
//Return:��
//Others:��
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

//����m��n�η�
u32 OLED_Pow(u8 m,u8 n)
{
    u32 ret = 1;
    while(n--)
        ret *= m;
    return ret;
}

//Function:��ʾ����
//Input:x��������;1:������;num:��ʾ�����֣�len�����ֳ���;size_num:��������
//Output:��
//Return:��
//Others:��
void OLED_Display_Num(u8 x,u8 y,u32 num,u8 len,u8 size_num)
{
    u8 t,temp;
    u8 enshow = 0;

    for(t=0;t<len;t++)
    {
        temp = (num/OLED_Pow(10,len-t-1))%10;//����ʾ������һλһλȡ����
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

//��ʼ��SSD1306
//���������뿪�����������ص�......
void OLED_Init(void)
{
    SPI_OLED_Init();

    OLED_RST_HIGH;
    delay_ms(200);
    OLED_RST_LOW;
    delay_ms(200);
    OLED_RST_HIGH; 

    OLED_Write_Cmd(0xAE);//�ر�OLED
    OLED_Write_Cmd(0x00);//�����е�λ��ַ
    OLED_Write_Cmd(0x10);//�����и�λ��ַ
    OLED_Write_Cmd(0x40);//������ʼ�е�ַ��ӳ��RAM��ʾ��ʼ�� (0x00~0x3F)
    OLED_Write_Cmd(0x81);//�Աȶ�����
    OLED_Write_Cmd(0xCF); // Set SEG Output Current Brightness
    OLED_Write_Cmd(0xA1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    OLED_Write_Cmd(0xC8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
    OLED_Write_Cmd(0xA6);//������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ
    OLED_Write_Cmd(0xA8);//��������·��(1 to 64)
    OLED_Write_Cmd(0x3f);//--1/64 duty
    OLED_Write_Cmd(0xD3);//-������ʾƫ��(0x00~0x3F)
    OLED_Write_Cmd(0x00);//-not offset
    OLED_Write_Cmd(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_Write_Cmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_Write_Cmd(0xD9);//--set pre-charge period
    OLED_Write_Cmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_Write_Cmd(0xDA);//--set com pins hardware configuration
    OLED_Write_Cmd(0x12);
    OLED_Write_Cmd(0xDB);//--set vcomh
    OLED_Write_Cmd(0x40);//Set VCOM Deselect Level
    OLED_Write_Cmd(0x20);//����ҳ��ַģʽ(0x00/0x01/0x02)
    OLED_Write_Cmd(0x02);//
    OLED_Write_Cmd(0x8D);//--set Charge Pump enable/disable
    OLED_Write_Cmd(0x14);//--set(0x10) disable
    OLED_Write_Cmd(0xA4);//������ʾ����;bit0:1,����;0,�ر�;(����/����)(0xa4/0xa5)
    OLED_Write_Cmd(0xA6);// ��ʹ�÷�����ʾ (0xa6/a7) 
    OLED_Write_Cmd(0xAF);//����ʾ

    OLED_Write_Cmd(0xAF); /*display ON*/
    OLED_Display_Clear();
    OLED_Set_Pos(0,0);  
}  


































