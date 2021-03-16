// Based on the filter class design from http://www.schwietering.com/jayduino/filtuino/index.php

// For calculations: find alpha as ALPHA = dt/(dt + 1/(2*pi*fc)) where dt is the sampling rate and fc is the cutoff frequency. 

// Current Assumptions: dt = .001 and fc = .5Hz , 


//High pass butterworth filter order=2 alpha1=0.003 
class  FilterBuHp2
{
  public:
    FilterBuHp2()
    {
      v[0]=0.0;
      v[1]=0.0;
      v[2]=0.0;
    }
  private:
    float v[3];
  public:
    float step(float x) //class II 
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = (9.867597804394033778e-1 * x)
         + (-0.97369487197631476860 * v[0])
         + (1.97334424978129874262 * v[1]);
      return 
         (v[0] + v[2])
        - 2 * v[1];
    }
};
