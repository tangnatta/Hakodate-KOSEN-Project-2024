/*PIC16F88　XCコンパイラ用テンプレート*/
/*                          XC8v1.32*/
/*ISO-2022-JP              MPLABv2.3*/

#include <xc.h>                 //xc.hを定義する


//CONFIG1 REG・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
#pragma config FOSC = INTOSCIO  //Oscillator(クロックソース設定)
                                //      外部発振:EXTRCCLK:クロック出力　EXTRCIO:I/Oポート
                                //      内部発振:INTOSCIO:31kHz~8MHz  INTSCCLK:RA6からクロック出力
                                //      外部発振器:EC
                                //      外部発振子:LP:32kHz      XT:4MHz        HS:~20MHz)
#pragma config WDTE = OFF       //WatchDog Timer(ウォッチドックタイマ設定)
                                //      ON:許可     OFF:禁止
#pragma config PWRTE = ON       //Power Up Timer(パワーアップタイマ設定)
                                //      ON:許可     OFF:禁止
#pragma config MCLRE = OFF      //Master Clear Enable(マスタークリア設定)
                                //      ON:MCLRとして使用   OFF:RA5として使用
#pragma config BOREN = OFF      //Brown Out Reset(ブラウンアウトリセット設定)
                                //      ON:許可     OFF:禁止
#pragma config LVP = OFF        // Low-Voltage Programming Enable (ICSP定電圧書き込みモード)
                                //      OFF：無効   ON：有効    ※基本無効
#pragma config CPD = OFF        //Data EE Read Protect(データEEPROMプロテクト設定)
                                //      ON:プロテクト有効   OFF:プロテクト無効
#pragma config WRT = OFF        // Flash Memory Self-Write Protection(プログラムメモリ保護)
                                //      ALL：保護   OFF：保護無し　256：一部分保護(256) 　2048：前半分保護
#pragma config CCPMX = RB0      //CCP1 Pin Selection bit(CCP1ピン設定)
                                //      RB0:RB0を使用   RB3:RB3を使用   ※PWM出力する足をここで決める
#pragma config CP = OFF         //Code Protect(コードプロテクト設定)
                                //      ON:プロテクト有効   OFF:プロテクト無効

//CONFIG2 REG・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
#pragma config FCMEN = ON       //(外部クロック不良時クロックソース自動切り替え設定)
                                //      ON:有効     OFF:無効
#pragma config IESO = ON        //Internal External Switch Over Mode(起動時の内部=>外部クロック切り替え許可設定)
                                //      ON:有効     OFF:無効    ※内部クロック使用時はOFF

//細かな定義類
# define _XTAL_FREQ 4000000     //クリスタルの周波数を定義（Delayで使用される）　4,000,000=4MHｚ
void settings(void);           //初期設定関数を宣言

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

