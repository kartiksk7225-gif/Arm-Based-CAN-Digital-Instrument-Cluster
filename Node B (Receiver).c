#include<lpc21xx.h>

typedef unsigned char u8;

typedef unsigned int u32;

typedef unsigned short int u16;


extern void delay_sec(unsigned int sec);

extern void delay_ms(unsigned int ms);


extern void lcd_data(unsigned char data);

extern void lcd_cmd(unsigned char cmd);

extern void lcd_init(void);

extern void lcd_string(unsigned char *ptr);

extern void lcd_integer(int num);

extern void lcd_cgram(void);

extern void lcd_float(float num);

extern void lcd_float_adc(float num);


#define DONE ((ADDR>>31)&1)


extern void adc_init(void);

extern u32 adc_read(u8 ch_num);


extern void config_vic_for_eint1(void);

extern void config_eint1(void);


extern void config_vic_for_eint0(void);

extern void config_eint0(void);


extern void config_vic_for_eint2(void);

extern void config_eint2(void);


extern void config_vic_for_timer1(void);

extern void en_timer1_interrupt(void);

//
#define TCS2 (C2GSR&8)

#define RBS2 (C2GSR&1)


typedef struct CAN2_MSG

{

u32 id;

u32 rtr;

u32 dlc;

u32 byteA;

u32 byteB;

}CAN2;


extern void en_can2_interrupt(void);

extern void config_vic_for_CAN2(void);


extern void can2_init(void);

extern void can2_tx(CAN2 v);

extern void can2_rx(CAN2 *ptr);
//main.c
#define HL (1<<18)
#define LI (1<<17)
#define RI (1<<19)
CAN2 m1;
u32 flag;
main()
{

        u8 f1,f2;
        u32 speed,per;
        //u32 eng_val;
        //float temp,vol;
        f1=f2=0;
        can2_init();
        config_vic_for_CAN2();
        en_can2_interrupt();
        lcd_init();
        lcd_cgram();
        IODIR0=HL|LI|RI;
        IOSET0=HL|LI|RI;
        //lcd_string("ECU 2");




        while(1)

        {


                l:


                if(flag)


                {

                                flag=0;

                                if(m1.id==0x215)


                                {


                                        //speed

                                        speed=m1.byteA;

                                        lcd_cmd(0x80);

                                        lcd_string("SPEED:");

                                        per=((float)speed/1023)*100;

                                        //lcd_cmd(0xc1);

                              
                                        lcd_integer(per);

                                        lcd_string("Kmph");


                                }


                                if(m1.id==0x213)  //headlight


                                {


                                        if((m1.byteA&0xf)==0x4)


                                        {


                                                lcd_cmd(0xc7);

                                                lcd_data(1);

                                        IOCLR0|=HL;


                                        }


                                        if((m1.byteA&0xf)==0x5)


                                        {


                                                lcd_cmd(0xc7);

                                                lcd_data(' ');

                                        IOSET0|=HL;




                                        }


                                }

                                if(m1.id==0x212)   //leftindicator


                                {


                                        if((m1.byteA&0xf)==0x2)


                                        {

                                                lcd_cmd(0xc1);

                                                lcd_data(' ');


                                                f1=0;


                                                f2=1;


                                        }


                                        if((m1.byteA&0xf)==0x3)


                                        {

                                                lcd_cmd(0xc1);

                                                lcd_data(' ');

                                                f2=0;

                                        //      uart0_tx_string("k\r\n");

                                        }


                                }

                                if(m1.id==0x214) //right indi


                                {


                                        if((m1.byteA&0xf)==0x6)


                                        {

                                                lcd_cmd(0xcc);

                                                lcd_data(' ');


                                                f1=1;

                                                f2=0;

                                        //      uart0_tx_string("l\r\n");

                                        }


                                        if((m1.byteA&0xf)==0x7)


                                        {

                                                lcd_cmd(0xcc);

                                                lcd_data(' ');

                                                f1=0;

                                        }


                                }

                                if(m1.id==0x216)


                                {


                                        //Engine temperature

                                        eng_val=m1.byteA;

                                        vol=(eng_val*3.3)/1023;

                                        temp=(vol-0.5)/0.01;

                                        lcd_cmd(0x88);

                                        lcd_string("T:");

                                        lcd_float_adc(temp);

                                        lcd_string("^C");


                                }


                }															

                if(f1)


                {


                        while(flag==0)


                        {


                                lcd_cmd(0xc0);

                                lcd_data(0);

                                IOCLR0|=LI;

                                delay_ms(250);


                                lcd_cmd(0xc0);

                                lcd_data(' ');

                                IOSET0|=LI;

                                delay_ms(250);

                        }


                        goto l;


                }


                if(f2)


                {


                        while(flag==0)


                        {


                                lcd_cmd(0xcf);

                                lcd_data(2);

                        IOCLR0|=RI;

                                delay_ms(250);


                                lcd_cmd(0xcf);

                                lcd_data(' ');

                                IOSET0|=RI;

                                delay_ms(250);

                        }


                        goto l;


                }

              


        }


}
//delay.c
void delay_sec(unsigned int sec)

{

        int a[]={15,60,30,15,15};

        unsigned int pclk;

        pclk=a[VPBDIV]*1000000;

        T0PC=0;

        T0PR=pclk-1;

        T0TC=0;

        T0TCR=1;        //START TIMER 0

        while(T0TC<sec);

        T0TCR=0;        //STOP TIMER 0

}

void delay_ms(unsigned int ms)

{

        int a[]={15,60,30,15,15};

        unsigned int pclk;

        pclk=a[VPBDIV]*1000;

        T0PC=0;

        T0PR=pclk-1;

        T0TC=0;

        T0TCR=1;        //start timer 0

        while(T0TC<ms);

        T0TCR=0;        //stop timer 0

}
//lcd_drive.c
void lcd_data(unsigned char data)

