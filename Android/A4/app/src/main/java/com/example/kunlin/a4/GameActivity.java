package com.example.kunlin.a4;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Button;
import android.widget.GridLayout;
import android.widget.GridLayout.Spec;
import android.view.ViewGroup.LayoutParams;
import android.os.Handler;
import android.util.Log;

import android.view.animation.Animation;
import android.view.animation.AnimationUtils;

import java.util.Observable;
import java.util.Observer;

public class GameActivity extends AppCompatActivity implements Observer{

    private Model model;
    TextView scoreText;
    TextView messageText;
    String[] messages = {   "Start: Press START key to play",
                            "Computer: Watch what I do ...",
                            "Human: You turn...",
                            "Lose: You lose. Press START to play again.",
                            "Win: You won! Press START to continue."};

    GridLayout buttonRowGrid;
    Button[] buttons;
    Animation myAnim;
    Handler handler;
    Button startButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(String.valueOf(R.string.DEBUG_MVC_ID), "GameActivity: OnCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game);

        model = new Model(Model.getInstance().getNumButtons(), Model.getInstance().getDifficulty());

        model.addObserver(this);

        scoreText = findViewById(R.id.textView3);
        messageText = findViewById(R.id.textView4);

        startButton = findViewById(R.id.button2);


        setGridButtonLayout();

        model.setChangedAndNotify();
    }

    protected void setGridButtonLayout(){
        buttonRowGrid = (GridLayout) findViewById(R.id.buttonRow);
        buttonRowGrid.removeAllViews();

        int total = model.getNumButtons();
        int column = 3;
        if(total == 4) column = 2;
        int row = total / column;

        buttons = new Button[total];

        buttonRowGrid.setColumnCount(column);
        buttonRowGrid.setRowCount(row + 1);

        for(int i = 0,c = 0, r = 0; i < model.getNumButtons(); i++, c++){
            if (c == column) {
                c = 0;
                r++;
            }
            buttons[i] = new Button(this);
            buttons[i].setId(i);
            buttons[i].setLayoutParams(new LayoutParams(250, 250));
            final int clickedIndex = i;
            buttons[i].setEnabled(false);
            buttons[i].setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if(model.getState() == Model.State.HUMAN) {
                        model.verifyButton(clickedIndex);
                    }
                }
            });
            Spec rowSpan = GridLayout.spec(GridLayout.UNDEFINED, 1);
            Spec colspan = GridLayout.spec(GridLayout.UNDEFINED, 1);
            if (r == 0 && c == 0) {
                Log.e("", "spec");
                colspan = GridLayout.spec(GridLayout.UNDEFINED, 2);
                rowSpan = GridLayout.spec(GridLayout.UNDEFINED, 2);
            }
            GridLayout.LayoutParams gridParam = new GridLayout.LayoutParams(
                    rowSpan, colspan);
            buttonRowGrid.addView(buttons[i]);
        }
    }

    @Override
    protected void onDestroy(){
        super.onDestroy();

        // Remove observer when activity is destroyed.
        model.deleteObserver(this);
    }

    @Override
    public void update(Observable o, Object arg){
        scoreText.setText("Score: " + model.getScore());
        // add delay for Computer -> Human, after animation
        messageText.setText(messages[model.getMessageInd()]);
        if((model.getState() == Model.State.HUMAN) && (model.getIsComputerEnd())){
            setbuttonsEnabled(true);
            messageText.setText(messages[model.getMessageInd()]);
        }else{
            setbuttonsEnabled(false);
        }
    }

    public void setbuttonsEnabled(boolean enable){
        for(int i = 0; i < model.getNumButtons(); i++){
            buttons[i].setEnabled(enable);
        }
    }

    public void backMainPage(View view){
//        model = defaultModel;
        Intent backMainActivity = new Intent(this, MainActivity.class);
        startActivity(backMainActivity);
    }

    // called when clicked Start button in Gamepage
    public void startComputerPlaying(View view){
        model.newRound();
        if(model.getState() == Model.State.COMPUTER){
            model.setIsComputerEnd(false);
            int nextInd = model.nextButton();
            clickButtonAnimation(nextInd);
        }
    }

    // make some animation effects on specified index of button
    public void clickButtonAnimation(int ind){
        myAnim = AnimationUtils.loadAnimation(this, R.anim.milkshake);
        if(model.getDifficulty() == 0){ // Easy Mode
            myAnim.setDuration(300);
        }else if(model.getDifficulty() == 1){ // Normal
            myAnim.setDuration(200);
        }else if(model.getDifficulty() == 2){ // Hard
            myAnim.setDuration(100);
        }
        myAnim.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                if(model.getState() == Model.State.COMPUTER){
                    int nextInd = model.nextButton();
                    clickButtonAnimation(nextInd);
                }
                else if(model.getState() == Model.State.HUMAN){
                    model.setIsComputerEnd(true);
                }
            }

            @Override
            public void onAnimationRepeat(Animation animation) {
            }
        });
        final int index = ind;
        handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                buttons[index].startAnimation(myAnim);
            }
        }, 200);
    }
}
