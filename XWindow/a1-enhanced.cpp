/****************************************   a1-enhanced.cpp sepecification    ************************************
    
    1. create new set of colors(red and yello)
    2. red in highligh border animation
    3. yello in background
    4. to compile: make NAME="a1-enhanced"
    5. to run: ./a1-enhanced
 
 ***************************************************************************************************************/
#include <stdio.h>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include "simon.h"
using namespace std;

#define PI 3.14159265

struct XInfo{
    Display* display;
    Window window;
    int screen;
    GC gc;
    XColor XRedPixel, XYellowPixel; // used for allocation of the given color
};

XInfo xinfo;

// frames per second to run animation loop
int FPS = 60;

// get microseconds
unsigned long now() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
    cerr << str << endl;
    exit(0);
}

// helper function to set X foreground colour
enum Colour {BLACK, WHITE, RED, YELLOW};
void setForeground(Colour c) {
    if (c == BLACK) {
        XSetForeground(xinfo.display, xinfo.gc, BlackPixel(xinfo.display, xinfo.screen));
    }
    else if(c == RED){
        XSetForeground(xinfo.display, xinfo.gc, xinfo.XRedPixel.pixel);
    }
    else if(c == YELLOW){
        XSetForeground(xinfo.display, xinfo.gc, xinfo.XYellowPixel.pixel);
    }
    else {
        XSetForeground(xinfo.display, xinfo.gc, WhitePixel(xinfo.display, xinfo.screen));
    }
}

void setBackground(Colour c) {
    if (c == BLACK) {
        XSetBackground(xinfo.display, xinfo.gc, BlackPixel(xinfo.display, xinfo.screen));
    } else if(c == BLACK){
        XSetBackground(xinfo.display, xinfo.gc, xinfo.XYellowPixel.pixel);
    }
    else {
        XSetBackground(xinfo.display, xinfo.gc, WhitePixel(xinfo.display, xinfo.screen));
    }
}

// helper function to convert int to string
string numToString(int num){
    ostringstream ss;
    ss << num;
    return ss.str();
}

// helper function to convert windowX to the first circle X
int WindowXtoFirstX(int windowWidth, int diameter, int numOfCircle){
    return ((windowWidth - diameter * numOfCircle ) / (numOfCircle + 1)) + (diameter / 2);
}

/*
 * Create a window
 */
void initX(int argc, char* argv[], XInfo& xinfo) {
    
    /*
     * Display opening uses the DISPLAY  environment variable.
     * It can go wrong if DISPLAY isn't set, or you don't have permission.
     */
    xinfo.display = XOpenDisplay( "" );
    if ( !xinfo.display ) {
        error( "Can't open display." );
    }
    
    /*
     * Find out some things about the display you're using.
     */
    // DefaultScreen is as macro to get default screen index
    int screen = DefaultScreen( xinfo.display );
    
    Status rc;			// return status of various Xlib functions.
    Colormap screen_colormap;     // color map to use for allocating colors.
    // get access to the screen's color map.
    screen_colormap = DefaultColormap(xinfo.display, DefaultScreen(xinfo.display));
    
    // allocate the set of colors we will want to use for the drawing.
    rc = XAllocNamedColor(xinfo.display, screen_colormap, "red", &xinfo.XRedPixel, &xinfo.XRedPixel);
    if (rc == 0) {
        fprintf(stderr, "XAllocNamedColor - failed to allocated 'red' color.\n");
        exit(1);
    }
    rc = XAllocNamedColor(xinfo.display, screen_colormap, "yellow", &xinfo.XYellowPixel, &xinfo.XYellowPixel);
    if (rc == 0) {
        fprintf(stderr, "XAllocNamedColor - failed to allocated 'red' color.\n");
        exit(1);
    }

    
    unsigned long white, black, yellow;
    white = XWhitePixel( xinfo.display, screen );
    black = XBlackPixel( xinfo.display, screen );
    yellow = xinfo.XYellowPixel.pixel;
    
    xinfo.window = XCreateSimpleWindow(
                                       xinfo.display,       // display where window appears
                                       DefaultRootWindow( xinfo.display ), // window's parent in window tree
                                       10, 10,                  // upper left corner location
                                       800, 400,                  // size of the window
                                       5,               // width of window's border
                                       black,           // window border colour
                                       yellow);             // window background colour
    
    // extra window properties like a window title
    XSetStandardProperties(
                           xinfo.display,    // display containing the window
                           xinfo.window,   // window whose properties are set
                           "simon",  // window's title
                           "CL",       // icon's title
                           None,       // pixmap for the icon
                           argv, argc,     // applications command line args
                           None );         // size hints for the window
    
    XSelectInput(xinfo.display, xinfo.window,
                 ButtonPressMask | KeyPressMask | StructureNotifyMask | PointerMotionMask); // select events
    
    /*
     * Put the window on the screen.
     */
    XMapRaised( xinfo.display, xinfo.window );
    
    XFlush(xinfo.display);
    
    // give server time to setup
    sleep(1);
}

