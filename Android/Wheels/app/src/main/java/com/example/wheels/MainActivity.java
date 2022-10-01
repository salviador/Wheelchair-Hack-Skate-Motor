package com.example.wheels;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.drawable.Animatable;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.example.wheels.databinding.ActivityMainBinding;

import java.util.ArrayList;
import java.util.List;

import no.nordicsemi.android.support.v18.scanner.BluetoothLeScannerCompat;
import no.nordicsemi.android.support.v18.scanner.ScanCallback;
import no.nordicsemi.android.support.v18.scanner.ScanFilter;
import no.nordicsemi.android.support.v18.scanner.ScanResult;
import no.nordicsemi.android.support.v18.scanner.ScanSettings;

public class MainActivity extends Activity {
    private boolean is_Scanner_BLE  = false;

    public ImageView searchanimation;
    private static final int PERMISSION_REQUEST_COARSE_LOCATION = 456;
    BluetoothLeScannerCompat scanner = BluetoothLeScannerCompat.getScanner();
    MainActivity.myScanBleCallback scanBleCallback = new MainActivity.myScanBleCallback();

    private List<Device> list_devices = new ArrayList<>();
    private String FILTER_NAME = "WHEELS Mic";
//    private String FILTER_NAME = "WHEEL";
    List<ScanFilter> filters;
    ScanSettings settings;

    private Intent main2Activity;

    private ActivityMainBinding binding;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        searchanimation = (ImageView) findViewById(R.id.searchanimation);
        if (!(ContextCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_FINE_LOCATION) ==
                PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_BACKGROUND_LOCATION) == PackageManager.PERMISSION_GRANTED

        )) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, 1);
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, PERMISSION_REQUEST_COARSE_LOCATION);
        }
        settings = new ScanSettings.Builder()
                .setPhy(ScanSettings.PHY_LE_ALL_SUPPORTED)
                .setLegacy(false)
                .setScanMode(ScanSettings.SCAN_MODE_LOW_POWER)
                .setReportDelay(0)
                .setUseHardwareBatchingIfSupported(false)
                .build();

        filters = new ArrayList<>();
        filters.add(new ScanFilter.Builder().setDeviceName(FILTER_NAME).build());



        list_devices.clear();


        main2Activity = new Intent(this, MainActivity2.class);
/*
// SOlo x debug
        main2Activity.putExtra(MainActivity2.EXTRA_DEVICE, "");
        startActivity(main2Activity);*/
    }

    @Override
    protected void onResume() {
        super.onResume();
    }
    public boolean isBleEnabled() {
        final BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        return adapter != null && adapter.isEnabled();
    }
    private void go_scan_activity(){
        if(isBleEnabled()) {
            if(!is_Scanner_BLE){
                is_Scanner_BLE = true;

                ((Animatable) (searchanimation.getDrawable())).start();

                scanner.startScan(filters,settings, scanBleCallback);
                // scanner.startScan(scanBleCallback);
            }
        }else {
            ((Animatable) (searchanimation.getDrawable())).stop();

            Toast.makeText(this,"Abilitare il Bluetooth!", Toast.LENGTH_LONG).show();
            finish();
        }
    }



    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                //Permission Granted
                go_scan_activity();
//Perform operations here only which requires permission
            } else {
                //Toast.makeText(this, "Abilitare i permessi!", Toast.LENGTH_LONG).show();
                finish();
            }
        }
    }


    @Override
    protected void onStart() {
        super.onStart();
        // scanBLEAdapter.clearDevice();
        list_devices.clear();
    //    scanner.stopScan(scanBleCallback);

        go_scan_activity();

    }

    @Override
    protected void onStop() {
        super.onStop();
        if(is_Scanner_BLE){
            is_Scanner_BLE = false;
            ((Animatable) (searchanimation.getDrawable())).stop();
            scanner.stopScan(scanBleCallback);
        }
    }


    @Override
    protected void onPause() {
        super.onPause();
    }


    public void add_Device(List<Device> devices, Device new_device, String filter_name){
        boolean Deviceexist = false;

        if(new_device != null) {
            if(new_device.getName() != null) {
                Log.i("SCAN", new_device.getName());
            }
        }

        if((new_device.getName()!=null)&&(new_device.getName().startsWith(filter_name))) {

            for (Device dev : devices) {
                if (devices.contains(new_device)) {
                    Deviceexist = true;
                }
            }

            if(new_device.getName().equals(FILTER_NAME)){
                if(is_Scanner_BLE){
                    is_Scanner_BLE = false;
                    scanner.stopScan(scanBleCallback);
                    ((Animatable) (searchanimation.getDrawable())).stop();

                }
                main2Activity.putExtra(MainActivity2.EXTRA_DEVICE, new_device);
                startActivity(main2Activity);
               // Toast.makeText(this,new_device.getAddress(), Toast.LENGTH_LONG).show();
            }

            if (Deviceexist == false) {
                devices.add(new_device);
                int idn = devices.indexOf(new_device);
                // scanBLEAdapter.notifyItemInserted(idn);
                // scanBLEAdapter.notifyDataSetChanged();



            }
        }
    }





    //Scan CallBack

    private class myScanBleCallback extends ScanCallback {
        public myScanBleCallback() {
            super();
        }

        @Override
        public void onScanResult(int callbackType, @NonNull ScanResult result) {
            super.onScanResult(callbackType, result);
            Device _device;
            _device = new Device(result);
            _device.update(result);
            add_Device(list_devices,_device,FILTER_NAME);
        }

        @Override
        public void onBatchScanResults(@NonNull List<ScanResult> results) {
            super.onBatchScanResults(results);
            for (final ScanResult result : results){
                Device _device;
                _device = new Device(result);
                _device.update(result);
                add_Device(list_devices,_device,FILTER_NAME);
            }
        }

        @Override
        public void onScanFailed(int errorCode) {
            super.onScanFailed(errorCode);
        }
    }








}