//メイン関数・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
void main(void){
    settings();                 //初期設定の読み込み
                                //一度だけ行う処理はここに記述
    kickOut();
    for(;;){                    //無限ループ開始        
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
    }                           //無限ループ終了
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

//初期設定関数・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
void settings(void){

    OPTION_REG = 0b11000000;
    /*OPTION REG(オプション機能の設定)
     *  bit7:~RBPU       …PORTB内部プルアップ使用設定
     *                      1:無効  0:有効
     *  bit6:INTEDG      …RB0/INT検出エッジ設定
     *                      1:立ち上り      0:立ち下り
     *  bit5:T0CS        …Timer0ソース選択
     *                      1:RA4/T0CKI     0:内部命令クロック(FOSC/4)
     *  bit4:T0SE        …Timer0ソースエッジ検出設定　※TOCS = 1のみ
     *                      1:RA4/T0CKIの立ち下り   0:RA4/T0CKIの立ち上り
     *  bit3:PSA         …Timer0/WDTプリスケーラ設定
     *                      1:非アクティブ(WDT)     0:アクティブ(Timer0)
     *  bit2:PS2         …プリスケーラ設定(WDTで使用時は半分)
     *  bit1:PS1           000=1:2    001=1:4     010=1:8    011=1:16
     *  bit0:PS0           100=1:32   101=1:64    110=1:128  111=1:256
     */

    OSCCON = 0b01100000;
    /*OSCCON REG(クロック周りの設定)
     *  bit7:-          …なし
     *  bit6:IRCF2      …内部発振周波数の設定
     *  bit5:IRCF1          000:31.25kHz    001:125kHz  010:250kHz  011:500kHz
     *  bit4:IRCF0          100:1MHz        101:2MHz    110:4MHz    111:8MHz
     *  bit3:OSTS       …スタートアップタイムアウト状態bit　※2段スタートのみ
     *                      1:メインクロック    0:セカンドクロック
     *  bit2:IOFS       …内部クロックステータスbit
     *                      0:不安定    1:安定状態
     *  bit1:SCS1       …クロックソース選択bit　※省電力に切り替え時に使用
     *  bit0:SCS0           00:FOSC<2:0>に従う  01:T1OSC    10:内部RC
     */

    //PORTA関連の設定
    TRISA = 0b00000000;
    /*TRISA REG(PICの入出力設定) ※ほかにANSEL REGを設定
     *  bit7:TRISA7    …RAxを入力/出力に設定する
     *  bit6:TRISA6       1:RAxを入力ピンに設定
     *  bit5:TRISA5       0:RAxを出力ピンに設定
     *  bit4:TRISA4
     *  bit3:TRISA3
     *  bit2:TRISA2
     *  bit1:TRISA1
     *  bit0:TRISA0
     */

    PORTA = 0b00000000;
    /*PORTA REG(PICのI/Oと連動)
     *  bit7:PA7    …RAxの出力値(TRISAxが0で出力に設定されている場合)
     *  bit6:RA6        1:Highレベルが出力
     *  bit5:RA5        0:Lowレベルが出力
     *  bit4:RA4
     *  bit3:RA3
     *  bit2:RA2
     *  bit1:RA1
     *  bit0:RA0
     */

    //PORTB関連の設定
    TRISB = 0b01111000;
    /*TRISB REG(PICの入出力設定)
     *  bit7:TRISB7    …RBxを入力/出力に設定する
     *  bit6:TRISB6       1:RBxを入力ピンに設定
     *  bit5:TRISB5       0:RBxを出力ピンに設定
     *  bit4:TRISB4
     *  bit3:TRISB3
     *  bit2:TRISB2
     *  bit1:TRISB1
     *  bit0:TRISB0
     */

    PORTB = 0b00000000;
    /*PORTB REG(PICのI/Oと連動)
     *  bit7:PB7    …RBxの出力値(TRISBxが0で出力に設定されている場合)
     *  bit6:RB6        1:Highレベルが出力
     *  bit5:RB5        0:Lowレベルが出力
     *  bit4:RB4
     *  bit3:RB3
     *  bit2:RB2
     *  bit1:RB1
     *  bit0:RB0
     */

    //AD変換関連の設定
    ANSEL = 0b00000000;
    /*ANSEL REG
     *  bit7:-      …なし
     *  bit6:ANS6   …ANxピンのA/D設定
     *  bit5:ANS5       1:ANxピンをアナログに設定
     *  bit4:ANS4       0:ANxピンをデジタルRAx/RBxに設定
     *  bit3:ANS3
     *  bit2:ANS2
     *  bit1:ANS1
     *  bit0:ANS0
     */
    ADCON0 = 0b00000000;
    /*ADCON0 REG
     *  bit7:ADCS1      …A/D変換クロックソース選択bit
     *  bit6:ADCS0          00:Fosc/2   01:Fosc/8   10:Fosc/32   11:FRC
     *  bit5:CHS2       …入力ピン選択
     *  bit4:CHS1           000:Ch0(AN0)    001:Ch1(AN1)    010:Ch2(AN2)    011:Ch3(AN3)
     *  bit3:CHS0           100:Ch4(AN4)    101:Ch5(AN5)    111:Ch6(AN6)
     *  bit2:GO_nDONE   …A/D変換開始・終了フラグ
     *  bit1:-              1:A/D変換開始   0:A/D変換待機or完了
     *  bit0:ADON       …A/D変換モジュールのON/OFF
     *                     1:ON    0:OFF
     */
    ADCON1 = 0b00000000;
    /*ADCON1 REG
     *  bit7:ADFM       …A/D変換出力設定 ADRESH XXXX XXXX | XXXX XXXX ADRESL
     *                      1：右詰め                 00 | 0000 0000
     *                      0：左詰め          0000 0000 | 00　
     *  bit6:ADCS2          00:Fosc/2   01:Fosc/8   10:Fosc/32   11:FRC
     *  bit5:VCFG1      …AD変換基準電圧設定bit
     *  bit4:VCFG0
     *  bit3:-          …なし
     *  bit2:-          …なし
     *  bit1:-          …なし
     *  bit0:-          …なし
     */

    //コンパレータ関連の設定
    CMCON = 0b00000111;
    /*CMCON REG
     *  bit7:C2OUT  …コンパレータ2出力値
     *  bit6:C1OUT  …コンパレータ1出力値
     *  bit5:C2INV  …コンパレータ2出力値反転許可bit
     *  bit4:C1INV  …コンパレータ1出力値反転許可bit
     *  bit3:CIS    …コンパレータ入力選択bit
     *  bit2:CM2    …コンパレータモード選択
     *  bit1:CM1        111:DDDD
     *  bit0:CM0
     */
    CVRCON = 0b00000000;
    /*CVRCON REG
     *  bit7:CVREN  …コンパレータ基準電圧ON/OFF
     *                  1:ON    0:OFF
     *  bit6:CVROE  …コンパレータ基準電圧出力選択
     *                  1:RA2   0:OFF
     *  bit5:CVRR   …コンパレータ基準電圧レンジ設定bit
     *  bit4:-          …なし
     *  bit3:CVR3   …コンパレータ基準電圧調整bit
     *  bit2:CVR2
     *  bit1:CVR1
     *  bit0:CVR0
     */

    //割り込み関連の設定
    INTCON = 0b00000000;
    /*INTCON REG
     *  bit7:GIE        …割り込み許可bit
     *                      1:許可  0:禁止
     *  bit6:PEIE       …周辺割り込み許可bit
     *                      1:許可  0:禁止
     *  bit5:TMR0IE     …タイマー0割り込み許可bit
     *                      1:許可  0:禁止
     *  bit4:INT0IE     …INTピン割り込み許可bit
     *                      1:許可  0:禁止
     *  bit3:RBIE       …RB状態変化割り込み許可bit
     *                      1:許可  0:禁止
     *  bit2:TMR0IF     …タイマー0割り込みフラグ    ※要クリア
     *                      1:割り込み  0:通常
     *  bit1:INT0IF     …INTピン割り込みフラグ    ※要クリア
     *                      1:割り込み  0:通常
     *  bit0:RBIF       …RB上値変化割り込みフラグ    ※要クリア
     *                      1:割り込み  0:通常
     */
    PIR2 = 0b00000000;
    /* PIR REG
     *  bit7:OSFIF      …クロック不良割り込みフラグ    ※要クリア
     *                      1:割り込み  0:通常
     *  bit6:CMIF       …コンパレータ割り込みフラグ    ※要クリア
     *                      1:割り込み  0:通常
     *  bit5:-
     *  bit4:EEIF       …EEPROM書き込み完了フラグ    ※要クリア
     *                      1:割り込み  0:通常
     *  bit3:-          …なし
     *  bit2:-          …なし
     *  bit1:-          …なし
     *  bit0:-          …なし
     */
    PIE2 = 0b00000000;
    /* PIE REG
     *  bit7:OSFIE      …クロック不良割り込み許可bit
     *                      1:許可  0:禁止
     *  bit6:CMIE       …コンパレータ割り込み許可bit
     *                      1:許可  0:禁止
     *  bit5:-
     *  bit4:EEIE       …EEPROM書き込み完了割り込み許可bit
     *                      1:許可  0:禁止
     *  bit3:-          …なし
     *  bit2:-          …なし
     *  bit1:-          …なし
     *  bit0:-          …なし
     */
}