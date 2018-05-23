package com.example.kunlin.a4;

import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;
import java.util.Random;

/**
 * Created by kunlin on 2017-12-01.
 */

public class Model extends Observable {
    // Create static instance of this mModel
    private static final Model ourInstance = new Model(4,1);
    static Model getInstance()
    {
        return ourInstance;
    }

    // possible game states:
    // PAUSE - nothing happening
    // COMPUTER - computer is play a sequence of buttons
    // HUMAN - human is guessing the sequence of buttons
    // LOSE and WIN - game is over in one of thise states
    public enum State { START, COMPUTER, HUMAN, LOSE, WIN };

    // the game state and score
    protected State state;
    protected int score;
    // length of sequence
    protected int length;
    // number of possible buttons
    protected int buttons;
    protected int difficulty;
    protected int messageInd;
    protected boolean isComputerEnd;
    // the sequence of buttons and current button
    protected ArrayList<Integer> sequence;
    protected int index;
    protected boolean debug;
    protected void init(int _buttons, int _difficulty, boolean _debug){
        // true will output additional information
        // (you will want to turn this off before)
        debug = _debug;
        length = 1;
        buttons = _buttons;
        difficulty = _difficulty;
        messageInd = 0;
        isComputerEnd = false;
        state = State.START;
        score = 0;
        sequence = new ArrayList<Integer>();
        if (debug) {
            System.out.println("[DEBUG] starting " + buttons + " button game");
        }
    }

    public Model(int _buttons, int _difficulty) { init(_buttons, _difficulty, false); }
    public Model(int _buttons, int _difficulty, boolean _debug) { init(_buttons, _difficulty, _debug); }
    public int getNumButtons() { return buttons; }
    public int getDifficulty(){ return difficulty; }
    public boolean getIsComputerEnd(){ return isComputerEnd; }
    public int getScore() { return score; }
    public State getState() { return state; }

    public void setNumButtons(int num){
        buttons = num;
        setChangedAndNotify();
    }

    public void setDifficulty(int num){
        difficulty = num;
        setChangedAndNotify();
    }

    public void setIsComputerEnd(boolean _isEnd){
        isComputerEnd = _isEnd;
        setChangedAndNotify();
    }

    public int getMessageInd(){
        switch (getState()) {
            case START:
                messageInd = 0;
                break;
            case COMPUTER:
                messageInd = 1;
                break;
            case HUMAN:
                messageInd = 2;
                break;
            case LOSE:
                messageInd = 3;
                break;
            case WIN:
                messageInd = 4;
                break;
            default:
                messageInd = 0;
                break;
        }
        return messageInd;
    }


    // helper function to do both
    void setChangedAndNotify() {
        setChanged();
        notifyObservers();
    }
    @Override
    public synchronized void deleteObserver(Observer o) {
        super.deleteObserver(o);
    }

    @Override
    public synchronized void addObserver(Observer o)
    {
        super.addObserver(o);
    }


    public String getStateAsString() {

        switch (getState()) {
            case START:
                return "START";
            case COMPUTER:
                return "COMPUTER";
            case HUMAN:
                return "HUMAN";

            case LOSE:
                return "LOSE";
            case WIN:
                return "WIN";
            default:
                return "Unkown State";
        }
    }

    public void newRound() {

        if (debug) {
            System.out.println("[DEBUG] newRound, Simon::state " + getStateAsString());
        }

        // reset if they lost last time
        if (state == State.LOSE) {
            if (debug) { System.out.println("[DEBUG] reset length and score after loss");}
            length = 1;
            score = 0;
        }

        sequence.clear();

        if (debug) { System.out.println("[DEBUG] new sequence: ");}

        for (int i = 0; i < length; i++) {
            Random r = new Random();
            int b = r.nextInt(buttons - 0) + 0;
//            int b = rand() % buttons;
//            sequence.push_back(b);
            sequence.add(b);
            if (debug) { System.out.print(b + " ");}
        }
        if (debug) { System.out.println(""); }

        index = 0;
        state = State.COMPUTER;
    }

    // call this to get next button to show when computer is playing
    public int nextButton() {

        if (state != State.COMPUTER) {
            System.out.println("[WARNING] nextButton called in " + getStateAsString());
            return -1;
        }

        // this is the next button to show in the sequence
        int button = sequence.get(index);

        if (debug) {
            System.out.println("[DEBUG] nextButton:  index " + index + " button ");
        }

        // advance to next button
        index++;

        // if all the buttons were shown, give
        // the human a chance to guess the sequence
        if (index >= sequence.size()) {
            index = 0;
            state = State.HUMAN;
        }
        return button;
    }

    public void verifyButton(int button) {

        if (state != State.HUMAN) {
            System.out.println("[WARNING] verifyButton called in " + getStateAsString());
//            return false;
        }

        // did they press the right button?
        boolean correct = (button == sequence.get(index));

        if (debug) {
            System.out.println("[DEBUG] verifyButton: index " + index + ", pushed " + button + ", sequence " + sequence.get(index));
        }

        // advance to next button
        index++;

        // pushed the wrong buttons
        if (!correct) {
            state = State.LOSE;
            if (debug) {
                System.out.println(", wrong. ");
                System.out.println("[DEBUG] state is now " + getStateAsString());
            }

            // they got it right
        } else {
            if (debug) { System.out.println(", correct."); }

            // if last button, then the win the round
            if (index == sequence.size()) {
                state = State.WIN;
                // update the score and increase the difficulty
                score++;
                length++;

                if (debug) {
                    System.out.println("[DEBUG] state is now " + getStateAsString());
                    System.out.println("[DEBUG] new score " + score + ", length increased to " + length);
                }
            }
        }
//        return correct;
        setChangedAndNotify();
    }

}
