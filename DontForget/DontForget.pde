import processing.serial.*;

TweetStewgate ts;
String token = "Token";
String myname = "TwitterID";
Serial myPort;
int inByte;
String tweetStr;
PFont font;

void setup(){
  size(640, 150);
  fill(255);
  frameRate(60);
  noStroke();
  myPort = new Serial(this, Serial.list()[2], 9600);
  ts = new TweetStewgate(token);
  printArray(Serial.list());
}

void serialEvent(Serial p){
  inByte = myPort.read();
  int rand = int(random(0,100001));
  println(inByte);
  if(inByte == 1){
    tweetStr = myname + " Please turn on the light, it makes my eyes worse." + "(" + rand  + ")";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }else if(inByte == 2){
    tweetStr = myname + " You're out of the room with the lights on!" + "(" + rand  + ")";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }else if(inByte == 3){
    tweetStr = myname + " Make sure you put everything on the table!" + "(" + rand  + ")";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }else if(inByte == 4){
    tweetStr = myname + " Make sure you put everything on the table NOW!!!!" + "(" + rand  + ")";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }else if(inByte == 5){
    tweetStr = myname + " You left it behind and You were out!" + "(" + rand  + ")";
    text(tweetStr, 10, 10, width-20, height-20);
    ts.tweetMsg(tweetStr);
    println(tweetStr);
  }
}

void draw(){
  background(0);
  
}
