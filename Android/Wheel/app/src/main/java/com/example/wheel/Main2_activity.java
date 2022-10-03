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



    public float VbatteryL = 0;
    public float VbatteryR = 0;
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








    public TextView idLabelDistanceMeter;
    public TextView idLabelAmperOra;

    public ArcGauge halfGauge;





   //public Button buttondebug;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);


        final Intent intent = getIntent();
        final Device device = intent.getParcelableExtra(EXTRA_DEVICE);
        final String whocall = intent.getStringExtra("no.nordicsemi.android.blinky.main2start");
        ScanActivity = new Intent(this, Scan.class);
        final String deviceName = device.getName();
        final String deviceAddress = device.getAddress();


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
        idLabelCurrentBattery = (TextView)findViewById(R.id.idLabelDutyCycleR);
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
                switch (value[0]){
                    case 'a':
                        //VESC_DATACAN_CAN_PACKET_STATUS_1_Left
                        int16_value[0]=value[1];
                        int16_value[1]=value[2];
                        RPM_L = ByteBuffer.wrap(int16_value).order(ByteOrder.LITTLE_ENDIAN).getShort();
halfGauge.setValue((int)RPM_L); //DEBUG

                        fvalue[0]=value[3];
                        fvalue[1]=value[4];
                        fvalue[2]=value[5];
                        fvalue[3]=value[6];
                        float CurrentM_L = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
//                        String spercentage = String.format("%.2f%%",percentage);
                        // idLabelCurrentLeft.setText( String.valueOf(CurrentM_L) + " A");
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
//                        halfGauge.setValue((int)RPM_Media);

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
idLabelAmperOra.setText(String.format("%.2f",Amp_HoursL) + " A/h"); //DEBUG

                        break;
                    case 'd':
                        //VESC_DATACAN_CAN_PACKET_STATUS_2_Rihght
                        fvalue[0]=value[1];
                        fvalue[1]=value[2];
                        fvalue[2]=value[3];
                        fvalue[3]=value[4];
                        Amp_HoursR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        float Amp_HourMedia = (Amp_HoursL + Amp_HoursR)/2;
//                        idLabelAmperOra.setText(String.format("%.2f",Amp_HourMedia) + " A/h");

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
idLabelCurrentBattery.setText(String.format("%.1f",Current_INL) + " A");    //DEBUG
int[] p6 = double_progressbar_float(Current_INL, 10);    //DEBUG
idProgressBarBatteryPOS.setProgress(p6[0]);    //DEBUG
idProgressBarBatteryNEG.setProgress(p6[1]);    //DEBUG


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
//                        idLabelCurrentBattery.setText(String.format("%.1f",CurrentINmedia) + " A");
//                        int[] p2 = double_progressbar_float(CurrentINmedia, 10);
//                        idProgressBarBatteryPOS.setProgress(p2[0]);
//                        idProgressBarBatteryNEG.setProgress(p2[1]);
//                        break;
                    case 'g':
                        //VESC_DATACAN_CAN_PACKET_STATUS_5_Left
                        int32_value[0]=value[1];
                        int32_value[1]=value[2];
                        int32_value[2]=value[3];
                        int32_value[3]=value[4];
                        TachimetroL = ByteBuffer.wrap(int32_value).order(ByteOrder.LITTLE_ENDIAN).getInt();
idLabelDistanceMeter.setText(String.valueOf(TachimetroL ) + " m"); //DEBUG

                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        VbatteryL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
idLabelBatteryVolt.setText(String.format("%.1f",VbatteryL) + " V"); //DEBUG
if(VbatteryL>25.200001){
    VbatteryL = (float)25.2000;
};
if(VbatteryL<19.640000000){
    VbatteryL = (float)19.640000000;
};
double calcPerc = 0.00;
if(VbatteryL > 22.24) {
//Polynomnial
    calcPerc = ((Math.pow(VbatteryL, 3) * 1.6921) +
            (Math.pow(VbatteryL, 2) * -126.6400) +
            (VbatteryL * 3176.7377) - 26611.4262);
}else if((VbatteryL > 21.65)&&(VbatteryL <= 22.24)) {
    calcPerc = ((Math.pow(VbatteryL, 2) * 5.107) +
            (VbatteryL * -208.58) + 2126.715);
}else{
    calcPerc = (VbatteryL-19.64)/2.010*5;
}
if(calcPerc<0.00) calcPerc = 0.000;

if(VbatteryL>=23.25){ //60% //DEBUG
    progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.BLUE)); //DEBUG
}else if((VbatteryL<23.25)&&(VbatteryL>=22.6)){ //DEBUG
    progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.YELLOW)); //DEBUG
}else if(VbatteryL<22.6){ //30% //DEBUG
    progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.RED)); //DEBUG
} //DEBUG
progressBarVoltBattery.setProgress((int)calcPerc); //DEBUG
idLabelBatteryPercent.setText(String.format("%d", (int)calcPerc) + " %");

                        break;
                    case 'h':
                        //VESC_DATACAN_CAN_PACKET_STATUS_5_Right
                        int32_value[0]=value[1];
                        int32_value[1]=value[2];
                        int32_value[2]=value[3];
                        int32_value[3]=value[4];
                        TachimetroR = ByteBuffer.wrap(int32_value).order(ByteOrder.LITTLE_ENDIAN).getInt();
                        int Tachimetromedia = (TachimetroL + TachimetroR)/2;
                        //Da rivedere x trasformare
//                        idLabelDistanceMeter.setText(String.valueOf(Tachimetromedia ) + " m");

                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        VbatteryR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        float Vbatterymedia = (VbatteryL + VbatteryR)/2;
/*                        idLabelBatteryVolt.setText(String.format("%.1f",Vbatterymedia) + " V");
                        if(Vbatterymedia>25.200001){
                            Vbatterymedia = (float)25.2000;
                        };
                        if(Vbatterymedia<19.640000000){
                            Vbatterymedia = (float)19.640000000;
                        };
                        double calcPercB = 0.00;
                        if(Vbatterymedia > 22.24) {
//Polynomnial
                            calcPercB = ((Math.pow(Vbatterymedia, 3) * 1.6921) +
                                    (Math.pow(Vbatterymedia, 2) * -126.6400) +
                                    (Vbatterymedia * 3176.7377) - 26611.4262);
                        }else if((Vbatterymedia > 21.65)&&(Vbatterymedia <= 22.24)) {
                            calcPercB = ((Math.pow(Vbatterymedia, 2) * 5.107) +
                                    (Vbatterymedia * -208.58) + 2126.715);
                        }else{
                            calcPercB = (Vbatterymedia-19.64)/2.010*5;
                        }
                        if(calcPercB<0.00) calcPercB = 0.000;


                        if(Vbatterymedia>=23.25){ //60%
                            progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.BLUE));
                        }else if((Vbatterymedia<23.25)&&(Vbatterymedia>=22.6)){
                            progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.YELLOW));
                        }else if(Vbatterymedia<22.6){ //30%
                            progressBarVoltBattery.setProgressTintList(ColorStateList.valueOf(Color.RED));
                        }
                        progressBarVoltBattery.setProgress((int)calcPercB);
                        idLabelBatteryPercent.setText(String.format("%d", (int)calcPercB) + " %");

*/


                        break;
                    default:
                        break;
                }



            }


        });
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

    }


    @Override
    public void onBackPressed() {
        super.onBackPressed();



    }















}