/*
 * An abstract class representing displayable things. */
class Displayable {
public:
    virtual void paint() = 0; // PURE VIRTUAL
    virtual void mouseInside(int mx, int my){}; // VIRTUAL
    virtual void mouseClick(int mx, int my){};
    virtual void setX(int x){};
    virtual void setY(int y){};
    virtual bool getisClick(){return -1;};
    virtual void setDefaultCircleAnimation(bool _isWaitAnimate){};
    virtual void setText(string s){};
    virtual void autoPlay(vector<Displayable *> dList, int numOfCircle){};
};

class Text : public Displayable {
public:
    virtual void paint(){
        XDrawImageString(xinfo.display, xinfo.window, xinfo.gc,
                         this->x, this->y, this->s.c_str(),
                         this->s.length() );
    }
    // constructor
    Text(int x, int y, string s): x(x), y(y), s(s){}
    
    Text(int x, int y, int num): x(x), y(y){
        ostringstream ss;
        ss << num;
        this->s = ss.str();
    }
    
    void setText(string _s){
        s = _s;
    }
    
    
private:
    int x;
    int y;
    string s;
};


void repaint( vector<Displayable*> dList);


class Button : public Displayable {
public:
    
    virtual void paint(){
        if(isWaitAnimate){
            DefaultCircleAnimation();
            usleep(8000);
        }
        else if(!isInside and !isClick and !isWaitAnimate){
            DefaultCircle();
        }
        if(isInside and !isClick){
            HighlightBorder();
        }
        if(isClick){
            HighlightAnimation();
            usleep(50000);
        }
    }
    
    // Constructor
    Button( int _x, int _y, int _runtimeDeg, int _offsetDeg, int _diameter, int _runtimeDiameter, int _runtimeDiameterSpeed,int _index){
        x = _x;
        y = _y;
        runtimeDeg = _runtimeDeg;
        runtimeY = _y;
        offsetDeg = _offsetDeg;
        diameter = _diameter;
        runtimeDiameter = _runtimeDiameter;
        runtimeDiameterSpeed = _runtimeDiameterSpeed;
        index = _index;
        
        isInside = false;
        isClick = false;
        isWaitAnimate = true;
    }
    
    // the CONTROLLER: Check if pointer is inside of circles BUG over here!!!
    void mouseInside(int mx, int my) {
        isInside = false;
        float dist = sqrt(pow(mx - x, 2) + pow(my - runtimeY, 2));
        if (dist < diameter / 2){
            isInside = true;
        }
    }
    
    void mouseClick(int mx, int my) {
        isClick = false;
        float dist = sqrt(pow(mx - x, 2) + pow(my - y, 2));
        if (dist < diameter / 2){
            isClick = true;
        }
    }
    
    void DefaultCircle(){
        setForeground(WHITE);
        XFillArc(xinfo.display, xinfo.window, xinfo.gc,
                 x - (diameter / 2),
                 y - diameter / 2 ,
                 diameter, diameter,
                 0, 360 * 64);
        setForeground(BLACK);
        XDrawArc(xinfo.display, xinfo.window, xinfo.gc,
                 x - (diameter / 2),
                 y - diameter / 2,
                 diameter, diameter,
                 0, 360 * 64);
        XDrawImageString(xinfo.display, xinfo.window, xinfo.gc,
                         x, y, numToString(index+1).c_str(),
                         numToString(index+1).length() );
    }
    
    void DefaultCircleAnimation(){
        setForeground(WHITE);
        XFillArc(xinfo.display, xinfo.window, xinfo.gc,
                 x - (diameter / 2),
                 runtimeY - diameter / 2 ,
                 diameter, diameter,
                 0, 360 * 64);
        setForeground(BLACK);
        XDrawArc(xinfo.display, xinfo.window, xinfo.gc,
                 x - (diameter / 2),
                 runtimeY - diameter / 2,
                 diameter, diameter,
                 0, 360 * 64);
        XDrawImageString(xinfo.display, xinfo.window, xinfo.gc,
                         x, runtimeY, numToString(index+1).c_str(),
                         numToString(index+1).length() );
        
        // update runtimeDeg and runtimeY
        runtimeDeg += offsetDeg;
        runtimeY = y + 8 * sin(runtimeDeg*PI/180);
        
        // change in
        if (runtimeDeg >= 360 ){
            runtimeDeg = 0;
        }
    }
    
