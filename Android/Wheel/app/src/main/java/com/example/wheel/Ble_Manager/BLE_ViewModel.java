package com.example.wheel.Ble_Manager;

import android.app.Application;
import android.bluetooth.BluetoothDevice;
import android.content.Context;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.ViewModel;

import com.example.wheel.Device;

import no.nordicsemi.android.ble.livedata.state.ConnectionState;

public class BLE_ViewModel extends AndroidViewModel {
    private BluetoothDevice device;
    private Ble_Manager ble_manager;

    public BLE_ViewModel(@NonNull final Application application) {
        super(application);
        ble_manager = new Ble_Manager(getApplication());
    }

    public LiveData<ConnectionState> getConnectionState() {return ble_manager.getState();}
    /*
    public LiveData<Boolean> getButtonState() {
        return ble_manager.getButtonState();
    }
    public LiveData<Boolean> getLedState() {
        return ble_manager.getLedState();
    }
    public LiveData<Float> getBatteryState() {
        return ble_manager.getBatteryState();
    }
    public LiveData<Boolean> getbuttonSleepState() {
        return ble_manager.getbuttonSleepState();
    }
    */
    public LiveData<byte[]> getTelemetriaState() {
        return ble_manager.getTelemetriaState();
    }


    public final LiveData<Boolean> getConnectionStateGATT() {return ble_manager.getConnectionStateGATT();}


    public void connect(@NonNull final Device target) {
        // Prevent from calling again when called again (screen orientation changed).
        if (device == null) {
            device = target.getDevice();
            reconnect();
        }
    }
    public void reconnect() {
        if (device != null) {
            ble_manager.connect(device)
                    .retry(3, 100)
                    .useAutoConnect(false)
                    .enqueue();
        }
    }

    private void disconnect() {
        device = null;
        ble_manager.disconnect().enqueue();
    }
/*
    public void setLedState(final boolean on) {
        ble_manager.turnLed(on);
    }
    public void setButton_SLEEP_State(final boolean on) {
        ble_manager.Button_Sleep_SEND(on);
    }
*/
    @Override
    protected void onCleared() {
        super.onCleared();
        if (ble_manager.isConnected()) {
            disconnect();
        }
    }
}
