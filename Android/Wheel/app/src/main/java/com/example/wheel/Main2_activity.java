package com.example.wheel;


import androidx.activity.OnBackPressedCallback;
import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProvider;

import android.content.Intent;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.ekn.gruzer.gaugelibrary.ArcGauge;
import com.ekn.gruzer.gaugelibrary.Range;
import com.example.wheel.Ble_Manager.BLE_ViewModel;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import no.nordicsemi.android.ble.livedata.state.ConnectionState;

import static android.content.ContentValues.TAG;
import static java.lang.Math.abs;


public class Main2_activity extends AppCompatActivity {
    public static final String EXTRA_DEVICE = "no.nordicsemi.android.blinky.EXTRA_DEVICE";

    private  Intent ScanActivity;
    private BLE_ViewModel viewModel;

    float Vbatterymedia;
    //FIR average filter
    public float[] VbatteryAverage;
    public int VbatteryAverageCounter=0;
    //IIR biquad Filter
    public float VbatteryIIROut,VbatteryFilter_out;
    public IIR_Biquads VbatterymediaIIR_Biquads;

    public float[] circularBuffBattColor = new float[30];
    public int icircularBuffBattColor=0;
    public float Vbatterycolor=3.5F;

    public float VbatteryL = 0,VbatteryLout;
    public IIR_Biquads VbatteryLIIR_Biquads;
    public float VbatteryR = 0,VbatteryRout;
    public IIR_Biquads VbatteryRIIR_Biquads;


    public int TachimetroL = 0;
    public int TachimetroR = 0;
    public float Amp_HoursL = 0;
    public float Amp_HoursR = 0;
    public float DutyCycle_L = 0;
    public float DutyCycle_R = 0;
    public float Current_INL = 0;
    public float Current_INR = 0;
    public short RPM_L = 0;
    public short RPM_R = 0;
    public short JOYBattery = 0;



    public TextView idLabelCurrentLeft;
    public TextView idLabelBatteryVolt;
    public TextView idLabelCurrentRight;

    public ProgressBar progressBarCurrentMleftPOS;
    public ProgressBar progressBarCurrentMleftNEG;
    public ProgressBar progressBarCurrentMlrightPOS;
    public ProgressBar progressBarCurrentMrightNEG;

    public ProgressBar idProgressBarBatteryNEG;
    public ProgressBar idProgressBarBatteryPOS;
    public TextView idLabelCurrentBattery;

    public ProgressBar progressBarDutyleftPOS;
    public ProgressBar progressBarDutyLeftNEG;
    public TextView idLabelDutyCycleL;
    public ProgressBar progressBarDutyRightPOS;
    public ProgressBar progressBarDutyRightNEG;
    public TextView idLabelDutyCycleR;

    public TextView idLabelTempLeftFet;
    public TextView idLabelTempLeftMotor;
    public TextView idLabelTempRightFet;
    public TextView idLabelTempRightMotor;

    public  ProgressBar progressBarVoltBattery;
    public  TextView idLabelBatteryPercent;

    public TextView idLabelJoyBattery;
    public ProgressBar ProgressBarJoyBattery;
    public TextView textViewPercJoyBatt;

    private Intent mainActivity;






    public TextView idLabelDistanceMeter;
    public TextView idLabelAmperOra;

    public ArcGauge halfGauge;





   //public Button buttondebug;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);

        VbatterymediaIIR_Biquads = new IIR_Biquads( 0.0004259715733236303,0.0008519431466472606, 0.0004259715733236303,
                -1.9407775999921872 * -1,  0.9424814862854817 * -1);
        VbatterymediaIIR_Biquads.setValues(25.0F);

        //Fs 3Hz; Fc 0.02Hz;
        VbatteryLIIR_Biquads = new IIR_Biquads( 0.0004259715733236303,0.0008519431466472606, 0.0004259715733236303,
                -1.9407775999921872 * -1,  0.9424814862854817 * -1);

        VbatteryRIIR_Biquads = new IIR_Biquads( 0.0004259715733236303,0.0008519431466472606, 0.0004259715733236303,
                -1.9407775999921872 * -1,  0.9424814862854817 * -1);


