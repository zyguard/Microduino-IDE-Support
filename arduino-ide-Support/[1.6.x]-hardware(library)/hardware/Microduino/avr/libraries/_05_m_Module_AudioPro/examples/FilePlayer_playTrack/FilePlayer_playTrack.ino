/*
   Microduino_AudioPro支持库例程
   模块WIKI：https://wiki.microduino.cn/index.php/MCookie-Module_AudioPro
   串口控制的简易MP3，需配合Microduino_SD模块使用
   歌曲文件名需要按照 track001.mp3 到 track009.mp3排列
   详情见help
*/

#include <Microduino_AudioPro.h>
#include <SD.h>

AudioPro_FilePlayer musicPlayer =  AudioPro_FilePlayer(SD);


uint8_t fileNum = 0;  //文件数量

void playNum(uint8_t num) {
  if (!musicPlayer.paused() && !musicPlayer.stopped()) {
    musicPlayer.stopPlaying();  //必要，否则SD类得不到关闭，内存溢出
  }

  Serial.print(F("Playing:"));
  if (!musicPlayer.playTrack(num)) {
    Serial.println(F("ERROR"));
  }
  else {
    Serial.print(F("OK \t File: "));
    Serial.println(num);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("AudioPro(VS1053) Simple Test");
  pinMode(SD_PIN_SEL, OUTPUT);    //先初始化AudioPro，所以先使能SD卡
  digitalWrite(SD_PIN_SEL, HIGH);
  delay(500);

  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(SD_PIN_SEL)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20, 20);

  // If DREQ is on an interrupt pin, we can do background
  // audio playing
  //musicPlayer.useInterrupt(VS1053_PIN_DREQ);  // DREQ int

  help();
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if ((0x20 <= c) && (c <= 0x126)) {  // strip off non-ASCII, such as CR or LF
      if (c >= '0' && c <= '9') {
        int num = c - 48;
        playNum(num);
      }
      else if (c == 'a') {             //控制运放开关
        if (! musicPlayer.getAmplifier()) {
          Serial.println("Amplifier On");
          musicPlayer.setAmplifier(true);
        } else {
          Serial.println("Amplifier Off");
          musicPlayer.setAmplifier(false);
        }
      }
      else if (c == 's') {    //停止播放音乐
        Serial.println(F("Stopping"));
        musicPlayer.stopPlaying();
      }
      // if we get an 'p' on the serial console, pause/unpause!
      else if (c == 'p') {    //暂停播放音乐
        if (musicPlayer.stopped()) {
          Serial.println("Player is Stopping.");
        }
        else if (! musicPlayer.paused()) {
          Serial.println("Paused");
          musicPlayer.pausePlaying(true);
        } else {
          Serial.println("Resumed");
          musicPlayer.pausePlaying(false);
        }
      }
      else if ((c == '-') || (c == '+')) {  //调整音量
        int _volume;
        if (c == '-') { // note dB is negative
          _volume  = musicPlayer.volumeDown();
        }
        else {
          _volume = musicPlayer.volumeUp();
        }
        Serial.print(F("Volume changed to -"));
        Serial.print(_volume >> 1, 1);
        Serial.println(F("[dB]"));
      }
      else if (c == 'i') {          //显示系统常见信息
        union twobyte mp3_vol; // create key_command existing variable that can be both word and double byte of left and right.
        mp3_vol.word = musicPlayer.getVolume(); // returns a double uint8_t of Left and Right packed into int16_t
        Serial.print("getVolume:");
        Serial.print(mp3_vol.byte[1]);
        Serial.print(",");
        Serial.println(mp3_vol.byte[0]);
        Serial.print("getAmplifier:");
        Serial.println(musicPlayer.getAmplifier());
        Serial.print("getPlaySpeed:");
        Serial.println(musicPlayer.getPlaySpeed());
        Serial.print("decodeTime:");
        Serial.println(musicPlayer.decodeTime());
      }
      else if ((c == '>') || (c == '<')) {  //控制播放速度
        uint16_t playspeed = musicPlayer.getPlaySpeed(); // create key_command existing variable
        // note playspeed of Zero is equal to ONE, normal speed.
        if (c == '>') { // note dB is negative
          // assume equal balance and use byte[1] for math
          if (playspeed >= 254) { // range check
            playspeed = 5;
          } else {
            playspeed += 1; // keep it simpler with whole dB's
          }
        } else {
          if (playspeed == 0) { // range check
            playspeed = 0;
          } else {
            playspeed -= 1;
          }
        }
        musicPlayer.setPlaySpeed(playspeed); // commit new playspeed
        Serial.print(F("playspeed to "));
        Serial.println(playspeed, DEC);
      }
      else if (c == 'm') {          //立体声
        uint16_t monostate = musicPlayer.getMonoMode();
        Serial.print(F("Mono Mode "));
        if (monostate == 0) {
          musicPlayer.setMonoMode(1);
          Serial.println(F("Enabled."));
        } else {
          musicPlayer.setMonoMode(0);
          Serial.println(F("Disabled."));
        }
      }
      else if (c == 'd') {          //差分输出
        uint16_t diff_state = musicPlayer.getDifferentialOutput();
        Serial.print(F("Differential Mode "));
        if (diff_state == 0) {
          musicPlayer.setDifferentialOutput(1);
          Serial.println(F("Enabled."));
        } else {
          musicPlayer.setDifferentialOutput(0);
          Serial.println(F("Disabled."));
        }
      }
      else if (c == 'r') {          //复位VS1053
        Serial.println(F("Reset Player."));
        musicPlayer.reset();
      }
      else if (c == 'n') {          //初始化VS1053
        Serial.println(F("begin Player."));
        musicPlayer.begin();
      }
      else if (c == 'f') {          //关闭VS1053
        Serial.println(F("End Player."));
        musicPlayer.end();
      }
      else if (c == 'h') {          //显示帮助
        help();
      }

    }
  }

  delay(100);
}

void help() {
  Serial.println(F("Microduino AudioPro Library Example:"));
  Serial.println(F("COMMANDS:"));
  Serial.println(F(" [0-9] to play a track"));
  Serial.println(F(" [a] to Set Amplifier"));
  Serial.println(F(" [s] to stop playing"));
  Serial.println(F(" [p] to pause or unpause"));
  Serial.println(F(" [s] to stop playing"));
  Serial.println(F(" [+ or -] to change volume"));
  Serial.println(F(" [> or <] to increment or decrement play speed by 1 factor"));
  Serial.println(F(" [m] Toggle between Mono and Stereo Output."));
  Serial.println(F(" [d] to toggle SM_DIFF between inphase and differential output"));
  Serial.println(F(" [r] Resets and initializes VS10xx chip."));
  Serial.println(F(" [f] turns OFF the VS10xx into low power reset."));
  Serial.println(F(" [n] turns ON the VS10xx out of low power reset."));
  Serial.println(F(" [i] retrieve current audio information (partial list)"));
  Serial.println(F(" [h] this help"));
}