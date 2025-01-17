#include <Arduino.h>

// screen libraries
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// SD library
#include <SD.h>
// SPI.h shared with screen
#include <ArduinoJson.h>

#include <vector>
#include <Keyboard.h>

Adafruit_ST7735 screen = Adafruit_ST7735(20, 22, 26);

File root;
bool hasSdCard = false;

enum ActionType
{
  PRESS,
  RELEASE,
  PRESS_AND_RELEASE
};

class Key
{
public:
  Key(char keycode, ActionType actionType)
  {
    this->keycode = keycode;
    this->actionType = actionType;
  }

  char getKey()
  {
    return this->keycode;
  }

  ActionType getActionType()
  {
    return this->actionType;
  }

private:
  char keycode;
  ActionType actionType;
};

class Action
{
public:
  virtual void execute() = 0;
};

class KeyBoardAction : public Action
{
public:
  void execute()
  {
    for (Key key : keys)
    {
      switch (key.getActionType())
      {
      case PRESS:
        Keyboard.press(key.getKey());
        break;
      case RELEASE:
        Keyboard.release(key.getKey());
        break;
      case PRESS_AND_RELEASE:
        Keyboard.print(key.getKey());
        break;
      }
    }
  }

  KeyBoardAction &add(Key key)
  {
    keys.push_back(key);
    return *this;
  }

private:
  std::vector<Key> keys;
};

class ActionsScreen
{
public:
  void setAction(int index, KeyBoardAction *action, ActionType actionType)
  {
    if (index < 0 || index > 7 || actionType == PRESS_AND_RELEASE) //press and release is not allowed
    {
      return;
    }

    if(actionType == PRESS){
      actionsPress[index] = action;
    } else if(actionType == RELEASE){
      actionsRelease[index] = action;
    }
  }

  void executeAction(int index, ActionType actionType)
  {
    if (index < 0 || index > 7 || actionType == PRESS_AND_RELEASE) //press and release is not allowed
    {
      return;
    }

    if(actionType == PRESS){
      actionsPress[index]->execute();
    } else if(actionType == RELEASE){
      actionsRelease[index]->execute();
    }
  }
private:
  KeyBoardAction *actionsPress[8];
  KeyBoardAction *actionsRelease[8];
};

ActionsScreen actionsScreen;

class Button
{
public:
  Button(int pin)
  {
    this->pin = pin;
    pinMode(pin, INPUT_PULLUP);
  }

  bool isPressed()
  {
    bool pressed = digitalRead(pin) == LOW;

    if(pressed){
      if(millis == 0){
        pressedMilli = millis();
        return true;
      }
    } else{
      pressedMilli = 0;
    }

    return false;
  }

  bool isHold(){
    return getPressedTime() > holdTreshold;
  }

private:
  int pin;
  unsigned long pressedMilli;
  const unsigned long holdTreshold = 500;

  unsigned long getPressedTime(){
    unsigned long pressedTime = millis()-pressedMilli;
    if(pressedMilli == 0){
      return 0;
    }
    return pressedMilli;
  }
};

int pins[] = {5, 6, 7, 8, 12, 13, 14, 15};
char buttonchars[] = {'w', 'a', 's', 'd', 'i', 'j', 'k', 'l'};
Button buttons[8] = {Button(pins[0]), Button(pins[1]), Button(pins[2]), Button(pins[3]), Button(pins[4]), Button(pins[5]), Button(pins[6]), Button(pins[7])};

void setup()
{
  SPI.setRX(16);
  SPI.setTX(19);
  SPI.setSCK(18);

  hasSdCard = SD.begin(21);
  Serial.begin(9600);

  if (hasSdCard)
  {
    root = SD.open("/");
  }

  screen.initR(INITR_BLACKTAB);
  screen.fillScreen(ST77XX_BLACK);
  screen.setRotation(3);


  Keyboard.begin();

  //get the macrosprig.json file
  File macroFile = SD.open("/macrosprig.json");
  if (macroFile)
  {
    Serial.println("macrosprig.json found");
    macroFile.close();
  }
  else
  {
    Serial.println("macrosprig.json not found");
  }

  Serial.println("Sprig is ready");

  StaticJsonDocument<2048> doc;

  DeserializationError error = deserializeJson(doc, macroFile);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  JsonObject keys = doc["keys"];

  for (int i = 0; i < 8; i++)
  {
    
  }
}

void loop()
{
  for(int i = 0; i < 8; i++){
    if(buttons[i].isPressed()){
      actionsScreen.executeAction(i);
    }
  }

  delay(100);
}

/*
sample macro file content
{
    "keys": {
        "w": {
            "p": [
                {
                    "key": "UP_ARROW",
                    "action": "pr"
                }
            ],
            "r": []
        },
        "a": {
            "p": [
                {
                    "key": "LEFT_ARROW",
                    "action": "pr"
                }
            ],
            "r": []
        },
        "s": {
            "p": [
                {
                    "key": "DOWN_ARROW",
                    "action": "pr"
                }
            ],
            "r": []
        },
        "d": {
            "p": [
                {
                    "key": "RIGHT_ARROW",
                    "action": "pr"
                }
            ],
            "r": []
        },
        "i": {
            "p": [
                {
                    "key": "LEFT_CTRL",
                    "action": "p"
                },
                {
                    "key": "c",
                    "action": "pr"
                },
                {
                    "key": "LEFT_CTRL",
                    "action": "r"
                }
            ],
            "r": []
        },
        "j": {
            "p": [
                {
                    "key": "LEFT_CTRL",
                    "action": "p"
                },
                {
                    "key": "x",
                    "action": "pr"
                },
                {
                    "key": "LEFT_CTRL",
                    "action": "r"
                }
            ],
            "r": []
        },
        "k": {
            "p": [
                {
                    "key": "LEFT_CTRL",
                    "action": "p"
                },
                {
                    "key": "v",
                    "action": "pr"
                },
                {
                    "key": "LEFT_CTRL",
                    "action": "r"
                }
            ],
            "r": []
        },
        "l": {
            "p": [
                {
                    "key": "LEFT_SHIFT",
                    "action": "p"
                }
            ],
            "r": [
                {
                    "key": "LEFT_SHIFT",
                    "action": "r"
                }
            ]
        }
    }
}
*/