        final Intent intent = getIntent();
        final Device device = intent.getParcelableExtra(EXTRA_DEVICE);
        final String whocall = intent.getStringExtra("no.nordicsemi.android.blinky.main2start");
        ScanActivity = new Intent(this, Scan.class);
        final String deviceName = device.getName();
        final String deviceAddress = device.getAddress();
        VbatteryAverage = new float[10];



        // This callback will only be called when MyFragment is at least Started.
        OnBackPressedCallback callback = new OnBackPressedCallback(true /* enabled by default */) {
            @Override
            public void handleOnBackPressed() {
                // Handle the back button event
            }
        };
        this.getOnBackPressedDispatcher().addCallback(this, callback);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);




                viewModel = new ViewModelProvider(this).get(BLE_ViewModel.class);
        viewModel.connect(device);
        viewModel.getConnectionState().observe(this, this::State_Connection );
        viewModel.getConnectionStateGATT().observe(this, this::State_ConnectionGATT );

        //-----------------------------UI
        idLabelCurrentLeft = (TextView) findViewById(R.id.idLabelCurrentLeft);
        idLabelCurrentLeft.setText("-.- A");
        idLabelBatteryVolt = (TextView) findViewById(R.id.idLabelBatteryVolt);
        idLabelBatteryVolt.setText("-.- V");
        idLabelCurrentRight = (TextView) findViewById(R.id.idLabelCurrentRight);
        idLabelCurrentRight.setText("-.- A");

        progressBarCurrentMleftPOS = (ProgressBar)findViewById(R.id.progressBarCurrentMleftPOS);
        progressBarCurrentMleftPOS.setProgress(0);
        progressBarCurrentMleftNEG = (ProgressBar)findViewById(R.id.progressBarCurrentMleftNEG);
        progressBarCurrentMleftNEG.setProgress(0);
        progressBarCurrentMlrightPOS = (ProgressBar)findViewById(R.id.progressBarCurrentMlrightPOS);
        progressBarCurrentMlrightPOS.setProgress(0);
        progressBarCurrentMrightNEG = (ProgressBar)findViewById(R.id.progressBarCurrentMrightNEG);
        progressBarCurrentMrightNEG.setProgress(0);


        idProgressBarBatteryNEG = (ProgressBar)findViewById(R.id.idProgressBarBatteryNEG);
        idProgressBarBatteryNEG.setProgress(0);
        idProgressBarBatteryPOS = (ProgressBar)findViewById(R.id.idProgressBarBatteryPOS);
        idProgressBarBatteryPOS.setProgress(0);
        idLabelCurrentBattery = (TextView)findViewById(R.id.idLabelCurrentBattery);
        idLabelCurrentBattery.setText("-.- A");

        progressBarDutyleftPOS = (ProgressBar)findViewById(R.id.progressBarDutyleftPOS);
        progressBarDutyleftPOS.setProgress(0);
        progressBarDutyLeftNEG = (ProgressBar)findViewById(R.id.progressBarDutyLeftNEG);
        progressBarDutyLeftNEG.setProgress(0);
        idLabelDutyCycleL = (TextView)findViewById(R.id.idLabelDutyCycleL);
        idLabelDutyCycleL.setText("-- %");

        progressBarDutyRightPOS = (ProgressBar)findViewById(R.id.progressBarDutyRightPOS);
        progressBarDutyRightPOS.setProgress(0);
        progressBarDutyRightNEG = (ProgressBar)findViewById(R.id.progressBarDutyRightNEG);
        progressBarDutyRightNEG.setProgress(0);
        idLabelDutyCycleR = (TextView)findViewById(R.id.idLabelDutyCycleR);
        idLabelDutyCycleR.setText("-- %");


        idLabelTempLeftFet = (TextView)findViewById(R.id.idLabelTempLeftFet);
        idLabelTempLeftFet.setText("-- °C");
        idLabelTempLeftMotor = (TextView)findViewById(R.id.idLabelTempLeftMotor);
        idLabelTempLeftMotor.setText("-- °C");
        idLabelTempRightFet = (TextView)findViewById(R.id.idLabelTempRightFet);
        idLabelTempRightFet.setText("-- °C");
        idLabelTempRightMotor = (TextView)findViewById(R.id.idLabelTempRightMotor);
        idLabelTempRightMotor.setText("-- °C");

        progressBarVoltBattery = (ProgressBar)findViewById(R.id.progressBarVoltBattery);
        progressBarVoltBattery.setProgress(0);
        idLabelBatteryPercent = (TextView)findViewById(R.id.idLabelBatteryPercent);
        idLabelBatteryPercent.setText("-- %");


        idLabelJoyBattery = (TextView)findViewById(R.id.idLabelJoyBattery);
        idLabelJoyBattery.setText("--");
        ProgressBarJoyBattery = (ProgressBar)findViewById(R.id.idProgressBarJoyBattery);
        ProgressBarJoyBattery.setProgress(0);
        textViewPercJoyBatt = (TextView)findViewById(R.id.textViewPercJoyBatt);
        textViewPercJoyBatt.setText("-- %");



















        idLabelDistanceMeter = (TextView) findViewById(R.id.idLabelDistanceMeter);
        idLabelDistanceMeter.setText("--- m");
        idLabelAmperOra = (TextView)findViewById(R.id.idLabelAmperOra);
        idLabelAmperOra.setText("-- A/h");
        halfGauge = new ArcGauge(Main2_activity.this);
        halfGauge = (ArcGauge) findViewById(R.id.idLabelRPMGauge);
        Range range = new Range();
        range.setColor(Color.parseColor("#ce0000"));
        range.setFrom(0.0);
        range.setTo(500.0);

        Range range2 = new Range();
        range2.setColor(Color.parseColor("#E3E500"));
        range2.setFrom(500.0);
        range2.setTo(1200.0);

        Range range3 = new Range();
        range3.setColor(Color.parseColor("#00b20b"));
        range3.setFrom(1200.0);
        range3.setTo(3000.0);
        //add color ranges to gauge
        halfGauge.addRange(range);
        halfGauge.addRange(range2);
        halfGauge.addRange(range3);
        //set min max and current value
        halfGauge.setMinValue(0);
        halfGauge.setMaxValue(3000);
        halfGauge.setValue(0);
        halfGauge.setEnabled(true);
        halfGauge.setValueColor(Color.WHITE);


