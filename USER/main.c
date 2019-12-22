#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 
#include "ad.h"
#include "spi.h"
#include "timer.h"
#include "oled.h"	
#include "esp8266.h"
#include "string.h"
#include "dac.h"
#include "A9G.h"

void System_Init();

int main(void)
{	 
  char p[30];
	u8 state=1;
	u8 key=0;
	int i=0;
	int heat=4;
	u8 delay=0;
				//char str[1024];
				//s32 s[10];
				//s32 sum=0;
				//int k=0;
				//int m=0;
	
	System_Init();															//ϵͳ��ʼ��
	
	OLED_ShowString(0,2,"  Initializing");			//��ʼ����ɣ���Ļ��ʾ��ʼ������
	
	for(delay=0;delay<6;delay++)								//��ʱ6��
	{delay_ms(1000);}
	
	while (1)
	{
		key=KEY_Scan(0);													//��¼�������£�������Ӧ����ֵ
		if(key)
		{
			/*state״̬��
				1��ͨ��ѡ��״̬������ѡ��ͨ��
				2���¶ȵ���״̬�������ı�����¶�
			
				key�������ܣ�
				1��ͨ�������ӣ���ѭ����/�¶�����
				2��ͨ���ż�С����ѭ����/�¶ȼ�С
				3���л�state״̬
			*/
			if(key==3)state++;
			if(state==3)state=1;
			
			if(key==1&&state==1)
			{	
				i++;
				if(i>5)	i=0;
			}
			
			if(key==2&&state==1)
			{	
				i--;
				if(i==-1)	i=5;
			}
			
			if(key==1&&state==2)
			{	
				heat++;
				if(heat>=5)		heat=5;
			}
			
			if(key==2&&state==2)
			{	
				heat--;
				if(heat==-1)	heat=0;
			}
			
			/*heatֵ���ڿ���pwm��ռ�ձ�*/
			switch(heat)
			{				 
				case 0:
					duty=0;
					break;
				case 1:	
					duty=5;
					break;
				case 2:
					duty=10;
					break;
				case 3:
					duty=15;
					break;
				case 4:
					duty=20;
					break;
				case 5:
					duty=25;
					break;
			}
		}
		else delay_ms(10);
		
		
		if(flag) 
		{
						//int j=0;
			flag = 0;
            //sum=0;
            //sprintf(str,"NO.%d   ",j++);
      sweep_read();																								//����ͨ������ֵ���Ŵ�������ѹ����ֵ����Ĵ���
		
            /*for(j=0; j<9; j++)
            {
                s[j]=s[j+1];
                sum+=s[j+1];
            }
            s[9]=dat[i];
            sum+=dat[i];*/
      res[i]=cal_res(gain[i], 100000, -dat[i]);										//���ݴ������������ʾ
      gain[i]=1<<gain[i];
      dat[i]=-dat[i];
		
            /*for(i=0;i<12 ;i++)
            {
                sprintf(p,"%d: D=%d  G=%d  ",i,dat[i], gain[i]);
                strcat(str,p);
            }*/
            //sprintf(p,"AT+CIPSEND=0,%d",strlen(str));
			OLED_Clear();
			
			/*��״̬����ʾͨ���ĵ�ѹ����ֵ���Ŵ���������ֵ*/
			if(state==1)
			{
				if (dat[i]>=0)  sprintf(p,"D = %d",dat[i]);
        else sprintf(p,"D = N%d",-dat[i]);
				
        OLED_ShowString(0,2,(u8 *)p);
        sprintf(p,"No.%d: G=%d",i+1,gain[i]);
        OLED_ShowString(0,0,(u8 *)p);
				
				/*�ֶδ������ֵres[i]�ĵ�λ����ʾ��OLED��*/
        if (res[i] <1e3) 
        {
					sprintf(p,"R = %lf",res[i]);
          if (p[7] != '.') p[8]='\0';
          else p[7] = '\0';
        }
        else if (res[i] <1e6) 
        {
					sprintf(p,"R = %lf",res[i]/ 1000.0);
          if (p[7] != '.') 
					{
							p[9]='k';
							p[10]= '\0';
					}
					else 
					{
							p[9]='k';
							p[10]='\0';
					}
        }
				else
				{
					sprintf(p,"R=%lfM",res[i]/ 1000000.0);                
					if (p[7] != '.') 
					{
							p[9]='M';
							p[10]= '\0';
					}
					else 
					{
							p[9]='M';
							p[10]='\0';
					}
				}
				OLED_ShowString(0,4,(u8 *)p);
			}
			/*��״̬����ʾͨ���ļ��ȵ�ѹ*/
			else if(state==2)
			{
				OLED_ShowString(0,0,"Heat voltage:");
				sprintf(p," %d V",heat);
				OLED_ShowString(0,2,(u8 *)p);
			}
		}
	}
}

void System_Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  delay_init();	    	                                //��ʱ������ʼ��
  uart_init(115200);	 																//���ڳ�ʼ��Ϊ115200
				//delay_ms(2);
				//printf("HELLOWORLD\n");
  LED_Init();		  																		//��ʼ����LED���ӵ�Ӳ���ӿ�
	Amp_Init();
				//Amp=1;
  ad_init();
				//printf("AD init done\n");
  usart3_init(115200);
  OLED_Init();
				//for (i=0;i<=1000;i++) printf("test\n\r");
	A9G_Init();
	TIM3_Int_Init(3999,7199);														//400ms 
  Dac1_Init();																				//DAC��ʼ��
	TIM4_Int_Init(299,10);	 														//24k 
				//TIM4_Int_Init(299,9);	 											//24k 
				//for(k=0; k<10 ;k++) s[k]=0;
}


