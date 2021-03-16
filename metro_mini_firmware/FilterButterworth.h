// Based on the filter class design from http://www.schwietering.com/jayduino/filtuino/index.php

// For calculations: find alpha = dt/(dt + 1/(2*pi*fc)) where dt is the sampling rate and fc is the cutoff frequency. 

// Current Assumptions: 


//Band stop butterworth filter order=2 alpha1=0.0006 alpha2=0.06 
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
      v[4] = (7.676587588983782018e-1 * x)
         + (-0.59004884997913487066 * v[0])
         + (2.65877114337209752293 * v[1])
         + (-4.54680790303286475051 * v[2])
         + (3.47808402272094330243 * v[3]);
      return 
         (v[0] + v[4])
        - 3.997124 * (v[1] + v[3])
        +5.994251 * v[2];
    }
};