    void HighlightBorder(){
        if(!isWaitAnimate){
            runtimeY = y;
        }
        setForeground(WHITE);
        XFillArc(xinfo.display, xinfo.window, xinfo.gc,
                 x - diameter / 2,
                 runtimeY - diameter / 2,
                 diameter, diameter,
                 0, 360 * 64);
        setForeground(RED);
        XSetLineAttributes(xinfo.display, xinfo.gc,
                           4, LineSolid, CapRound, JoinMiter);
        XDrawArc(xinfo.display, xinfo.window, xinfo.gc,
                 x - diameter / 2,
                 runtimeY - diameter / 2,
                 diameter, diameter,
                 0, 360 * 64);
        XSetLineAttributes(xinfo.display, xinfo.gc,
                           1, LineSolid, CapRound, JoinMiter);
        setForeground(BLACK);
        XDrawImageString(xinfo.display, xinfo.window, xinfo.gc,
                         x, runtimeY, numToString(index+1).c_str(),
                         numToString(index+1).length() );
    }
    
    void HighlightAnimation(){
        // one BLACK button
        setForeground(RED);
        XFillArc(xinfo.display, xinfo.window, xinfo.gc,
                 x - diameter / 2,
                 y - diameter / 2,
                 diameter, diameter,
                 0, 360 * 64);
        
        // draw ball from centre animate one WHITE border
        setForeground(WHITE);
        XDrawArc(xinfo.display, xinfo.window, xinfo.gc,
                 x - runtimeDiameter/2,
                 y - runtimeDiameter/2,
                 runtimeDiameter, runtimeDiameter,
                 0, 360*64);
        
        // update diameter
        runtimeDiameter -= runtimeDiameterSpeed;
        
        // change in diameter
        if (runtimeDiameter <= 0){
            isClick = false;
            runtimeDiameter = diameter;
        }
    }
    
    void setDefaultCircleAnimation(bool _isWaitAnimate){
        isWaitAnimate = _isWaitAnimate;
    }
    
    void autoPlay(vector<Displayable *> dList, int numOfCircle){
        unsigned long lastRepaint = 0;
        isClick = true;
        unsigned long end = now();
        while(end - lastRepaint > 1000000 / FPS){
            if (isClick){
                dList[1]->setText("Watch what I do ...");
                repaint(dList);
                lastRepaint = now();
                usleep(50000);
            }
            else {
                repaint(dList);
                break;
            }
            
        }
        usleep(250000);
    }
    
    // set functions
    virtual void setX(int _x){
        x = _x;
    }
    
    virtual void setY(int _y){
        y = _y;
    }
    
    // get function
    bool getisClick(){
        return isClick;
    }
    
private:
    // VIEW "essential geometry"
    int x;
    int y;
    int runtimeDeg;
    int runtimeY;
    int offsetDeg;
    int diameter;
    int runtimeDiameter;
    int runtimeDiameterSpeed;
    int index;
    // The MODEL
    bool isInside;
    bool isClick;
    bool isWaitAnimate;
};





/*
 * Function to repaint a display list
 */
void repaint( vector<Displayable*> dList) {
    vector<Displayable*>::const_iterator begin = dList.begin();
    vector<Displayable*>::const_iterator end = dList.end();
    
    XClearWindow(xinfo.display, xinfo.window);
    
    int screen = DefaultScreen( xinfo.display );
    setForeground(BLACK);
    setBackground(WHITE);
    
    while ( begin != end ) {
        Displayable* d = *begin;
        d->paint();
        begin++;
    }
    XFlush(xinfo.display);
}


