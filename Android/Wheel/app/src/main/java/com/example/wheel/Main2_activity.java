package com.example.wheel;


import androidx.activity.OnBackPressedCallback;
import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProvider;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.ekn.gruzer.gaugelibrary.ArcGauge;
import com.ekn.gruzer.gaugelibrary.Range;
import com.example.wheel.Ble_Manager.BLE_ViewModel;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

import no.nordicsemi.android.ble.livedata.state.ConnectionState;

import static android.content.ContentValues.TAG;



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
    public ProgressBar idProgressBarBattery;
    public TextView idLabelDistanceMeter;
    public TextView idLabelAmperOra;
    public TextView idLabelDutyCycle;
    public ProgressBar progressBarDuty;
    public TextView idLabelCurrentBattery;
    public ProgressBar progressBarCurrentBattery;
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
        idLabelCurrentLeft.setText("0.0 A");
        idLabelBatteryVolt = (TextView) findViewById(R.id.idLabelBatteryVolt);
        idLabelBatteryVolt.setText("0.0 V");
        idLabelCurrentRight = (TextView) findViewById(R.id.idLabelCurrentRight);
        idLabelCurrentRight.setText("0.0 A");
        idProgressBarBattery = (ProgressBar) findViewById(R.id.idProgressBarBattery);
        idProgressBarBattery.setProgress(0);
        idLabelDistanceMeter = (TextView) findViewById(R.id.idLabelDistanceMeter);
        idLabelDistanceMeter.setText("0 m");
        idLabelAmperOra = (TextView)findViewById(R.id.idLabelAmperOra);
        idLabelAmperOra.setText("12 A/h");
        idLabelDutyCycle = (TextView)findViewById(R.id.idLabelDutyCycle);
        idLabelDutyCycle.setText("23 %");
        progressBarDuty = (ProgressBar)findViewById(R.id.progressBarDuty);
        progressBarDuty.setProgress(0);
        idLabelCurrentBattery = (TextView)findViewById(R.id.idLabelCurrentBattery);
        idLabelCurrentBattery.setText("0.0 A");
        progressBarCurrentBattery = (ProgressBar)findViewById(R.id.progressBarCurrentBattery);
        progressBarCurrentBattery.setProgress(0);
        halfGauge = new ArcGauge(Main2_activity.this);
        halfGauge = (ArcGauge) findViewById(R.id.idLabelRPMGauge);
        Range range = new Range();
        range.setColor(Color.parseColor("#ce0000"));
        range.setFrom(0.0);
        range.setTo(50.0);

        Range range2 = new Range();
        range2.setColor(Color.parseColor("#E3E500"));
        range2.setFrom(50.0);
        range2.setTo(100.0);

        Range range3 = new Range();
        range3.setColor(Color.parseColor("#00b20b"));
        range3.setFrom(100.0);
        range3.setTo(150.0);
        //add color ranges to gauge
        halfGauge.addRange(range);
        halfGauge.addRange(range2);
        halfGauge.addRange(range3);
        //set min max and current value
        halfGauge.setMinValue(0);
        halfGauge.setMaxValue(100);
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

                        fvalue[0]=value[3];
                        fvalue[1]=value[4];
                        fvalue[2]=value[5];
                        fvalue[3]=value[6];
                        float CurrentM_L = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
//                        String spercentage = String.format("%.2f%%",percentage);
                        // idLabelCurrentLeft.setText( String.valueOf(CurrentM_L) + " A");
                        idLabelCurrentLeft.setText( String.format("%.2f",CurrentM_L) + " A");

                        fvalue[0]=value[7];
                        fvalue[1]=value[8];
                        fvalue[2]=value[9];
                        fvalue[3]=value[10];
                        DutyCycle_L = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
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

                        fvalue[0]=value[7];
                        fvalue[1]=value[8];
                        fvalue[2]=value[9];
                        fvalue[3]=value[10];
                        DutyCycle_R = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        float DutyMedia = (DutyCycle_L + DutyCycle_R)/2;
                        idLabelDutyCycle.setText( String.format("%.1f",DutyMedia) + " %");
                        progressBarDuty.setProgress((int)DutyMedia);

                        break;
                    case 'c':
                        //VESC_DATACAN_CAN_PACKET_STATUS_2_Left
                        fvalue[0]=value[1];
                        fvalue[1]=value[2];
                        fvalue[2]=value[3];
                        fvalue[3]=value[4];
                        Amp_HoursL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();

                        break;
                    case 'd':
                        //VESC_DATACAN_CAN_PACKET_STATUS_2_Rihght
                        fvalue[0]=value[1];
                        fvalue[1]=value[2];
                        fvalue[2]=value[3];
                        fvalue[3]=value[4];
                        Amp_HoursR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        float Amp_HourMedia = (Amp_HoursL + Amp_HoursR)/2;
                        idLabelAmperOra.setText(String.format("%.2f",Amp_HourMedia) + " A/h");

                        break;
                    case 'e':
                        //VESC_DATACAN_CAN_PACKET_STATUS_4_Left
                        fvalue[0]=value[1];
                        fvalue[1]=value[2];
                        fvalue[2]=value[3];
                        fvalue[3]=value[4];
                        float temperature_FETL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        float temperature_MOTORL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
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
                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        float temperature_MOTORR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        fvalue[0]=value[9];
                        fvalue[1]=value[10];
                        fvalue[2]=value[11];
                        fvalue[3]=value[12];
                        Current_INR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        float CurrentINmedia = (Current_INL+Current_INR)/2;
                        idLabelCurrentBattery.setText(String.format("%.1f",CurrentINmedia) + " A");
                        progressBarCurrentBattery.setProgress((int)CurrentINmedia);

                        break;
                    case 'g':
                        //VESC_DATACAN_CAN_PACKET_STATUS_5_Left
                        int32_value[0]=value[1];
                        int32_value[1]=value[2];
                        int32_value[2]=value[3];
                        int32_value[3]=value[4];
                        TachimetroL = ByteBuffer.wrap(int32_value).order(ByteOrder.LITTLE_ENDIAN).getInt();

                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        VbatteryL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();

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
                        idLabelDistanceMeter.setText(String.valueOf(Tachimetromedia ) + " m");

                        fvalue[0]=value[5];
                        fvalue[1]=value[6];
                        fvalue[2]=value[7];
                        fvalue[3]=value[8];
                        VbatteryR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                        float Vbatterymedia = (VbatteryL + VbatteryR)/2;
                        idLabelBatteryVolt.setText(String.format("%.1f",Vbatterymedia) + " V");
                        float calcPerc = (float) (((Vbatterymedia - 19.64) * 0.0556) * 100);
                        idProgressBarBattery.setProgress((int)calcPerc);

                        break;
                    default:
                        break;
                }



            }


        });
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