{

        IOCLR1=0xfe<<16;

        IOSET1=(data&0xf0)<<16;

        IOSET1=1<<17;

        IOCLR1=1<<18;

        IOSET1=1<<19;

        delay_ms(2);

        IOCLR1=1<<19;


        IOCLR1=0xfe<<16;

        IOSET1=(data&0xf)<<20;

        IOSET1=1<<17;

        IOCLR1=1<<18;

        IOSET1=1<<19;

        delay_ms(2);

        IOCLR1=1<<19;

}

void lcd_cmd(unsigned char cmd)

{

        IOCLR1=0xfe<<16;

        IOSET1=(cmd&0xf0)<<16;

        IOCLR1=1<<17;

        IOCLR1=1<<18;

        IOSET1=1<<19;

        delay_ms(2);

        IOCLR1=1<<19;


        IOCLR1=0xfe<<16;

        IOSET1=(cmd&0xf)<<20;

        IOCLR1=1<<17;

        IOCLR1=1<<18;

        IOSET1=1<<19;

        delay_ms(2);

        IOCLR1=1<<19;

}

void lcd_init()

{

        IODIR1=0xfe<<16;

        PINSEL2=0;

        lcd_cmd(0x02);

        lcd_cmd(0x28);

        lcd_cmd(0x0e);

        lcd_cmd(0x0c);

        lcd_cmd(0x01);

}

void lcd_string(unsigned char *ptr)

{

        while(*ptr)

        {

                lcd_data(*ptr);

                ptr++;

        }

}

void lcd_integer(int num)

{

        int a[15],i;

        if(num==0)

        {

                lcd_data('0');

        }

        if(num<0)

        {

                lcd_data('-');

                num=-num;

        }

        i=0;

        while(num>0)

        {

                a[i]=num%10+48;

                num=num/10;

                i++;

        }

        for(i=i-1;i>=0;i--)

        {

                lcd_data(a[i]);

        }

}

void lcd_cgram(void)

{

         unsigned char a[]={0x00,0x04,0x08,0x1f,0x1f,0x08,0x04,0x00,0x00,0x0e,0x00,0x1f,0x11,0x11,0x0e,0x00,0x0,0x04,0x02,0x1f,0x1f,0x02,0x04,0x00},i;


                lcd_cmd(0x40);

        for(i=0;i<=24;i++)

                lcd_data(a[i]);

}


void lcd_float(float num)

{

        int temp,k=1000000;

        temp=num;

        if(temp<0)

        {

                lcd_data('-');

                temp=-temp;

                num=-num;

        }

        lcd_integer(temp);

        lcd_data('.');

        temp=(num-temp)*k;

        k=k/10;

        while(k>temp)

        {

                lcd_data('0');

                k/=10;

        }

        lcd_integer(temp);

}

void lcd_float_adc(float num)

{


        int temp,a[15],i,j,c=0,k;

        if(num<0)

        {

                lcd_data('-');

                num=-num;

        }

        temp=num*10;

        j=num;

        for(c=0,k=num;k;c++,k/=10);

        i=0;

        if(temp==0)

        {

                a[i]=0;

                i++;

        }

        else

        {

                while(temp)

                {

                        a[i]=temp%10;

                        temp=temp/10;

                        i++;

                }

        }

        lcd_integer(j);

        lcd_data('.');

        //for(i=i-c-1;i>=0;i--)

        lcd_integer(a[0]);

}

//can_tx.c
extern u32 flag;

extern CAN2 m1;

void CAN2_RX_Handler(void) __irq

{

        m1.id=C2RID;

        m1.dlc=(C2RFS>>16)&0xf;

        m1.rtr=(C2RFS>>30)&1;

        if(m1.rtr==0)

        {

                m1.byteA=C2RDA;

                m1.byteB=C2RDB;

        }

        C2CMR=(1<<2);

        flag=1;

        VICVectAddr=0;

}

void en_can2_interrupt(void)

{

        C2IER=1;

}

void config_vic_for_CAN2(void)

{

        VICIntSelect=0;

        VICVectCntl2=27|(1<<5);

        VICVectAddr2=(u32) CAN2_RX_Handler;

        VICIntEnable=(1<<27);

}
//can_rx.c
u32 *ptr=(u32*)0xe0038000;

void can2_init(void)

{

        VPBDIV=1;

        PINSEL1|=0x14000;

        C2MOD=1;

        C2BTR=0X001C001D;


        ptr[0]=0x32112212;

        ptr[1]=0x22132214;

        ptr[2]=0x22152216;


        SFF_sa=0;

        ENDofTable=0xc;

        SFF_GRP_sa=0xc;

        EFF_GRP_sa=0xc;

        EFF_sa=0xc;

        AFMR=2;

        C2MOD=0;

}

void can2_tx(CAN2 v)

{

        C2TID1=v.id;

        C2TFI1=(v.dlc<<16);

        if(v.rtr==0)

        {

                C2TDA1=v.byteA;

                C2TDB1=v.byteB;

        }

        else

                C2TFI1|=(1<<30);


        C2CMR=1|(1<<5);

        while(TCS2==0);

}

void can2_rx(CAN2 *ptr)

{

        while(RBS2==0);

        ptr->id=C2RID;

        ptr->dlc=(C2RFS>>16)&0xf;

        ptr->rtr=(C2RFS>>30)&1;

        if(ptr->rtr==0)

        {

                ptr->byteA=C2RDA;

                ptr->byteB=C2RDB;

        }

        C2CMR=(1<<2);

}



