package com.example.wheels;

import static java.lang.Math.abs;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.ProgressBar;
import android.widget.TextView;


import androidx.core.view.GestureDetectorCompat;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.ViewModelProvider;
import androidx.viewpager.widget.ViewPager;

import com.ekn.gruzer.gaugelibrary.ArcGauge;
import com.ekn.gruzer.gaugelibrary.Range;
import com.example.wheels.Ble_Manager.BLE_ViewModel;
//import com.example.wheels.databinding.ActivityMain2Binding;
//import com.google.android.gms.wearable.Wearable;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import no.nordicsemi.android.ble.livedata.state.ConnectionState;

public class MainActivity2 extends FragmentActivity { //Activity ,FragmentActivity

  // private ActivityMain2Binding binding;
    private GestureDetectorCompat mDetector;

    public static final String EXTRA_DEVICE = "no.nordicsemi.android.blinky.EXTRA_DEVICE";

    private Intent ScanActivity;
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


//Fragment1
    public TextView textViewCurrentMLeft;
    public ProgressBar progressBarCurrentMLeftTop;
    public ProgressBar progressBarCurrentMLeftBotton;

    public TextView textViewCurrentMRight;
    public ProgressBar progressBarCurrentMRightTop;
    public ProgressBar progressBarCurrentMRightBotton;

    public TextView textViewBattery;
    public ProgressBar progressBarBattery;

    public TextView textViewDistance;
    public TextView textViewDuty;
    public TextView textViewAmpHour;

    public TextView textViewAmpBattery;
    public ProgressBar progressBarCurrentBatteryneg;
    public ProgressBar progressBarCurrentBatterypos;

    public ArcGauge idLabelRPMGauge;

    //Fragment2
    public TextView textViewFETLeft;
    public TextView textViewMotorTempLeft;
    public TextView textViewFETRight;
    public TextView textViewMotorTempRight;








  public ViewPager viewPager;



