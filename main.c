#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "ws2812.h"

void gradualFill(u_int n, u_char r, u_char g, u_char b);
void alternateSeq(u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2);
void colorFade(u_char r, u_char g, u_char b);
void lightIndex(u_int lightNumber);
void lightSeq(u_int R, u_int G, u_int B);
void fillArray();
void lightSeqFlex(int R, int B, int G);
int clapper(int power);

int main(void) {
    WDTCTL = WDTPW + WDTHOLD;  // Stop WDT

    DCOCTL = CALDCO_16MHZ; // set internal oscillator at 16MHz
    BCSCTL1 = CALBC1_16MHZ; // set internal oscillator at 16MHz
    ADC10CTL1 |= CONSEQ1; //continuous sample mode, MUST BE SET FIRST!
    ADC10CTL0 |= ADC10SHT_2 + ADC10ON + MSC; //sample and hold time, adc on, cont. sample
    ADC10AE0 |= 0x01; // select channel A0
    ADC10CTL0 |= ADC10SC + ENC; // start conversions

    if (CALBC1_16MHZ==0xFF)    // If calibration constant erased
    {
        while(1);              // do not load, trap CPU!!
    }

    // configure clock to 16 MHz
    BCSCTL1 = CALBC1_16MHZ;    // DCO = 16 MHz
    DCOCTL = CALDCO_16MHZ;

    // initialize LED strip
    initStrip();  // ***** HAVE YOU SET YOUR NUM_LEDS DEFINE IN WS2812.H? ******

    // set strip color red
    fillStrip(0xFF, 0x00, 0x00);

    // show the strip
    showStrip();

    volatile unsigned int volt;
    int power = 0;
    // gradually fill for ever and ever
    while (1) {

        power = clapper(power);

    }
}

void lightSeq(u_int R, u_int G, u_int B)
{
    fillStrip(R, G, B);
    _delay_cycles(1000000);

    while(R != 5 && G != 5 && B != 5)
    {
        clearStrip();
        if (R != 0)
        {
            R -= 5;
        } else if (G != 0)
        {
            G -= 5;
        }
        else
        {
            B -= 5;
        }

        fillStrip(R, G, B);
        _delay_cycles(1000000);
    }

    while(R != 255 && G != 255 && B != 255)
    {
        clearStrip();
        if(R != 0)
        {
            R += 5;
        }
        else if (G != 0)
        {
            G += 5;
        }
        else
        {
            B += 5;
        }
        fillStrip(R, G, B);
        _delay_cycles(1000000);
    }


}

// Index through 3 green 3 yellow and 2 red lights
void lightIndex(u_int lightNumber)
{
    int i;
    for(i = 1; i <= lightNumber; i++)
    {
        if(i <= 3)
        {
            setLEDColor(i-1, 0x00, 0x8b, 0x29); //green
        }
        else if(i >= 3 && i <= 6)
        {
            setLEDColor(i-1, 0xb0, 0xbc, 0x00); //yellow
        }
        else
        {
            setLEDColor(i-1, 0xff, 0x00, 0x00); //red
        }
    }
}



void fillArray ()
{
    volatile unsigned int volume = ADC10MEM;
    clearStrip();

    if (volume <= 500)  // No lights are lit up
    {
        clearStrip();
    }
    else if (volume >= 501  && volume <= 565) // Lights up 1 green light
    {
        lightIndex(1);
    }
    else if (volume >= 566  && volume <= 630) // Lights up 2 green lights
    {
        lightIndex(2);
    }
    else if (volume >= 631  && volume <= 695) // Lights up 3 green lights
    {
        lightIndex(3);
    }
    else if (volume >= 696  && volume <= 760) // Lights up 3 green lights 1 yellow
    {
        lightIndex(4);
    }
    else if (volume >= 761 && volume <= 825) // Lights up 3 green lights 2 yellow
    {
        lightIndex(5);
    }
    else if (volume >= 826 && volume <= 890) // Lights up 3 green lights 3 yellow
    {
        lightIndex(6);
    }
    else if (volume >= 891  && volume <= 955) // 3 green lights 3 yellow 1 red
    {
        lightIndex(7);
    }
    else  //3 green 3 yellow 2 red max volume
    {
        lightIndex(8);
    }
    showStrip();

}




void colorMorph(){
    u_char saturation;
    volatile unsigned int volt = ADC10MEM;

    saturation = volt/4;

    clearStrip();
    fillStrip(0x00, 0x00, saturation);
    showStrip();

}

void equalizerRGB () {
    volatile unsigned int volt = ADC10MEM;

    if (volt >= 1 && volt <= 341) {
        clearStrip();
        fillStrip(0xFF, 0x00, 0x00);
        showStrip();
    } else if (volt >= 342 && volt <= 682) {
        clearStrip();
        fillStrip(0xAC, 0xA8, 0x32);
        showStrip();
    } else {
        clearStrip();
        fillStrip(0x00, 0xFF, 0x00);
        showStrip();
    }

    return;
}

