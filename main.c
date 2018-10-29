#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#include "config..h"


#define _XTAL_FREQ 32000000
#include "flex_lcd.h"


#define triger LATBbits.LATB1
#define eco PORTBbits.RB0


void main(void) {
    float distancia;
    unsigned int conteoMSB = 0;
    unsigned int conteoLSB = 0;
    unsigned char buffer1 [16];
    
    
    //**VELOCIDAD DEL OSCILADOR
    OSCFRQbits.HFFRQ=0b110;
    
    //**PUERTOS
    PORTB=0; //**LIMPIAMOS EL PUERTO C
   
    ANSELB=0;
      ANSELD=0; // PUERTO D COMO DIGITAL
    ANSELE=0; // PUERTO E COMO DIGITAL
    TRISBbits.TRISB1=0;//**SALIDA DE PULSO
   
    TRISBbits.TRISB0=1;//**ENTRADA DE PULSO
       
    //**CONFIGURACION DEL TIMMER1
    T1CON=0b00110100; //**PAGINA <<411
    T1CLKbits.CS=0b0001; //**FUENTE DE RELOJ Fosc/4 
    
    /*11 PREESCALER DE 8
     1 NO SONCRONIZADO
     ULTIMO BIT, INDICA ENCENDIDO DEL TIMMER1*/
    
    //**interrupcion del timmer1
    INTCON=0b11000000; //**SOLO SE HABILITO LA INTERRUPCION PEIE Y GIE PAGINA <<<414 DATASHEET
    PIE4bits.TMR1IE=1; //**HABILITACION DE INTERRUPCIONES DEL TIMMER1 PAGINA <<138 DATASHEET
    PIR4bits.TMR1IF=0; //**CONFIGURACION DE LA BANDERA PAJ <<148 DATASHEET
    
    //INICIALIZA LA LCD
      Lcd_Init();//INICIALIZA LCD
    Lcd_Cmd(LCD_CLEAR );//LIMPIA LCD
    Lcd_Cmd(LCD_CURSOR_OFF );//APAGA EL CURSOR
    Lcd_Cmd(LCD_BLINK_CURSOR_ON );
        __delay_ms(500);
    
    while (1){
        
          Lcd_Out(1,13,"cm");   
             __delay_ms(500);
        
             
        TMR1L=177; //**carga de los registros
        TMR1H=224; //**""
        //**PARA LA SALIDA DEL PULSO DE VOLTAJE
        
        triger=1; //**INICIAMOS EL DISPARO DEL PULSO A DETECTAR 
        __delay_us(10);//**ESPERAMOS 10 us
        triger=0; //**APAGAMOS EL PULSO DEL SENSOR
        
    
        
        
        
        while(!eco);//***ESPERA DE PULSO DE RESPUESTA DEL PIN ECHO
            /*ESTO SIGNIFICA QUE MIENTRAS EL VALOR DE ECO SEA CERO SEGIREMOS ESPERANDO HASTA QUE CAMBIE 
             DE ESTADO*/
            
            T1CONbits.TMR1ON=1;//**CUANDO HALLA LLEGADO ENCENDER EL TIMMER 1
            
        while(eco && !PIR4bits.TMR1IF);
            /*LA PRIMERA CONDICION ESPERA A QUE EL VALOR DEL PIN ECO VUELBA A CERO
             Y QUE ADEMAS SE HALLA LEIDO EL VALOR DE LA BANDERA */
                
            T1CONbits.TMR1ON=0; //** UNA VEZ CUMPLIDAS LAS CONDICIONES APAGAMOS EL TIMMER1
            PIR4bits.TMR1IF=0; //** LIMPIAMOS BANDERA
            
         
         
            if(PIR4bits.TMR1IF==1){
                conteoMSB=142;
                conteoLSB=183;
                PIR4bits.TMR1IF=0; //**LIMPIAR BANDERA
            } 
            else {
                conteoMSB=TMR1H;
                conteoLSB=TMR1L;
            }
           // TMR1//
            //conteoMSB=TMR1H;
            //conteoLSB=TMR1L;
            
            conteoMSB=conteoMSB<<8; //**OPERACIONES NECESARIAS
            conteoMSB=conteoMSB + conteoLSB;
            
            distancia=(((conteoMSB-45536)/29)/2)-207;
            __delay_ms(500);
            
           sprintf(buffer1,"distancia= %04.0f",distancia);
         Lcd_Out2(1,0,buffer1);
         __delay_ms(500);
            
    }
            
    return;
}