  @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);


        final Intent intent = getIntent();
        final Device device = intent.getParcelableExtra(EXTRA_DEVICE);
        final String whocall = intent.getStringExtra("no.nordicsemi.android.blinky.main2start");
        ScanActivity = new Intent(this, MainActivity.class);
        final String deviceName = device.getName();
        final String deviceAddress = device.getAddress();



        viewModel = new ViewModelProvider(this ).get(BLE_ViewModel.class);
        viewModel.connect(device);
        viewModel.getConnectionState().observe(this, this::State_Connection );
        viewModel.getConnectionStateGATT().observe(this, this::State_ConnectionGATT );


        viewPager = (ViewPager) findViewById(R.id.viewpager);
        viewPager.setAdapter(new CustomPagerAdapter(this));
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        viewPager.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {

          @Override
          public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {

            Log.i("POSITION", String.valueOf(position) + "  ---> *****"   );
            switch (position){
              case 0:
                //-----------------------------UI
                textViewCurrentMLeft = (TextView) findViewById(R.id.textViewCurrentMLeft);
                progressBarCurrentMLeftTop = (ProgressBar)findViewById(R.id.progressBarCurrentMLeftTop);
                progressBarCurrentMLeftBotton = (ProgressBar)findViewById(R.id.progressBarCurrentMLeftBotton);
                textViewCurrentMLeft.setText("--.- A");
                progressBarCurrentMLeftTop.setProgress(0);
                progressBarCurrentMLeftBotton.setProgress(0);


                textViewCurrentMRight = (TextView) findViewById(R.id.textViewCurrentMRight);
                progressBarCurrentMRightTop = (ProgressBar)findViewById(R.id.progressBarCurrentMRightTop);
                progressBarCurrentMRightBotton = (ProgressBar)findViewById(R.id.progressBarCurrentMRightBotton);
                textViewCurrentMRight.setText("--.- A");
                progressBarCurrentMRightTop.setProgress(0);
                progressBarCurrentMRightBotton.setProgress(0);


                textViewBattery = (TextView) findViewById(R.id.textViewBattery);
                progressBarBattery = (ProgressBar)findViewById(R.id.progressBarBattery);
                textViewBattery.setText("--.- V");
                progressBarBattery.setProgress(0);

                textViewDistance = (TextView) findViewById(R.id.textViewDistance);
                textViewDuty = (TextView) findViewById(R.id.textViewDuty);
                textViewAmpHour = (TextView) findViewById(R.id.textViewAmpHour);
                textViewDistance.setText("--- m");
                textViewDuty.setText("-- %");
                textViewAmpHour.setText("---");

                textViewAmpBattery = (TextView) findViewById(R.id.textViewAmpBattery);
                progressBarCurrentBatteryneg = (ProgressBar)findViewById(R.id.progressBarCurrentBatteryneg);
                progressBarCurrentBatterypos = (ProgressBar)findViewById(R.id.progressBarCurrentBatterypos);
                textViewAmpBattery.setText("-.- A");
                progressBarCurrentBatteryneg.setProgress(0);
                progressBarCurrentBatterypos.setProgress(0);

                idLabelRPMGauge = (ArcGauge) findViewById(R.id.idLabelRPMGauge);
                Range range = new Range();
                range.setColor(Color.parseColor("#ce0000"));
                range.setFrom(0.0);
                range.setTo(500.0);

                Range range2 = new Range();
                range2.setColor(Color.parseColor("#E3E500"));
                range2.setFrom(500.0);
                range2.setTo(1200.0);

                Range range3 = new Range();
                range3.setColor(Color.parseColor("#df2b1e"));
                range3.setFrom(1200.0);
                range3.setTo(2500.0);
                //add color ranges to gauge
                idLabelRPMGauge.addRange(range);
                idLabelRPMGauge.addRange(range2);
                idLabelRPMGauge.addRange(range3);
                //set min max and current value
                idLabelRPMGauge.setMinValue(0);
                idLabelRPMGauge.setMaxValue(2500);
                idLabelRPMGauge.setValue(1000);
                idLabelRPMGauge.setEnabled(true);
                idLabelRPMGauge.setValueColor(Color.WHITE);

                break;
              case 1:
                textViewFETLeft = (TextView) findViewById(R.id.textViewFETLeft);
                textViewMotorTempLeft = (TextView) findViewById(R.id.textViewMotorTempLeft);
                textViewFETRight = (TextView) findViewById(R.id.textViewFETRight);
                textViewMotorTempRight = (TextView) findViewById(R.id.textViewMotorTempRight);
                break;
              case 2:
                Log.i("MAIN2", "position3");
                break;
              case 3:
                Log.i("MAIN2", "position4");
                break;
            }
          }

          @Override
          public void onPageSelected(int position) {
          }

          @Override
          public void onPageScrollStateChanged(int state) {
          }
        });








      /*
      // setContentView(R.layout.fragment_1);

        //-----------------------------UI
        textViewCurrentMLeft = (TextView) findViewById(R.id.textViewCurrentMLeft);
        progressBarCurrentMLeftTop = (ProgressBar)findViewById(R.id.progressBarCurrentMLeftTop);
        progressBarCurrentMLeftBotton = (ProgressBar)findViewById(R.id.progressBarCurrentMLeftBotton);
        textViewCurrentMLeft.setText("0.0 A");
        progressBarCurrentMLeftTop.setProgress(0);
        progressBarCurrentMLeftBotton.setProgress(0);


        textViewCurrentMRight = (TextView) findViewById(R.id.textViewCurrentMRight);
        progressBarCurrentMRightTop = (ProgressBar)findViewById(R.id.progressBarCurrentMRightTop);
        progressBarCurrentMRightBotton = (ProgressBar)findViewById(R.id.progressBarCurrentMRightBotton);
        textViewCurrentMRight.setText("0.0 A");
        progressBarCurrentMRightTop.setProgress(0);
        progressBarCurrentMRightBotton.setProgress(0);


        textViewBattery = (TextView) findViewById(R.id.textViewBattery);
        progressBarBattery = (ProgressBar)findViewById(R.id.progressBarBattery);
        textViewBattery.setText("0.0 V");
        progressBarBattery.setProgress(0);

        textViewDistance = (TextView) findViewById(R.id.textViewDistance);
        textViewDuty = (TextView) findViewById(R.id.textViewDuty);
        textViewAmpHour = (TextView) findViewById(R.id.textViewAmpHour);
        textViewDistance.setText("0 m");
        textViewDuty.setText("0 %");
        textViewAmpHour.setText("0.0 A/h");

        textViewAmpBattery = (TextView) findViewById(R.id.textViewAmpBattery);
        progressBarCurrentBatteryneg = (ProgressBar)findViewById(R.id.progressBarCurrentBatteryneg);
        progressBarCurrentBatterypos = (ProgressBar)findViewById(R.id.progressBarCurrentBatterypos);
        textViewAmpBattery.setText("0.0 A");
        progressBarCurrentBatteryneg.setProgress(0);
        progressBarCurrentBatterypos.setProgress(0);


*/









      byte[] fvalue =  new byte[4];
      byte[] int16_value =  new byte[2];
      byte[] int32_value =  new byte[4];


      viewModel.getTelemetriaState().observe(this, value ->{
        //Log.i("BLE", "******data****");

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
              if(viewPager.getCurrentItem()==0){
                textViewCurrentMLeft.setText( String.format("%.1f",CurrentM_L) + " A");
                int[] p = double_progressbar_float(CurrentM_L, 10);
                progressBarCurrentMLeftTop.setProgress(p[0]);
                progressBarCurrentMLeftBotton.setProgress(p[1]);
              }

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
              idLabelRPMGauge.setValue((int)RPM_Media);

              fvalue[0]=value[3];
              fvalue[1]=value[4];
              fvalue[2]=value[5];
              fvalue[3]=value[6];
              float CurrentM_R = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
              textViewCurrentMRight.setText( String.format("%.1f",CurrentM_R) + " A");
              int[] p1 = double_progressbar_float(CurrentM_R, 10);
              progressBarCurrentMRightTop.setProgress(p1[0]);
              progressBarCurrentMRightBotton.setProgress(p1[1]);

              fvalue[0]=value[7];
              fvalue[1]=value[8];
              fvalue[2]=value[9];
              fvalue[3]=value[10];
              DutyCycle_R = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
              float DutyMedia = (DutyCycle_L + DutyCycle_R)/2;
              textViewDuty.setText( String.format("%.1f",DutyMedia) + " %");

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
              textViewAmpHour.setText(String.format("%.1f",Amp_HourMedia));

              break;
            case 'e':
              //VESC_DATACAN_CAN_PACKET_STATUS_4_Left
              fvalue[0]=value[1];
              fvalue[1]=value[2];
              fvalue[2]=value[3];
              fvalue[3]=value[4];
              float temperature_FETL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
              if(viewPager.getCurrentItem()==1){
                textViewFETLeft.setText( String.format("%.1f",temperature_FETL) + " °C");
              }

              fvalue[0]=value[5];
              fvalue[1]=value[6];
              fvalue[2]=value[7];
              fvalue[3]=value[8];
              float temperature_MOTORL = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
              if(viewPager.getCurrentItem()==1){
                textViewMotorTempLeft.setText( String.format("%.1f",temperature_MOTORL) + " °C");
              }

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
              if(viewPager.getCurrentItem()==1){
                textViewFETRight.setText( String.format("%.1f",temperature_FETR) + " °C");
              }

              fvalue[0]=value[5];
              fvalue[1]=value[6];
              fvalue[2]=value[7];
              fvalue[3]=value[8];
              float temperature_MOTORR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
              if(viewPager.getCurrentItem()==1){
                textViewMotorTempRight.setText( String.format("%.1f",temperature_MOTORR) + " °C");
              }

              fvalue[0]=value[9];
              fvalue[1]=value[10];
              fvalue[2]=value[11];
              fvalue[3]=value[12];
              Current_INR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
              float CurrentINmedia = (Current_INL+Current_INR)/2;
              textViewAmpBattery.setText(String.format("%.1f",CurrentINmedia) + " A");
              int[] p2 = double_progressbar_float(CurrentINmedia, 10);
              progressBarCurrentBatterypos.setProgress(p2[0]);
              progressBarCurrentBatteryneg.setProgress(p2[1]);

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
              textViewDistance.setText(String.valueOf(Tachimetromedia ) + " m");

              fvalue[0]=value[5];
              fvalue[1]=value[6];
              fvalue[2]=value[7];
              fvalue[3]=value[8];
              VbatteryR = ByteBuffer.wrap(fvalue).order(ByteOrder.LITTLE_ENDIAN).getFloat();
              float Vbatterymedia = (VbatteryL + VbatteryR)/2;
              textViewBattery.setText(String.format("%.1f",Vbatterymedia) + " V");
              float calcPerc = (float) (((Vbatterymedia - 19.64) * 0.0556) * 100);
              if(Vbatterymedia>=23.25){ //60%
                progressBarBattery.setProgressTintList(ColorStateList.valueOf(Color.BLUE));
              }else if(Vbatterymedia<23.25){
                progressBarBattery.setProgressTintList(ColorStateList.valueOf(Color.YELLOW));
              }else if(Vbatterymedia<22.6){ //30%
                progressBarBattery.setProgressTintList(ColorStateList.valueOf(Color.RED));
              }
              progressBarBattery.setProgress((int)calcPerc);

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
        Log.i("BLE", "Connessione Stato: " + connectionStateLiveData.toString());


    }
    private void State_ConnectionGATT(Boolean connectionStateLiveData){
      Log.i("BLE","State_ConnectionGATT " + connectionStateLiveData.toString() );
      if(connectionStateLiveData==false){
        //finish();
        //android.os.Process.killProcess(android.os.Process.myPid());
        //System.exit(0);
      }
    }


  @Override
  protected void onStop() {
    super.onStop();
    viewModel.disconnect();
  }



  @Override
  public boolean onTouchEvent(MotionEvent event) {

    Log.i("MAIN2", "---------------------------");



   // this.finish();
    android.os.Process.killProcess(android.os.Process.myPid());

    return super.onTouchEvent(event);

  }


}