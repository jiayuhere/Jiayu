#include <reg51.h>
#define false 0
#define true 1
#define uchar unsigned char
#define uint unsigned int
sbit pause_key = P3^1;  //��ͣ��ť
sbit auto_key = P3^0;   //�ֶ�ģʽ��Ч���л�
sbit change_key = P3^2; //�ֶ�ģʽЧ���л�
sbit pauseLed = P3^6;   //��ͣ������ָʾ��
sbit autoLed = P3^7;    //�Զ����ֶ�ģʽָʾ��
int ledCode[8]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f}; //led����(������ʾ)
int ledCode2[8]={0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80,0x00}; //led���루����ʾ����
//int disCode[10]={0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f,0x01,0x09}; //����ܶ��� 1~9
void displayLed(void);  //��ʾled��������
void keyScan(void); //����ɨ�账����
void Delay10ms(unsigned int n); //��ʱ10ms
bit isPause = false;    //�Ƿ���ͣ
bit isAuto = true;  //�Ƿ��Զ�����
bit isChange = false;   //�Ƿ�Ҫ�л���һ��Ч��
uchar time;   //��ʱ��0.5s
uchar types;    //�ڼ��ֵƹ���ʾ����
uint counts;    //�ƹ�ĵڼ���

//Location������Ҫ��ʾ�ĵڼ�λ����ܣ�Number������ʾ���������ʾ������
void smg(unsigned char Location,Number){
    
    unsigned char smgWeiShu[] = {0xe3,0xe7,0xeb,0xef,0xf3,0xf7,0xfb,0xff};    
    unsigned char smgNumber[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
        P2=smgWeiShu[Location];
        P0=smgNumber[Number];
        //�������Ӱ
       Delay10ms(100);
        P0=0x00;
}

/******************************************************************************* 
* �� �� ��          : Delay10ms (���) 
* ��������         : ��ʱ��������ʱn*10ms 
* ��      ��         : n-��ʱ���� 
* ��      ��         : �� 
*******************************************************************************/
void Delay10ms(unsigned int n)
{
    unsigned char a, b;
    for (; n>0; n--)
    {
        for (b=38; b>0; b--)
        {
            for (a=130; a>0; a--);
        }
    }
}


/******************************************************************************* 
* �� �� ��          : T0_INT
* ��������         : T0��ʱ���жϺ���
* ��      ��         : ��
* ��      ��         : �� 
*******************************************************************************/
void T0_INT(void)  interrupt 1
{
    TL0= (65536-50000)/256;
    TH0= (65536-50000)%256;
    time ++;
    if(time >= 10) //��ʱʱ�䣺0.5s
    {
        time=0;
        if(isChange == true)    //���Ա任��һ����ʾЧ����
        {
            counts = 0;
            types++;    //��ʾ��һ��Ч��
            if(types > 9) types = 0;
//            P0 = disCode[types];        //�����������ʾ
			smg(8,types);
			 Delay10ms(100);
            isChange = false;
        }
        displayLed();
        counts++;
    }
}
/******************************************************************************* 
* �� �� ��          : main
* ��������         : ������
* ��      ��         : ��
* ��      ��         : �� 
*******************************************************************************/
void main(void)
{
    TMOD=0x61; //0110 0001  //��ʽһ
    TL0= (65536-50000)/256;     //50ms
    TH0= (65536-50000)%256;
    TR0=1;      //����T0
    ET0=1;      //T0�ж�����
    EA=1;   //���жϿ���

    time = 0;   //��ʱ��ʱ�����֣�0.5s��
    counts = 0; //�ƹ�ĵڼ���    
    types = 0;  //�ƹ���ʾģʽ
    pauseLed = 0; //��ָͣʾ����
//    P0 = disCode[types];        //�����������ʾ
	smg(8,types);
	 Delay10ms(100);
    while(1)
    {
        keyScan();  //����ɨ�輰����
    }
}
/******************************************************************************* 
* �� �� ��          : keyScan
* ��������         : ����ɨ�账��
* ��      ��         : ��
* ��      ��         : �� 
*******************************************************************************/
void keyScan(void)
{
    if(pause_key == 0)  //��������ͣ��ť
    {
        Delay10ms(1);
        if(pause_key == 0)
        {
            isPause = ~isPause;
            pauseLed = isPause;
            if(isPause == true)
            {
                ET0=0;      //�ر�T0�ж�
                P0 = 0x40;  //�������ʾ ��-��
//                P1 = 0x00;  //���еĵƶ�������
                P2 = 0x00;
            }else{
                ET0=1;      //T0�ж�����
//                P0 = disCode[types];        //�����������ʾ
					smg(8,types);
					 Delay10ms(100);
                displayLed();
            }
            while(pause_key == 0);  //��ֹ�����ظ����
        }
    }
    if(auto_key == 0)   //�Զ����ֶ��л���������
    {
        Delay10ms(1);
        if(auto_key == 0)
        {
            isAuto = ~isAuto;
            autoLed = isAuto;
        }
        while(auto_key == 0);   //��ֹ�����ظ����
    }
    if(change_key == 0 && isAuto == false)  //�ֶ�ģʽ������Ч���л�����
    {
        Delay10ms(1);
        if(change_key == 0)
        {
            isChange = true;
        }
        while(change_key == 0); //��ֹ�����ظ����
    }
}
/******************************************************************************* 
* �� �� ��          : displayLed
* ��������         : ��ʾled��
* ��      ��         : (ȫ�ֱ��� )types����ʾЧ����counts����ǰЧ���µĵڼ���
* ��      ��         : �� 
*******************************************************************************/
void displayLed(void)
{
    switch(types)
    {
		          case 0: //˳ʱ���������
        {
            if(counts >= 17) counts = 0;
            if(counts <8)
            {
//                P1 = ~ledCode2[7 - counts];
                P2 = 0xff;
            }
            else if(counts <16)
            {
//                P1 = 0x00;
                P2 = ~ledCode2[15 - counts];
            }
            else    //ȫ��
            {
//                P1 = 0x00;
                P2 = 0x00;
                if(isAuto == true) isChange = true;
            }
            break;
        }
//        case 0: //˳ʱ����תled��
//        {
//            if(counts >= 8) counts = 0;
//            if(counts >=7)
//            {
//                if(isAuto == true) isChange = true;
//            }
//            if(counts <8)
//            {
////                P1 = 0xff;
//                P2 = ledCode[7-counts];
////					smg(8,7-counts);
////					 Delay10ms(50);
//            }
////            else
////            {
//////                P1 = ledCode[15-counts];
////                P2 = 0xff;
////            }
//            break;
//        }
        case 1: //˳ʱ����������
        {
            if(counts >= 17) counts = 0;
            if(counts <8)
            {
//                P1 = ledCode2[7 - counts];
                P2 = 0x00;
            }
            else if(counts <16)
            {
//                P1 = 0xff;
                P2 = ledCode2[15 - counts];
            }
            else    //ȫ��
            {
//                P1 = 0xff;
                P2 = 0xff;
                if(isAuto == true) isChange = true;
            }
            break;
        }
//        case 1: //��ʱ����תLED��
//        {
//            if(counts >= 8) counts = 0;
//            if(counts >=7)
//            {
//                if(isAuto == true) isChange = true;
//            }
//        if(counts <8)
//        {
////                P1 = ledCode[counts];
//            P2 = 0xff;
////        }
////            else
//			if(counts >=8)
//            {
////                P1 = 0xff;
//                P2 = ledCode[counts-8];
////				 Delay10ms(100);
//            }
//            break;
//        }
        case 2: //��ʱ���������
        {
            if(counts >= 17) counts = 0;
            if(counts <8)
            {
//                P1 = 0xff;
                P2 = ledCode2[counts];
            }
            else if(counts <16)
            {
//                P1 = ledCode2[counts - 7];
                P2 = 0x00;
            }
            else    //ȫ��
            {
//                P1 = 0x00;
                P2 = 0x00;
                if(isAuto == true) isChange = true;
            }
            break;
        }
		        case 3: //��ʱ��������
        {
            if(counts >= 17) counts = 0;
            if(counts <8)
            {
//                P1 = 0x00;
                P2 = ~ledCode2[counts];
            }
            else if(counts <16)
            {
//                P1 = ~ledCode2[counts - 7];
                P2 = 0xff;
            }
            else    //ȫ��
            {																   
//                P1 = 0xff;
                P2 = 0xff;
                if(isAuto == true) isChange = true;
            }
            break;
        }
 
        case 4: //�����滻
        {
            if(counts >= 8) counts = 0;
            if(counts >=7)
            {
                if(isAuto == true) isChange = true;
            }
            if(counts % 2 == 0) //ż��
            {
//                P1=0xaa;
                P2=0xaa;
            }
            else
            {
//                P1=0x55;
                P2=0x55;
            }
            break;
        }
		        case 5: //�����Ƽӷ�
        {
            if(counts >= 255) counts = 0;
            if(counts == 254 && isAuto == true) isChange = true;
//            P1 = ~counts;
            P2 = ~counts;
            break;
        }

//        case 3: //�Խ�˳ʱ��
//        {
//            if(counts >= 8) counts = 0;
//            if(counts >=7)
//            {
//                if(isAuto == true) isChange = true;
//            }
////            P1 = ledCode[7 - counts];
//            P2 = ledCode[7 - counts];
//            break;
//        }
//        case 4: //�Խ���ʱ��
//        {
//            if(counts >= 8) counts = 0;
//            if(counts >=7)
//            {
//                if(isAuto == true) isChange = true;
//            }
////            P1 = ledCode[counts];
//            P2 = ledCode[counts];
//            break;
//        }
//        case 3: //˳ʱ���������
//        {
//            if(counts >= 17) counts = 0;
//            if(counts <8)
//            {
////                P1 = ~ledCode2[7 - counts];
//                P2 = 0xff;
//            }
//            else if(counts <16)
//            {
////                P1 = 0x00;
//                P2 = ~ledCode2[15 - counts];
//            }
//            else    //ȫ��
//            {
////                P1 = 0x00;
//                P2 = 0x00;
//                if(isAuto == true) isChange = true;
//            }
//            break;
//        }
//        case 4: //˳ʱ����������
//        {
//            if(counts >= 17) counts = 0;
//            if(counts <8)
//            {
////                P1 = ledCode2[7 - counts];
//                P2 = 0x00;
//            }
//            else if(counts <16)
//            {
////                P1 = 0xff;
//                P2 = ledCode2[15 - counts];
//            }
//            else    //ȫ��
//            {
////                P1 = 0xff;
//                P2 = 0xff;
//                if(isAuto == true) isChange = true;
//            }
//            break;
//        }
//        case 3: //��ʱ���������
//        {
//            if(counts >= 17) counts = 0;
//            if(counts <8)
//            {
////                P1 = 0xff;
//                P2 = ledCode2[counts];
//            }
//            else if(counts <16)
//            {
////                P1 = ledCode2[counts - 7];
//                P2 = 0x00;
//            }
//            else    //ȫ��
//            {
////                P1 = 0x00;
//                P2 = 0x00;
//                if(isAuto == true) isChange = true;
//            }
//            break;
//        }
//        case 4: //��ʱ��������
//        {
//            if(counts >= 17) counts = 0;
//            if(counts <8)
//            {
////                P1 = 0x00;
//                P2 = ~ledCode2[counts];
//            }
//            else if(counts <16)
//            {
////                P1 = ~ledCode2[counts - 7];
//                P2 = 0xff;
//            }
//            else    //ȫ��
//            {																   
////                P1 = 0xff;
//                P2 = 0xff;
//                if(isAuto == true) isChange = true;
//            }
//            break;
//        }
//       
        default:
            types = 0;
//            P0 = disCode[types];        //�����������ʾ
			smg(8,types);
			 Delay10ms(100);

    }
 }