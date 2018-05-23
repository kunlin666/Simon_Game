package com.example.kunlin.a4;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.content.Intent;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


    }

    public void startSetting(View view){
        Intent startSettingActivity = new Intent(this, SettingActivity.class);
        startActivity(startSettingActivity);
    }

    public void startGame(View view){
        Intent startGameActivity = new Intent(this, GameActivity.class);
        startActivity(startGameActivity);
    }

}
