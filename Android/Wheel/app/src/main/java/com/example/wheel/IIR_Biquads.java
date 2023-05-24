package com.example.wheel;

//https://www.earlevel.com/main/2003/02/28/biquads/

//https://www.earlevel.com/main/2010/12/20/biquad-calculator/
//https://www.earlevel.com/main/2003/02/28/biquads/
//https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3/

public class IIR_Biquads {

    public boolean start_inizialize_valueFLAG=false;


    //IIR biquad Filter
    public float IN, IIROut;

    public double b0;
    public double b1;

    public double a0;
    public double a1;
    public double a2;

    private double x_last1,x_last2,y_last1,y_last2;
    private double tb0,tb1,tb2,ta1,ta2;


    public float Calc_IIR(float X){

        tb0 = (double) X * a0;
        tb1 = x_last1 * a1;
        tb2 = x_last2 * a2;

        ta1 = y_last1 * b0;
        ta2 = y_last2 * b1;
        IIROut = (float) (tb0 + tb1 + tb2 + ta1 + ta2 );


        x_last2 = x_last1;
        x_last1 = (double) X;
        y_last2 = y_last1;
        y_last1 = IIROut;

        return IIROut;
    }



    public IIR_Biquads(double a0, double a1, double a2,
                       double b0,double b1){
        this.a0 = a0;
        this.a1 = a1;
        this.a2 = a2;
        this.b0 = b0;
        this.b1 = b1;
        this.start_inizialize_valueFLAG = false;
    }

    public void setValues(float initvalue){
        if(start_inizialize_valueFLAG==false) {
            this.x_last2 = (float) initvalue;
            this.x_last1 = (float) initvalue;
            this.y_last1 = (float) initvalue;
            this.y_last2 = (float) initvalue;
            start_inizialize_valueFLAG=true;
        }
    }


}