void gradualFill(u_int n, u_char r, u_char g, u_char b){
    int i;

    for (i = 0; i < n; i++){        // n is number of LEDs
        setLEDColor(i, r, g, b);
        showStrip();
        _delay_cycles(10000000);       // lazy delay
    }
}

void alternateFill(u_int n, u_char r, u_char g, u_char b) {
    int i;

    for (i = 0; i < 4; i++) {
        setLEDColor(i, r, g, b);


    }
}

void rgbIndex(){
    volatile unsigned int volume=ADC10MEM;

    if(volume <= 300)
    {
        clearStrip();
        fillStrip(0x00, 0x8b, 0x29);
        showStrip();
    }
    else if(volume >= 301 && volume <= 700)
    {
        clearStrip();
        fillStrip(0xb0, 0xbc, 0x00);
        showStrip();
    }
    else
    {
        clearStrip();
        fillStrip(0xa5, 0x00, 0x00);
        showStrip();
    }
    return;
}


void alternateSeq(u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2) {
    int i = 0;

    for (i=0; i<8; i++){
        if (i % 2 == 0) {
            setLEDColor(i, r1, g1, b1);
        } else {
            setLEDColor(i, r2, g2, b2);
        }
       }

        showStrip();
        _delay_cycles(10000000);

        clearStrip();
        _delay_cycles(1000000);

        for (i=0; i<8; i++){
            if (i % 2 == 0) {
                setLEDColor(i, r2, g2, b2);
            } else {
                setLEDColor(i, r1, g1, b1);
            }
        }

        showStrip();
        _delay_cycles(10000000);

        clearStrip();
        _delay_cycles(1000000);


}

void colorFade(u_char r, u_char g, u_char b) {

    while (b != 0x00) {
        fillStrip(r,g,b);
        showStrip();
        _delay_cycles(1000000);

        if (r != 0x00) {
            r = r - 0x0A;
        }
        if (g != 0x00) {
            g = g - 0x0A;
        }
        if (b != 0x00) {
            b = b - 0x0A;
        }


        clearStrip();
    }

    fillStrip(0x00, 0x00,0x00);
    showStrip();
    _delay_cycles(1000000);

    r = 0x00;
    b = 0x00;
    g = 0x00;

    while (b != 0xFF) {
            fillStrip(r,g,b);
            showStrip();
            _delay_cycles(1000000);

            if (r != 0x00) {
                r = r + 0x0A;
            }
            if (g != 0x00) {
                g = g + 0x0A;
            }
            if (b != 0x00) {
                b = b + 0x0A;
            }


            clearStrip();
        }

        fillStrip(r, g,b);
        showStrip();
        _delay_cycles(1000000);


}

int getLowestColor(int R, int G, int B)
{
    if(R <= G && R <= B)
    {
        return R;
    }
    else if(G <= R && G <= B)
    {
        return G;
    }
    else
    {
        return B;
    }
}


int getHighestColor (int R, int G, int B) {

    if (R >= G && R>= B) {
        return R;
    }
    else if (G >= R && G >= B) {
        return G;
    }
    else
    {
        return B;
    }
}

/*
 * Blah blahlbhlahb.
 * @Params: int R, int G, int B
 * @Returns: None
 */
void lightSeqFlex(int R, int G, int B)
{

        int mostColor = getHighestColor(R, G, B);
        int maxIteration = mostColor / 5;
        int i;

        fillStrip(R, G, B);
        _delay_cycles(1000000);

// Calculate values for 8th, 9th, and 10th iterations. Check to see the colors make sense.

        for(i = 0; i < maxIteration; i++) // 1.) R = 153, G = 51, B = 255 // 2.) R = 148, 46, 250 // 3.) R = 143, 41, 245 // 4.) R = 138, 36, 240 // 5.) 133, 31, 235 // 6.) R = 128 G = 26, B = 230 // 7.) 123, 21, 225 8.) 118, 16, 220 9.) 113, 11, 215 10.) 108, 6, 210
        {
            if (R >= 5) {
                R -= 5;
            } else {
                R = 0;
            }

            if (G >= 5) {
                G -= 5;
            } else {
                G = 0;
            }

            if (B >= 5) {
                B -= 5;
            } else {
                B = 0;
            }

            fillStrip(R, G, B);
            _delay_cycles(1000000);
        }

        for(i = 0; i < maxIteration; i++)
        {
            if(R != 255)
            {
                R += 5;
            } else {
                R = 255;
            }
            if(G != 255)
            {
                G += 5;
            } else {
                G = 255;
            }
            if(B != 255)
            {
                B += 5;
            } else {
                B = 255;
            }

            fillStrip(R, G, B);
            _delay_cycles(1000000);
        }

}

int clapper(int power)
{
    volatile unsigned int volume = ADC10MEM;
    if (volume > 900 && power == 1)
    {
        clearStrip();
        _delay_cycles(10000000);
        return 0;
    }
    if (volume > 900 && power == 0)
    {
        fillStrip(0, 255, 0);
        _delay_cycles(10000000);
        return 1;
    }
    return power;
}