/*
        buttondebug = new Button(Main2_activity.this);
        buttondebug = (Button) findViewById(R.id.button);
        buttondebug.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                halfGauge.setValue(75);
                Log.i("TEST", "buttonnnnnnnnnnnnnnn");
                Toast.makeText(getBaseContext(), "a", Toast.LENGTH_LONG).show();
            }
        });
*/





        byte[] fvalue =  new byte[4];
        byte[] int16_value =  new byte[2];
        byte[] int32_value =  new byte[4];

        viewModel.getTelemetriaState().observe(this, value ->{
            if(value.length > 0) {
                switch (value[0]) {
                    case 'a':
                        //VESC_DATACAN_CAN_PACKET_STATUS_1_Left
                        int16_value[0]=value[1];
                        int16_value[1]=value[2];
                        RPM_L = ByteBuffer.wrap(int16_value).order(ByteOrder.LITTLE_ENDIAN).getShort();

                        fvalue[0]=value[3];
                        fvalue[1]=value[4];
                        fvalue[2]=value[5];
                        fvalue[3]=value[6];
                        float CurrentM_L = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
//                        String spercentage = String.format("%.2f%%",percentage);
                        idLabelCurrentLeft.setText( String.format("%.2f",CurrentM_L) + " A");
                        int[] p = double_progressbar_float(CurrentM_L, 10);
                        progressBarCurrentMleftPOS.setProgress(p[0]);
                        progressBarCurrentMleftNEG.setProgress(p[1]);

                        fvalue[0]=value[7];
                        fvalue[1]=value[8];
                        fvalue[2]=value[9];
                        fvalue[3]=value[10];
                        DutyCycle_L = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();

                        idLabelDutyCycleL.setText( String.format("%.1f",DutyCycle_L) + " %");
                        int[] p3 = double_progressbar_float(DutyCycle_L, 10);
                        progressBarDutyleftPOS.setProgress(p3[0]);
                        progressBarDutyLeftNEG.setProgress(p3[1]);
                        break;
                    case 'b':
                        //VESC_DATACAN_CAN_PACKET_STATUS_1_Right
                        int16_value[0]=value[1];
                        int16_value[1]=value[2];
                        RPM_R = ByteBuffer.wrap(int16_value).order(ByteOrder.LITTLE_ENDIAN).getShort();
                        float RPM_Media = (RPM_L + RPM_R) / 2;
                        halfGauge.setValue((int)RPM_Media);

                        fvalue[0]=value[3];
                        fvalue[1]=value[4];
                        fvalue[2]=value[5];
                        fvalue[3]=value[6];
                        float CurrentM_R = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();

                        idLabelCurrentRight.setText( String.format("%.1f",CurrentM_R) + " A");
                        int[] p1 = double_progressbar_float(CurrentM_R, 10);
                        progressBarCurrentMlrightPOS.setProgress(p1[0]);
                        progressBarCurrentMrightNEG.setProgress(p1[1]);

                        fvalue[0]=value[7];
                        fvalue[1]=value[8];
                        fvalue[2]=value[9];
                        fvalue[3]=value[10];
                        DutyCycle_R = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        idLabelDutyCycleR.setText( String.format("%.1f",DutyCycle_R) + " %");
                        int[] p4 = double_progressbar_float(DutyCycle_R, 10);
                        progressBarDutyRightPOS.setProgress(p4[0]);
                        progressBarDutyRightNEG.setProgress(p4[1]);
                        break;
                    case 'c':
                        //VESC_DATACAN_CAN_PACKET_STATUS_2_Left
                        fvalue[0]=value[1];
                        fvalue[1]=value[2];
                        fvalue[2]=value[3];
                        fvalue[3]=value[4];
                        Amp_HoursL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
      //      Log.i("TELEMETRIA", "Amp_HoursL=" + String.format("%.4f",Amp_HoursL));

                        break;
                    case 'd':
                        //VESC_DATACAN_CAN_PACKET_STATUS_2_Rihght
                        fvalue[0]=value[1];
                        fvalue[1]=value[2];
                        fvalue[2]=value[3];
                        fvalue[3]=value[4];
                        Amp_HoursR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();

           // Log.i("TELEMETRIA", "Amp_HoursR=" + String.format("%.4f",Amp_HoursR));

                       // float Amp_HourMedia = (Amp_HoursL + Amp_HoursR)/2;
                        idLabelAmperOra.setText(String.format("%.3f",Amp_HoursL) + " A/h");
                        break;
                    case 'e':
                        //VESC_DATACAN_CAN_PACKET_STATUS_4_Left
                        fvalue[0]=value[1];
                        fvalue[1]=value[2];
                        fvalue[2]=value[3];
                        fvalue[3]=value[4];
                        float temperature_FETL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        idLabelTempLeftFet.setText( String.format("%.1f",temperature_FETL) + " °C");

                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        float temperature_MOTORL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        idLabelTempLeftMotor.setText( String.format("%.1f",temperature_MOTORL) + " °C");

                        fvalue[0]=value[9];
                        fvalue[1]=value[10];
                        fvalue[2]=value[11];
                        fvalue[3]=value[12];
                        Current_INL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        break;
                    case 'f':
                        //VESC_DATACAN_CAN_PACKET_STATUS_4_Right
                        fvalue[0]=value[1];
                        fvalue[1]=value[2];
                        fvalue[2]=value[3];
                        fvalue[3]=value[4];
                        float temperature_FETR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        idLabelTempRightFet.setText( String.format("%.1f",temperature_FETR) + " °C");

                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        float temperature_MOTORR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        idLabelTempRightMotor.setText( String.format("%.1f",temperature_MOTORR) + " °C");

                        fvalue[0]=value[9];
                        fvalue[1]=value[10];
                        fvalue[2]=value[11];
                        fvalue[3]=value[12];
                        Current_INR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        float CurrentINmedia = (Current_INL+Current_INR)/2;
                        idLabelCurrentBattery.setText(String.format("%.1f",CurrentINmedia) + " A");
                        int[] p2 = double_progressbar_float(CurrentINmedia, 10);
                        idProgressBarBatteryPOS.setProgress(p2[0]);
                        idProgressBarBatteryNEG.setProgress(p2[1]);
                        break;

                    case 'g':
                        //VESC_DATACAN_CAN_PACKET_STATUS_5_Left
                        int32_value[0]=value[1];
                        int32_value[1]=value[2];
                        int32_value[2]=value[3];
                        int32_value[3]=value[4];
                        TachimetroL = ByteBuffer.wrap(int32_value).order(ByteOrder.LITTLE_ENDIAN).getInt();
//                        Log.i("TELEMETRIA", "TachimetroL=" + TachimetroL);

                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        VbatteryL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();

                        VbatteryLIIR_Biquads.setValues(VbatteryL);
                        VbatteryLout = VbatteryLIIR_Biquads.Calc_IIR(VbatteryL);

                        //Log.i("TELEMETRIA", "VbatteryLout=" + String.format("%.4f",VbatteryLout));
                        break;

                    case 'h':
                        //VESC_DATACAN_CAN_PACKET_STATUS_5_Right
                        int32_value[0]=value[1];
                        int32_value[1]=value[2];
                        int32_value[2]=value[3];
                        int32_value[3]=value[4];
                        TachimetroR = ByteBuffer.wrap(int32_value).order(ByteOrder.LITTLE_ENDIAN).getInt();
                        int Tachimetromedia = (TachimetroL + TachimetroR)/2;
                        float Tachimetromediaf = ((float) Tachimetromedia) / (float)299.3333333;
                        int Tachimetromediafi =  (int)Tachimetromediaf;

                        //Da rivedere x trasformare
                        idLabelDistanceMeter.setText(String.valueOf(Tachimetromediafi ) + " m");
   //                     Log.i("TELEMETRIA", "TachimetroR=" + TachimetroR);

                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        VbatteryR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();

                        VbatteryRIIR_Biquads.setValues(VbatteryR);
                        VbatteryRout = VbatteryRIIR_Biquads.Calc_IIR(VbatteryR);

                        Vbatterymedia = (VbatteryLout + VbatteryRout)/2;

                        /*
                         //FIR Filter
                        VbatteryAverage[VbatteryAverageCounter] = Vbatterymedia;
                        VbatteryAverageCounter++;
                        VbatteryAverageCounter = VbatteryAverageCounter % 10;
                        float vbata=0;
                        for (int i = 0; i < 10; i++) {
                            vbata += VbatteryAverage[VbatteryAverageCounter];
                        }
                        Vbatterymedia = vbata / 10;
                        */

                        //IIR Biquad Filter
                                //https://www.earlevel.com/main/2010/12/20/biquad-calculator/
                                //https://www.earlevel.com/main/2003/02/28/biquads/
                                //https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3/
                        VbatterymediaIIR_Biquads.setValues(Vbatterymedia);
                        VbatteryIIROut = VbatterymediaIIR_Biquads.Calc_IIR(Vbatterymedia);



                        VbatteryFilter_out = VbatteryIIROut;
                        idLabelBatteryVolt.setText(String.format("%.1f",VbatteryIIROut) + " V");
                        if(VbatteryFilter_out>25.200001){
                            VbatteryFilter_out = (float)25.2000;
                        };
                        if(VbatteryFilter_out<19.640000000){
                            VbatteryFilter_out = (float)19.640000000;
                        };
                        double calcPercB = 0.00;
                        if(VbatteryFilter_out > 22.24) {
//Polynomnial
                            calcPercB = ((Math.pow(VbatteryFilter_out, 3) * 1.6921) +
                                    (Math.pow(VbatteryFilter_out, 2) * -126.6400) +
                                    (VbatteryFilter_out * 3176.7377) - 26611.4262);
                        }else if((VbatteryFilter_out > 21.65)&&(VbatteryFilter_out <= 22.24)) {
                            calcPercB = ((Math.pow(VbatteryFilter_out, 2) * 5.107) +
                                    (VbatteryFilter_out * -208.58) + 2126.715);
                        }else{
                            calcPercB = (VbatteryFilter_out-19.64)/2.010*5;
                        }
                        if(calcPercB<0.00) calcPercB = 0.000;

                        //color battery level
                        /*
                        if(VbatteryFilter_out>=23.25){ //60%
                            progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.BLUE));
                        }else if((VbatteryFilter_out<23.25)&&(VbatteryFilter_out>=22.6)){
                            progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.YELLOW));
                        }else if(VbatteryFilter_out<22.6){ //30%
                            progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.RED));
                        }
                        */

                        if(VbatteryFilter_out>=23.25){ //60%
                            circularBuffBattColor[icircularBuffBattColor] = 3.500F;
                            icircularBuffBattColor = icircularBuffBattColor + 1;
                            icircularBuffBattColor = icircularBuffBattColor % 30;

                        }else if((VbatteryFilter_out<23.25)&&(VbatteryFilter_out>=22.6)){
                            circularBuffBattColor[icircularBuffBattColor] = 2.500F;
                            icircularBuffBattColor = icircularBuffBattColor + 1;
                            icircularBuffBattColor = icircularBuffBattColor % 30;

                        }else if(VbatteryFilter_out<22.6){ //30%
                            circularBuffBattColor[icircularBuffBattColor] = 1.500F;
                            icircularBuffBattColor = icircularBuffBattColor + 1;
                            icircularBuffBattColor = icircularBuffBattColor % 30;
                        }


                        float vbata=0;
                        for (int i = 0; i < 30; i++) {
                            vbata += circularBuffBattColor[i];
                        }
                        Vbatterycolor = vbata / 30;
                        if((Vbatterycolor>=3.000)&(Vbatterycolor<4.000)){ //60%
                            progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.BLUE));
                        }else if((Vbatterycolor>=2.000)&(Vbatterycolor<3.000)){
                            progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.YELLOW));
                        }else if(Vbatterycolor<2.000){ //30%
                            progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.RED));
                        }














                        progressBarVoltBattery.setProgress((int)calcPercB);
                        idLabelBatteryPercent.setText(String.format("%d", (int)calcPercB) + " %");

