#include<lpc21xx.h>

typedef unsigned char u8;

typedef unsigned int u32;

typedef unsigned short int u16;


extern void delay_sec(unsigned int sec);

extern void delay_ms(unsigned int ms);


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

extern void can2_init(void);

extern void can2_tx(CAN2 v);

extern void can2_rx(CAN2 *ptr);


//#define SW1 ((IOPIN0>>14)&1)

//#define SW2 ((IOPIN0>>15)&1)

//#define SW3 ((IOPIN0>>16)&1)


u8 flag,flag1,flag2,flag3;

CAN2 v1,v2;
//main.c
main()

{

        u8 f1,f2,f3;

        u32 eng_temp,speed;

        f1=f2=f3=0;

        can2_init();

        adc_init();

        config_vic_for_timer1();

        en_timer1_interrupt();

        config_vic_for_eint0();

        config_eint0();

        config_vic_for_eint1();

        config_eint1();

        config_vic_for_eint2();

        config_eint2();

        v1.dlc=1;

        v1.rtr=0;


        v1.byteA=0;


        v1.byteB=0;

        v2.dlc=2;

        v2.rtr=0;


        v2.byteA=0;


        v2.byteB=0;

        while(1)

        {

                if(flag3)

                {

                        flag3=0;


                        v1.id=0x213;

                        f1^=1;

                        if(f1)

                        {

                                v1.byteA=0x4;

                        }

                        else

                        {

                                v1.byteA=0x5;

                        }

                        can2_tx(v1);

                }

                if(flag2)

                {

                        flag2=0;

                        v1.id=0x214;

                        f2^=1;

                        if(f2)

                        {

                                v1.byteA=0x6;

                        }

                        else

                        {

                                v1.byteA=0x7;

                        }

                        can2_tx(v1);

                }

                if(flag1)

                {

                        flag1=0;

                        v1.id=0x212;

                        f3^=1;

                        if(f3)

                        {

                                v1.byteA=0x2;

                        }

                        else

                        {

                                v1.byteA=0x3;

                        }

                        can2_tx(v1);

                }

                if(flag)

                {

                        flag=0;

                        v2.id=0x215;

                        speed=adc_read(2);

                        v2.byteA=speed;

                        can2_tx(v2);

                        delay_ms(5);

                        v2.id=0x216;

                        eng_temp=adc_read(1);

                        v2.byteA=eng_temp;

                        can2_tx(v2);

                }

        }

}
//adc_drive.c
void adc_init(void)

{

        PINSEL1|=0x15400000;

        ADCR=0x00200400;

}

u32 adc_read(u8 ch_num)

{

        u32 result=0;

        ADCR|=(1<<ch_num);

        ADCR|=(1<<24);

        while(DONE==0);

        ADCR^=(1<<24);

        ADCR^=(1<<ch_num);

        result=(ADDR>>6)&0x3ff;

        return result;

}
//eint0.c
extern u8 flag1;

void EINT0_Handler(void)__irq

{

        flag1=1;




        EXTINT|=1;

        VICVectAddr=0;


}

void config_vic_for_eint0(void)

{


        VICIntSelect=0;

        VICVectCntl1=14|(1<<5);

        VICVectAddr1=(u32)EINT0_Handler;

        VICIntEnable|=(1<<14);

}

void config_eint0(void)

{

        EXTPOLAR=0;

        EXTMODE|=1;

        PINSEL1|=1;

}
//eint1.c
extern u8 flag2;

void EINT1_Handler(void)__irq

{

        flag2=1;


        EXTINT|=2;

        VICVectAddr=0;




}

void config_vic_for_eint1(void)

{

        VICIntSelect=0;

        VICVectCntl0=15|(1<<5);

        VICVectAddr0=(u32)EINT1_Handler;

        VICIntEnable|=(1<<15);

}

void config_eint1(void)

{

        EXTPOLAR=0;

        EXTMODE|=2;

        PINSEL0|=0x20000000;

}
extern u8 flag3;
//eint2.c
void EINT2_Handler(void)__irq

{


        flag3=1;


        EXTINT|=4;

        VICVectAddr=0;

}

void config_vic_for_eint2(void)

{


        //VICIntSelect=0;

        VICVectCntl3=16|(1<<5);

        VICVectAddr3=(u32)EINT2_Handler;

        VICIntEnable|=(1<<16);

}

void config_eint2(void)

{

        EXTPOLAR=0;

        EXTMODE|=4;

        PINSEL0|=0x80000000;

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
//can_drive.c
void can2_init(void)

{

        VPBDIV=1;

        PINSEL1|=0x14000;

        C2MOD=1;

        AFMR=0;

        C2BTR=0X001C001D;

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

//timer_interrupt.c
int a[]={15,60,30,15,15};

unsigned int pclk;

extern u8 flag;

void TIMER1_Handler(void) __irq

{


        flag=1;

        T1IR=1;

        VICVectAddr=0;

}

void config_vic_for_timer1(void)

{

        VICIntSelect=0;

        VICVectCntl2=5|(1<<5);

        VICVectAddr2=(int)TIMER1_Handler;

        VICIntEnable|=(1<<5);

}

void en_timer1_interrupt(void)

{

        T1MCR=3;

        pclk=a[VPBDIV]*1000000;

        T1PC=0;

        T1PR=pclk-1;

        T1TC=0;

        T1MR0=1;

        T1TCR=1;

}

