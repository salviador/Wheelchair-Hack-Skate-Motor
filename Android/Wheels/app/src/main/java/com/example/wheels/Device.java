package com.example.wheels;


import android.bluetooth.BluetoothDevice;
import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import no.nordicsemi.android.support.v18.scanner.ScanResult;

public class Device implements Parcelable {
    private final BluetoothDevice device;
    private ScanResult lastScanResult;
    private String name;
    private int rssi;
    private int previousRssi;
    private int highestRssi = -128;

    public Device(@NonNull final ScanResult scanResult) {
        device = scanResult.getDevice();
        update(scanResult);
    }

    public void update(@NonNull final ScanResult scanResult) {
        lastScanResult = scanResult;
        name = scanResult.getScanRecord() != null ?
                scanResult.getScanRecord().getDeviceName() : null;
        previousRssi = rssi;
        rssi = scanResult.getRssi();
        if (highestRssi < rssi)
            highestRssi = rssi;
    }


    @Override
    public boolean equals(@Nullable Object o) {
        if (o instanceof Device) {
            final Device that = (Device) o;
            return device.getAddress().equals(that.device.getAddress());
        }
        return super.equals(o);
    }

    @Override
    public void writeToParcel(Parcel parcel, int flags) {
        parcel.writeParcelable(device, flags);
        parcel.writeParcelable(lastScanResult, flags);
        parcel.writeString(name);
        parcel.writeInt(rssi);
        parcel.writeInt(previousRssi);
        parcel.writeInt(highestRssi);
    }
    private Device(final Parcel in) {
        device = in.readParcelable(BluetoothDevice.class.getClassLoader());
        lastScanResult = in.readParcelable(ScanResult.class.getClassLoader());
        name = in.readString();
        rssi = in.readInt();
        previousRssi = in.readInt();
        highestRssi = in.readInt();
    }
    @Override
    public int describeContents() {
        return 0;
    }
    public static final Creator<Device> CREATOR = new Creator<Device>() {
        @Override
        public Device createFromParcel(final Parcel source) {
            return new Device(source);
        }

        @Override
        public Device[] newArray(final int size) {
            return new Device[size];
        }
    };
















    @NonNull
    public BluetoothDevice getDevice() {
        return device;
    }
    @NonNull
    public String getAddress() {
        return device.getAddress();
    }
    @Nullable
    public String getName() {
        return name;
    }
    public int getRssi() {
        return rssi;
    }
    @NonNull
    public ScanResult getScanResult() {
        return lastScanResult;
    }
    public int getHighestRssi() {
        return highestRssi;
    }
    boolean hasRssiLevelChanged() {
        final int newLevel =
                rssi <= 10 ?
                        0 :
                        rssi <= 28 ?
                                1 :
                                rssi <= 45 ?
                                        2 :
                                        rssi <= 65 ?
                                                3 :
                                                4;
        final int oldLevel =
                previousRssi <= 10 ?
                        0 :
                        previousRssi <= 28 ?
                                1 :
                                previousRssi <= 45 ?
                                        2 :
                                        previousRssi <= 65 ?
                                                3 :
                                                4;
        return newLevel != oldLevel;
    }
    public boolean matches(@NonNull final ScanResult scanResult) {
        return device.getAddress().equals(scanResult.getDevice().getAddress());
    }
    @Override
    public int hashCode() {
        return device.hashCode();
    }

}