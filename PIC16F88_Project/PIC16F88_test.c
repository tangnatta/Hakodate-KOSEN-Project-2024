/*PIC16F88$B!!(BXC$B%3%s%Q%$%iMQ%F%s%W%l!<%H(B*/
/*                          XC8v1.32*/
/*ISO-2022-JP              MPLABv2.3*/

#include <xc.h>                 //xc.h$B$rDj5A$9$k(B


//CONFIG1 REG$B!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&(B
#pragma config FOSC = INTOSCIO  //Oscillator($B%/%m%C%/%=!<%9@_Dj(B)
                                //      $B30ItH/?6(B:EXTRCCLK:$B%/%m%C%/=PNO!!(BEXTRCIO:I/O$B%]!<%H(B
                                //      $BFbItH/?6(B:INTOSCIO:31kHz$(D"7(B8MHz  INTSCCLK:RA6$B$+$i%/%m%C%/=PNO(B
                                //      $B30ItH/?64o(B:EC
                                //      $B30ItH/?6;R(B:LP:32kHz      XT:4MHz        HS:$(D"7(B20MHz)
#pragma config WDTE = OFF       //WatchDog Timer($B%&%)%C%A%I%C%/%?%$%^@_Dj(B)
                                //      ON:$B5v2D(B     OFF:$B6X;_(B
#pragma config PWRTE = ON       //Power Up Timer($B%Q%o!<%"%C%W%?%$%^@_Dj(B)
                                //      ON:$B5v2D(B     OFF:$B6X;_(B
#pragma config MCLRE = OFF      //Master Clear Enable($B%^%9%?!<%/%j%"@_Dj(B)
                                //      ON:MCLR$B$H$7$F;HMQ(B   OFF:RA5$B$H$7$F;HMQ(B
#pragma config BOREN = OFF      //Brown Out Reset($B%V%i%&%s%"%&%H%j%;%C%H@_Dj(B)
                                //      ON:$B5v2D(B     OFF:$B6X;_(B
#pragma config LVP = OFF        // Low-Voltage Programming Enable (ICSP$BDjEE05=q$-9~$_%b!<%I(B)
                                //      OFF$B!'L58z(B   ON$B!'M-8z(B    $B"(4pK\L58z(B
#pragma config CPD = OFF        //Data EE Read Protect($B%G!<%?(BEEPROM$B%W%m%F%/%H@_Dj(B)
                                //      ON:$B%W%m%F%/%HM-8z(B   OFF:$B%W%m%F%/%HL58z(B
#pragma config WRT = OFF        // Flash Memory Self-Write Protection($B%W%m%0%i%`%a%b%jJ]8n(B)
                                //      ALL$B!'J]8n(B   OFF$B!'J]8nL5$7!!(B256$B!'0lItJ,J]8n(B(256) $B!!(B2048$B!'A0H>J,J]8n(B
#pragma config CCPMX = RB0      //CCP1 Pin Selection bit(CCP1$B%T%s@_Dj(B)
                                //      RB0:RB0$B$r;HMQ(B   RB3:RB3$B$r;HMQ(B   $B"((BPWM$B=PNO$9$kB-$r$3$3$G7h$a$k(B
#pragma config CP = OFF         //Code Protect($B%3!<%I%W%m%F%/%H@_Dj(B)
                                //      ON:$B%W%m%F%/%HM-8z(B   OFF:$B%W%m%F%/%HL58z(B

//CONFIG2 REG$B!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&(B
#pragma config FCMEN = ON       //($B30It%/%m%C%/ITNI;~%/%m%C%/%=!<%9<+F0@Z$jBX$(@_Dj(B)
                                //      ON:$BM-8z(B     OFF:$BL58z(B
#pragma config IESO = ON        //Internal External Switch Over Mode($B5/F0;~$NFbIt(B=>$B30It%/%m%C%/@Z$jBX$(5v2D@_Dj(B)
                                //      ON:$BM-8z(B     OFF:$BL58z(B    $B"(FbIt%/%m%C%/;HMQ;~$O(BOFF

