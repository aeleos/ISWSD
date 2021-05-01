// Based on the filter class design from http://www.schwietering.com/jayduino/filtuino/index.php

// For calculations: find alpha = dt/(dt + 1/(2*pi*fc)) where dt is the sampling rate and fc is the cutoff frequency. 

// Current Assumptions: 


//Band stop butterworth filter order=1 alpha1=0.001 alpha2=0.1 
class  FilterBuBs2
{
  public:
    FilterBuBs2()
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
      v[2] = (7.567445040180087457e-1 * x)
         + (-0.51348900803603525489 * v[0])
         + (1.51040231445543438049 * v[1]);
      return 
         1.000000 * v[0]
        +v[2]
        - 1.995921 * v[1];
    }
};
