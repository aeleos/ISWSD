// Based on the filter class design from http://www.schwietering.com/jayduino/filtuino/index.php

// For calculations: find alpha = dt/(dt + 1/(2*pi*fc)) where dt is the sampling rate and fc is the cutoff frequency. 

// Current Assumptions: 


//Band stop butterworth filter order=2 alpha1=0.06 alpha2=6 
class  FilterBuBs2
{
  public:
    FilterBuBs2()
    {
      for(int i=0; i <= 4; i++)
        v[i]=0.0;
    }
  private:
    float v[5];
  public:
    float step(float x) //class II 
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = v[3];
      v[3] = v[4];
      v[4] = (INF * x)
         + (-1.71241780505297924719 * v[0])
         + (5.93308138268682316863 * v[1])
         + (-7.75633281397461615825 * v[2])
         + (4.53393654971823245603 * v[3]);
      return 
        NaN * v[0]
        NaN * v[4]
        NaN * v[1]
        NaN * v[3]
        NaN * v[2];
    }
};
