package com.example.kunlin.a4;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.SeekBar;
import android.content.Intent;
import android.util.Log;

import java.util.Observable;
import java.util.Observer;

public class SettingActivity extends AppCompatActivity implements Observer {

    private Model model;
    TextView numButtonsLabel;
    TextView levelDifficultyLabel;
    SeekBar seekBarButton;
    SeekBar seekBarDifficulty;

    String[] difficulty = {"Easy", "Normal", "Hard"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
//        Log.d(String.valueOf(R.string.DEBUG_MVC_ID), "SettingActivity: OnCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);

        model = Model.getInstance();
        model.addObserver(this);

        // set a change listener on the SeekBar
        seekBarButton = findViewById(R.id.seekBar);
        seekBarButton.setOnSeekBarChangeListener(seekBarButtonChangeListener);

        seekBarDifficulty = findViewById(R.id.seekBar2);
        seekBarDifficulty.setOnSeekBarChangeListener(seekBarDiffChangeListener);

        numButtonsLabel = findViewById(R.id.textView);
        levelDifficultyLabel = findViewById(R.id.textView2);

        model.setChangedAndNotify();

    }

    SeekBar.OnSeekBarChangeListener seekBarButtonChangeListener = new SeekBar.OnSeekBarChangeListener() {

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            // updated continuously as the user slides the thumb
            model.setNumButtons(progress+1);
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            // called when the user first touches the SeekBar
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            // called after the user finishes moving the SeekBar
        }
    };

    SeekBar.OnSeekBarChangeListener seekBarDiffChangeListener = new SeekBar.OnSeekBarChangeListener() {

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            // updated continuously as the user slides the thumb
            model.setDifficulty(progress);
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            // called when the user first touches the SeekBar
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            // called after the user finishes moving the SeekBar
        }
    };

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        // Remove observer when activity is destroyed.
        model.deleteObserver(this);
    }

    @Override
    public void update(Observable o, Object arg)
    {
//        Log.d(String.valueOf(R.string.DEBUG_MVC_ID), "View: Update SettingView");

        numButtonsLabel.setText("Buttons: " + model.buttons);
        levelDifficultyLabel.setText("Difficulty: " + difficulty[model.difficulty]);
        seekBarButton.setProgress(model.buttons-1);
        seekBarDifficulty.setProgress(model.difficulty);
    }

    public void backMainPage(View view){
        Intent backMainActivity = new Intent(this, MainActivity.class);
        startActivity(backMainActivity);
    }

}