//$B:Y$+$JDj5AN`(B
# define _XTAL_FREQ 4000000     //$B%/%j%9%?%k$N<~GH?t$rDj5A!J(BDelay$B$G;HMQ$5$l$k!K!!(B4,000,000=4MH$B#z(B
void settings(void);           //$B=i4|@_Dj4X?t$r@k8@(B

#define BIT_READ(val, bit) (((val) & (1 << (bit))) >> bit)

inline int getValue(int current);
inline void setMotor(int on);
inline void display7Seg(int n);
inline void kickOut();
inline void switchLoop();

#define A1 RA0
#define B1 RA1
#define C1 RA2
#define D1 RA3

#define A10 RA4
#define B10 RA5
#define C10 RA6
#define D10 RA7

#define WS RB6
#define BS RB5

#define LS RB4

#define SW RB3

#define BLED RB2
#define WLED RB1

#define MIN1 RB0
#define MIN2 RB7

int iB = 0;
int iW = 0;

int lastWS = 0;
int lastBS = 0;
//int lastLS = 0;
int lastSW = 0;

int BW=0;

//$B%a%$%s4X?t!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&(B
void main(void){
    settings();                 //$B=i4|@_Dj$NFI$_9~$_(B
                                //$B0lEY$@$19T$&=hM}$O$3$3$K5-=R(B
    kickOut();
    for(;;){                    //$BL58B%k!<%W3+;O(B        
        // Kick out unwanted one
        if (WS && !lastWS){
            iW++;
        }
        if (BS && !lastBS){
            iB++;
            
            // Push it outt
            kickOut();
        }
        
        switchLoop();
        
        // Asign last varialble 
        lastWS = getValue(WS);
        lastBS = getValue(BS);
        //lastLS = getValue(LS);
        
        __delay_ms(9);
    }                           //$BL58B%k!<%W=*N;(B
}

inline void switchLoop(){
    // Check Switch
    if (SW && !lastSW){
        BW=!BW;
    }
    
    // Display 7Seg
    switch (BW){
        case 0:
            display7Seg(iW);
            WLED=1;
            BLED=0;
            break;

        case 1:
            display7Seg(iB);
            WLED=0;
            BLED=1;
            break;

        default:
            BW=0;
            display7Seg(iW);
            WLED=1;
            BLED=0;
            break;
    }
    
    // Asign last varialble
    lastSW = getValue(SW);
}

inline int getValue(int current){
    if (current){
        return 1;
    }
    else{
        return 0;
    }
}

inline void setMotor(int on){
    if (on){
        MIN1=0;
        MIN2=1;
        return;
    }
    MIN1=1;
    MIN2=1;
}

inline void display7Seg(int n){
    int one = n%10;
    int ten = n%100;
    
    // Write one digit
    A1=BIT_READ(one, 0);
    B1=BIT_READ(one, 1);
    C1=BIT_READ(one, 2);
    D1=BIT_READ(one, 3);
    
    // Write one digit
    A10=BIT_READ(ten, 0);
    B10=BIT_READ(ten, 1);
    C10=BIT_READ(ten, 2);
    D10=BIT_READ(ten, 3);
}

inline void kickOut(){
    setMotor(1);
    __delay_ms(99);
    while (LS==0){
        switchLoop();
        __delay_ms(9);
    }
    setMotor(0);
}