//                        Log.i("TELEMETRIA", "VbatteryR=" + String.format("%.4f",VbatteryR));

                        break;


                    case 't':
                        //JOYSTICK_BATTERY_DATA_telemetry
                        int16_value[0]=value[1];
                        int16_value[1]=value[2];
                        JOYBattery = ByteBuffer.wrap(int16_value).order(ByteOrder.LITTLE_ENDIAN).getShort();
                        idLabelJoyBattery.setText(String.format("%.2f", (float)JOYBattery/1000.00) + " V");

                        //1.00*2 = 2.0 V min  --- 1.5*2 = 3 V max
                        float Vbattjoy = (float) ((float)JOYBattery/1000.00);
                        if(Vbattjoy > 3.00){
                            Vbattjoy = 3.00F;
                        }
                        if(Vbattjoy < 2.00){
                            Vbattjoy = 2.00F;
                        }
                        float calcPercBjoy = (Vbattjoy - 2.0F) * (100/(3.0F - 2.0F));
                        ProgressBarJoyBattery.setProgress((int)calcPercBjoy);
                        textViewPercJoyBatt.setText(String.format("%d", (int)calcPercBjoy) + " %");

                        //Log.i("TELEMETRIA", "JOYBattery=" + JOYBattery);

                        break;


                }




            }
        });



        mainActivity = new Intent(this, MainActivity.class);

    }





    public int[] double_progressbar_float(float value, float moltiplicatore){
        int[] bar = new int[2];

        if(value>0){
            bar[0] = (int) (value * moltiplicatore);
            bar[1] = 0;
        }else  if (value<0){
            bar[0] = 0;
            bar[1] = (int) (abs(value) * moltiplicatore);
        }else {
            bar[0] = 0;
            bar[1] = 0;
        }
        return bar;
    }






















    //CallBack observer livedata ---> stato connessione
    private void State_Connection(ConnectionState connectionStateLiveData){
        Log.d(TAG, "Connessione Stato: " + connectionStateLiveData.toString());
    }
    private void State_ConnectionGATT(Boolean connectionStateLiveData){
        //Toast.makeText(this,"State_ConnectionGATT " + connectionStateLiveData.toString() , Toast.LENGTH_LONG).show();
        if(connectionStateLiveData==false){
            mainActivity.putExtra(MainActivity.EXTRA_DEVICE2, "exit");
            startActivity(mainActivity);

            //finish();
            //android.os.Process.killProcess(android.os.Process.myPid());
            //System.exit(0);
        }

    }


    @Override
    public void onBackPressed() {
        super.onBackPressed();



    }















}