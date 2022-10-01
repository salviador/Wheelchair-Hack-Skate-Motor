package com.example.wheels;


public enum ModelObject {
    TELEMETRIA1(R.string.telemetria1, R.layout.fragment_1),
    TELEMETRIA2(R.string.telemetria2, R.layout.fragment_2),
    TELEMETRIA3(R.string.telemetria3, R.layout.fragment_3),
    TELEMETRIA4(R.string.telemetria4, R.layout.fragment_4);

    private int mTitleResId;
    private int mLayoutResId;

    ModelObject(int titleResId, int layoutResId) {
        mTitleResId = titleResId;
        mLayoutResId = layoutResId;
    }

    public int getTitleResId() {
        return mTitleResId;
    }

    public int getLayoutResId() {
        return mLayoutResId;
    }

}