int main(int argc, char *argv[]){
    
    initX(argc, argv, xinfo); //draw a 200*400 window
    XEvent event; // save the event here
    xinfo.gc = XCreateGC(xinfo.display, xinfo.window, 0, 0); // create a simple graphics context, gc for drawing
    XFontStruct * font; // load a larger font
    font = XLoadQueryFont (xinfo.display, "12x24");
    XSetFont (xinfo.display, xinfo.gc, font->fid);
    // get the number of buttons from args
    // (default to 4 if no args)
    int numOfCircle = 4;
    if (argc > 1) {
        numOfCircle = atoi(argv[1]);
    }
    numOfCircle = max(1, min(numOfCircle, 9));
    
    // create the Simon game object
    Simon simon = Simon(numOfCircle, true);
    
    // list of Displayables
    vector<Displayable*> dList;
    
    // time of last window paint
    unsigned long lastRepaint = 0;
    
    XWindowAttributes w;
    XGetWindowAttributes(xinfo.display, xinfo.window, &w);
    
    dList.push_back(new Text(50, 50, simon.getScore()));
    dList.push_back(new Text(50, 100, "Press SPACE to play"));
    
    unsigned int windowWidth, windowHeight, posX, runtimeDeg, offsetDeg, whiteSpace, diameter, runtimeDiameter, runtimeDiameterSpeed;
    windowWidth = 800;
    windowHeight = 400;
    diameter = 100;
    runtimeDiameter = 100;
    runtimeDiameterSpeed = 15; // not sure about values
    
    posX = WindowXtoFirstX(windowWidth, diameter, numOfCircle);
    runtimeDeg = 0;
    offsetDeg = 20;
    whiteSpace = posX - diameter / 2;
    
    // store buttons
    for (int i = 0; i<numOfCircle; i++){
        posX = posX + (diameter + whiteSpace) * i;
        dList.push_back(new Button(posX, windowHeight / 2, runtimeDeg + i * 60, offsetDeg, diameter, runtimeDiameter, runtimeDiameterSpeed, i)); // i is index, i+1 to display buttons: dList[2..2+numOfCircle-1]
    }
    
    int clickedNum; bool insideClick;
    // event loop
    while ( true ) {
        
        if (XPending(xinfo.display) > 0) {
            XNextEvent( xinfo.display, &event );
            
            switch ( event.type ) {
                    
                case ButtonPress:
                    // buttons not waiting
                    if (simon.getState() == Simon::HUMAN) {
                        for(int i = 0; i<numOfCircle; i++){
                            dList[2+i]->mouseClick(event.xbutton.x, event.xbutton.y);
                            insideClick = dList[2+i]->getisClick();
                            if(insideClick){
                                clickedNum = i;
                                break;
                            }
                        }
                        
                        // see if guess was correct
                        if (insideClick) {
                            if(!simon.verifyButton(clickedNum)){
                            }
                        }
                    }
                    break;
                    
                case MotionNotify:
                    for(int i = 0; i < numOfCircle; i++){
                        dList[2+i]->mouseInside(event.xmotion.x, event.xmotion.y);
                    }
                    break;
                    
                case ConfigureNotify:
                    /* Window has been resized; change width and height
                     * to send to place_text and place_graphics in
                     * next Expose */
                    for(int i = 0; i<numOfCircle; i++){
                        posX = WindowXtoFirstX(event.xconfigure.width, diameter, numOfCircle);
                        whiteSpace = posX - diameter / 2;
                        posX = posX + (diameter + whiteSpace) * i;
                        dList[2+i]->setX (posX);
                        dList[2+i]->setY (event.xconfigure.height / 2);
                    }
                    break;
                    
                case KeyPress: // any keypress
                    KeySym key;
                    char text[10];
                    int i = XLookupString( (XKeyEvent*)&event, text, 10, &key, 0 );
                    if ( i == 1 && text[0] == 'q' ) { // to quit
                        XCloseDisplay(xinfo.display);
                        exit(0);
                    }
                    if( i == 1 && text[0] == ' ' ) {// space is pressed
                        // buttons not waiting
                        for(int i = 0; i< numOfCircle; i++){
                            dList[2+i]->setDefaultCircleAnimation(false);
                        }
                        simon.newRound();
                        while (simon.getState() == Simon::COMPUTER) {
                            dList[2+simon.nextButton()]->autoPlay(dList,numOfCircle);
                        }
                        dList[1]->setText("You turn...");
                    }
                    break;
            }
        }
        
        unsigned long end = now();
        if (end - lastRepaint > 1000000 / FPS)  {
            
            switch (simon.getState()) {
                    // will only be in this state right after Simon object is contructed
                case Simon::START:
                    dList[1]->setText("Press SPACE to play");
                    break;
                    // they won last round
                    // score is increased by 1, sequence length is increased by 1
                case Simon::WIN:
                    if(!dList[2+clickedNum]->getisClick()){
                        dList[1]->setText("You won! Press SPACE to continue.");
                        // buttons waiting
                        for(int i = 0; i< numOfCircle; i++){
                            dList[2+i]->setDefaultCircleAnimation(true);
                        }
                        dList[0]->setText(numToString(simon.getScore()));
                    }
                    break;
                    // they lost last round
                    // score is reset to 0, sequence length is reset to 1
                case Simon::LOSE:
                    if(!dList[2+clickedNum]->getisClick()){
                        dList[1]->setText( "You lose. Press SPACE to play again.");
                        // buttons waiting
                        for(int i = 0; i< numOfCircle; i++){
                            dList[2+i]->setDefaultCircleAnimation(true);
                        }
                        dList[0]->setText(numToString(simon.getScore()));
                    }
                    break;
                default:
                    // should never be any other state at this point ...
                    break;
            }
            
            repaint(dList);
            XFlush( xinfo.display );
            lastRepaint = now(); // remember when the paint happened
        }
        
        // IMPORTANT: sleep for a bit to let other processes work
        if (XPending(xinfo.display) == 0) {
            usleep(1000000 / FPS - (end - lastRepaint));
        }
    }
    
    XCloseDisplay(xinfo.display);
}