//$B=i4|@_Dj4X?t!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&!&(B
void settings(void){

    OPTION_REG = 0b11000000;
    /*OPTION REG($B%*%W%7%g%s5!G=$N@_Dj(B)
     *  bit7:~RBPU       $B!D(BPORTB$BFbIt%W%k%"%C%W;HMQ@_Dj(B
     *                      1:$BL58z(B  0:$BM-8z(B
     *  bit6:INTEDG      $B!D(BRB0/INT$B8!=P%(%C%8@_Dj(B
     *                      1:$BN)$A>e$j(B      0:$BN)$A2<$j(B
     *  bit5:T0CS        $B!D(BTimer0$B%=!<%9A*Br(B
     *                      1:RA4/T0CKI     0:$BFbItL?Na%/%m%C%/(B(FOSC/4)
     *  bit4:T0SE        $B!D(BTimer0$B%=!<%9%(%C%88!=P@_Dj!!"((BTOCS = 1$B$N$_(B
     *                      1:RA4/T0CKI$B$NN)$A2<$j(B   0:RA4/T0CKI$B$NN)$A>e$j(B
     *  bit3:PSA         $B!D(BTimer0/WDT$B%W%j%9%1!<%i@_Dj(B
     *                      1:$BHs%"%/%F%#%V(B(WDT)     0:$B%"%/%F%#%V(B(Timer0)
     *  bit2:PS2         $B!D%W%j%9%1!<%i@_Dj(B(WDT$B$G;HMQ;~$OH>J,(B)
     *  bit1:PS1           000=1:2    001=1:4     010=1:8    011=1:16
     *  bit0:PS0           100=1:32   101=1:64    110=1:128  111=1:256
     */

    OSCCON = 0b01100000;
    /*OSCCON REG($B%/%m%C%/<~$j$N@_Dj(B)
     *  bit7:-          $B!D$J$7(B
     *  bit6:IRCF2      $B!DFbItH/?6<~GH?t$N@_Dj(B
     *  bit5:IRCF1          000:31.25kHz    001:125kHz  010:250kHz  011:500kHz
     *  bit4:IRCF0          100:1MHz        101:2MHz    110:4MHz    111:8MHz
     *  bit3:OSTS       $B!D%9%?!<%H%"%C%W%?%$%`%"%&%H>uBV(Bbit$B!!"((B2$BCJ%9%?!<%H$N$_(B
     *                      1:$B%a%$%s%/%m%C%/(B    0:$B%;%+%s%I%/%m%C%/(B
     *  bit2:IOFS       $B!DFbIt%/%m%C%/%9%F!<%?%9(Bbit
     *                      0:$BIT0BDj(B    1:$B0BDj>uBV(B
     *  bit1:SCS1       $B!D%/%m%C%/%=!<%9A*Br(Bbit$B!!"(>JEENO$K@Z$jBX$(;~$K;HMQ(B
     *  bit0:SCS0           00:FOSC<2:0>$B$K=>$&(B  01:T1OSC    10:$BFbIt(BRC
     */

    //PORTA$B4XO"$N@_Dj(B
    TRISA = 0b00000000;
    /*TRISA REG(PIC$B$NF~=PNO@_Dj(B) $B"($[$+$K(BANSEL REG$B$r@_Dj(B
     *  bit7:TRISA7    $B!D(BRAx$B$rF~NO(B/$B=PNO$K@_Dj$9$k(B
     *  bit6:TRISA6       1:RAx$B$rF~NO%T%s$K@_Dj(B
     *  bit5:TRISA5       0:RAx$B$r=PNO%T%s$K@_Dj(B
     *  bit4:TRISA4
     *  bit3:TRISA3
     *  bit2:TRISA2
     *  bit1:TRISA1
     *  bit0:TRISA0
     */

    PORTA = 0b00000000;
    /*PORTA REG(PIC$B$N(BI/O$B$HO"F0(B)
     *  bit7:PA7    $B!D(BRAx$B$N=PNOCM(B(TRISAx$B$,(B0$B$G=PNO$K@_Dj$5$l$F$$$k>l9g(B)
     *  bit6:RA6        1:High$B%l%Y%k$,=PNO(B
     *  bit5:RA5        0:Low$B%l%Y%k$,=PNO(B
     *  bit4:RA4
     *  bit3:RA3
     *  bit2:RA2
     *  bit1:RA1
     *  bit0:RA0
     */

    //PORTB$B4XO"$N@_Dj(B
    TRISB = 0b01111000;
    /*TRISB REG(PIC$B$NF~=PNO@_Dj(B)
     *  bit7:TRISB7    $B!D(BRBx$B$rF~NO(B/$B=PNO$K@_Dj$9$k(B
     *  bit6:TRISB6       1:RBx$B$rF~NO%T%s$K@_Dj(B
     *  bit5:TRISB5       0:RBx$B$r=PNO%T%s$K@_Dj(B
     *  bit4:TRISB4
     *  bit3:TRISB3
     *  bit2:TRISB2
     *  bit1:TRISB1
     *  bit0:TRISB0
     */

    PORTB = 0b00000000;
    /*PORTB REG(PIC$B$N(BI/O$B$HO"F0(B)
     *  bit7:PB7    $B!D(BRBx$B$N=PNOCM(B(TRISBx$B$,(B0$B$G=PNO$K@_Dj$5$l$F$$$k>l9g(B)
     *  bit6:RB6        1:High$B%l%Y%k$,=PNO(B
     *  bit5:RB5        0:Low$B%l%Y%k$,=PNO(B
     *  bit4:RB4
     *  bit3:RB3
     *  bit2:RB2
     *  bit1:RB1
     *  bit0:RB0
     */

    //AD$BJQ494XO"$N@_Dj(B
    ANSEL = 0b00000000;
    /*ANSEL REG
     *  bit7:-      $B!D$J$7(B
     *  bit6:ANS6   $B!D(BANx$B%T%s$N(BA/D$B@_Dj(B
     *  bit5:ANS5       1:ANx$B%T%s$r%"%J%m%0$K@_Dj(B
     *  bit4:ANS4       0:ANx$B%T%s$r%G%8%?%k(BRAx/RBx$B$K@_Dj(B
     *  bit3:ANS3
     *  bit2:ANS2
     *  bit1:ANS1
     *  bit0:ANS0
     */
    ADCON0 = 0b00000000;
    /*ADCON0 REG
     *  bit7:ADCS1      $B!D(BA/D$BJQ49%/%m%C%/%=!<%9A*Br(Bbit
     *  bit6:ADCS0          00:Fosc/2   01:Fosc/8   10:Fosc/32   11:FRC
     *  bit5:CHS2       $B!DF~NO%T%sA*Br(B
     *  bit4:CHS1           000:Ch0(AN0)    001:Ch1(AN1)    010:Ch2(AN2)    011:Ch3(AN3)
     *  bit3:CHS0           100:Ch4(AN4)    101:Ch5(AN5)    111:Ch6(AN6)
     *  bit2:GO_nDONE   $B!D(BA/D$BJQ493+;O!&=*N;%U%i%0(B
     *  bit1:-              1:A/D$BJQ493+;O(B   0:A/D$BJQ49BT5!(Bor$B40N;(B
     *  bit0:ADON       $B!D(BA/D$BJQ49%b%8%e!<%k$N(BON/OFF
     *                     1:ON    0:OFF
     */
    ADCON1 = 0b00000000;
    /*ADCON1 REG
     *  bit7:ADFM       $B!D(BA/D$BJQ49=PNO@_Dj(B ADRESH XXXX XXXX | XXXX XXXX ADRESL
     *                      1$B!'1&5M$a(B                 00 | 0000 0000
     *                      0$B!':85M$a(B          0000 0000 | 00$B!!(B
     *  bit6:ADCS2          00:Fosc/2   01:Fosc/8   10:Fosc/32   11:FRC
     *  bit5:VCFG1      $B!D(BAD$BJQ494p=`EE05@_Dj(Bbit
     *  bit4:VCFG0
     *  bit3:-          $B!D$J$7(B
     *  bit2:-          $B!D$J$7(B
     *  bit1:-          $B!D$J$7(B
     *  bit0:-          $B!D$J$7(B
     */

    //$B%3%s%Q%l!<%?4XO"$N@_Dj(B
    CMCON = 0b00000111;
    /*CMCON REG
     *  bit7:C2OUT  $B!D%3%s%Q%l!<%?(B2$B=PNOCM(B
     *  bit6:C1OUT  $B!D%3%s%Q%l!<%?(B1$B=PNOCM(B
     *  bit5:C2INV  $B!D%3%s%Q%l!<%?(B2$B=PNOCMH?E>5v2D(Bbit
     *  bit4:C1INV  $B!D%3%s%Q%l!<%?(B1$B=PNOCMH?E>5v2D(Bbit
     *  bit3:CIS    $B!D%3%s%Q%l!<%?F~NOA*Br(Bbit
     *  bit2:CM2    $B!D%3%s%Q%l!<%?%b!<%IA*Br(B
     *  bit1:CM1        111:DDDD
     *  bit0:CM0
     */
    CVRCON = 0b00000000;
    /*CVRCON REG
     *  bit7:CVREN  $B!D%3%s%Q%l!<%?4p=`EE05(BON/OFF
     *                  1:ON    0:OFF
     *  bit6:CVROE  $B!D%3%s%Q%l!<%?4p=`EE05=PNOA*Br(B
     *                  1:RA2   0:OFF
     *  bit5:CVRR   $B!D%3%s%Q%l!<%?4p=`EE05%l%s%8@_Dj(Bbit
     *  bit4:-          $B!D$J$7(B
     *  bit3:CVR3   $B!D%3%s%Q%l!<%?4p=`EE05D4@0(Bbit
     *  bit2:CVR2
     *  bit1:CVR1
     *  bit0:CVR0
     */

    //$B3d$j9~$_4XO"$N@_Dj(B
    INTCON = 0b00000000;
    /*INTCON REG
     *  bit7:GIE        $B!D3d$j9~$_5v2D(Bbit
     *                      1:$B5v2D(B  0:$B6X;_(B
     *  bit6:PEIE       $B!D<~JU3d$j9~$_5v2D(Bbit
     *                      1:$B5v2D(B  0:$B6X;_(B
     *  bit5:TMR0IE     $B!D%?%$%^!<(B0$B3d$j9~$_5v2D(Bbit
     *                      1:$B5v2D(B  0:$B6X;_(B
     *  bit4:INT0IE     $B!D(BINT$B%T%s3d$j9~$_5v2D(Bbit
     *                      1:$B5v2D(B  0:$B6X;_(B
     *  bit3:RBIE       $B!D(BRB$B>uBVJQ2=3d$j9~$_5v2D(Bbit
     *                      1:$B5v2D(B  0:$B6X;_(B
     *  bit2:TMR0IF     $B!D%?%$%^!<(B0$B3d$j9~$_%U%i%0(B    $B"(MW%/%j%"(B
     *                      1:$B3d$j9~$_(B  0:$BDL>o(B
     *  bit1:INT0IF     $B!D(BINT$B%T%s3d$j9~$_%U%i%0(B    $B"(MW%/%j%"(B
     *                      1:$B3d$j9~$_(B  0:$BDL>o(B
     *  bit0:RBIF       $B!D(BRB$B>eCMJQ2=3d$j9~$_%U%i%0(B    $B"(MW%/%j%"(B
     *                      1:$B3d$j9~$_(B  0:$BDL>o(B
     */
    PIR2 = 0b00000000;
    /* PIR REG
     *  bit7:OSFIF      $B!D%/%m%C%/ITNI3d$j9~$_%U%i%0(B    $B"(MW%/%j%"(B
     *                      1:$B3d$j9~$_(B  0:$BDL>o(B
     *  bit6:CMIF       $B!D%3%s%Q%l!<%?3d$j9~$_%U%i%0(B    $B"(MW%/%j%"(B
     *                      1:$B3d$j9~$_(B  0:$BDL>o(B
     *  bit5:-
     *  bit4:EEIF       $B!D(BEEPROM$B=q$-9~$_40N;%U%i%0(B    $B"(MW%/%j%"(B
     *                      1:$B3d$j9~$_(B  0:$BDL>o(B
     *  bit3:-          $B!D$J$7(B
     *  bit2:-          $B!D$J$7(B
     *  bit1:-          $B!D$J$7(B
     *  bit0:-          $B!D$J$7(B
     */
    PIE2 = 0b00000000;
    /* PIE REG
     *  bit7:OSFIE      $B!D%/%m%C%/ITNI3d$j9~$_5v2D(Bbit
     *                      1:$B5v2D(B  0:$B6X;_(B
     *  bit6:CMIE       $B!D%3%s%Q%l!<%?3d$j9~$_5v2D(Bbit
     *                      1:$B5v2D(B  0:$B6X;_(B
     *  bit5:-
     *  bit4:EEIE       $B!D(BEEPROM$B=q$-9~$_40N;3d$j9~$_5v2D(Bbit
     *                      1:$B5v2D(B  0:$B6X;_(B
     *  bit3:-          $B!D$J$7(B
     *  bit2:-          $B!D$J$7(B
     *  bit1:-          $B!D$J$7(B
     *  bit0:-          $B!D$J$7(B
     */
}