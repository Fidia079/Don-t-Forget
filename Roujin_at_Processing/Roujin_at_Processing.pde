import processing.serial.*;

TweetStewgate ts;
String token = "input Token";
String myname = "tweetID";
Serial myPort;
int inByte;
String tweetStr;
PFont font;

void setup(){
  size(640, 150);
  fill(255);
  frameRate(60);
  noStroke();
  myPort = new Serial(this, Serial.list()[0], 9600);
  ts = new TweetStewgate(token);
  printArray(Serial.list());
}

void serialEvent(Serial p){
  inByte = myPort.read();
  println(inByte);
  if(inByte == 1){
    tweetStr = myname + " Please turn on the light, it makes my eyes worse.";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }else if(inByte == 2){
    tweetStr = myname + " You're out of the room with the lights on!";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }else if(inByte == 3){
    tweetStr = myname + " Make sure you put everything on the table!";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }else if(inByte == 4){
    tweetStr = myname + " Make sure you put everything on the table NOW!!!!";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }else if(inByte == 5){
    tweetStr = myname + " You left it behind and You were out!";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }
}

void draw(){
  background(0);
  
}
