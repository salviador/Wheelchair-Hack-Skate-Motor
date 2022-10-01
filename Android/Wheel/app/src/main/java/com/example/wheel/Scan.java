package com.example.wheel;
import java.util.ArrayList;
import java.util.List;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.Manifest;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
//import android.R;

import no.nordicsemi.android.support.v18.scanner.BluetoothLeScannerCompat;
import no.nordicsemi.android.support.v18.scanner.ScanCallback;
import no.nordicsemi.android.support.v18.scanner.ScanFilter;
import no.nordicsemi.android.support.v18.scanner.ScanResult;
import no.nordicsemi.android.support.v18.scanner.ScanSettings;

import com.example.wheel.ScanBLEAdapter;



public class Scan extends AppCompatActivity {
    private boolean is_Scanner_BLE  = false;

    private static final int PERMISSION_REQUEST_COARSE_LOCATION = 456;

    BluetoothLeScannerCompat scanner = BluetoothLeScannerCompat.getScanner();
    myScanBleCallback scanBleCallback = new myScanBleCallback();

    private List<Device> list_devices = new ArrayList<>();

    private RecyclerView mRecyclerView;
    private ScanBLEAdapter scanBLEAdapter = new ScanBLEAdapter(list_devices);

    private Intent main2Activity;

//    private String FILTER_NAME = "WHEEL";
    private String FILTER_NAME = "WHEELS Mic";


    List<ScanFilter> filters;
    ScanSettings settings;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scan);

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


        main2Activity = new Intent(this, Main2_activity.class);

        mRecyclerView = (RecyclerView) findViewById(R.id.recycleview_device);
        mRecyclerView.setLayoutManager(new LinearLayoutManager(this));
        mRecyclerView.setAdapter(scanBLEAdapter);

        scanBLEAdapter.SetOnItemClickListener(new ScanBLEAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, int position, Device device) {
                Log.d("SCAN", "onItemClick" + position );
                if(is_Scanner_BLE){
                    is_Scanner_BLE = false;
                    scanner.stopScan(scanBleCallback);
                }
                main2Activity.putExtra(Main2_activity.EXTRA_DEVICE, device);
                startActivity(main2Activity);
            }
        });

        list_devices.clear();



        List<ScanFilter> filters = new ArrayList<>();
        //filters.add(new ScanFilter.Builder().setServiceUuid(mUuid).build());
    }

    @Override
    protected void onStart() {
        super.onStart();
        scanBLEAdapter.clearDevice();
        list_devices.clear();
        scanner.stopScan(scanBleCallback);


        if(!is_Scanner_BLE){
            is_Scanner_BLE = true;
            scanner.startScan(filters,settings, scanBleCallback);
           // scanner.startScan(scanBleCallback);
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        if(is_Scanner_BLE){
            is_Scanner_BLE = false;
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
            if (Deviceexist == false) {
                devices.add(new_device);
                int idn = devices.indexOf(new_device);
                scanBLEAdapter.notifyItemInserted(idn);
                scanBLEAdapter.notifyDataSetChanged();

            }
        }
    }


    //Scan CallBack

    private class myScanBleCallback extends ScanCallback{
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