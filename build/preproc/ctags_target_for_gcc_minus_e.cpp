# 1 "/home/mrdarip/git/macroSprig/main/main.ino"
# 2 "/home/mrdarip/git/macroSprig/main/main.ino" 2
# 1 "/home/mrdarip/git/macroSprig/main/main.ino"
// screen libraries
# 3 "/home/mrdarip/git/macroSprig/main/main.ino" 2
# 4 "/home/mrdarip/git/macroSprig/main/main.ino" 2


// SD library
# 8 "/home/mrdarip/git/macroSprig/main/main.ino" 2
// SPI.h shared with screen

# 11 "/home/mrdarip/git/macroSprig/main/main.ino" 2
# 12 "/home/mrdarip/git/macroSprig/main/main.ino" 2

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
  void setAction(int index, KeyBoardAction *action)
  {
    if (index < 0 || index > 7)
    {
      return;
    }
    actions[index] = action;
  }

  void executeAction(int index)
  {
    if (index < 0 || index > 7)
    {
      return;
    }
    actions[index]->execute();
  }

private:
  KeyBoardAction *actions[8];
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
    return digitalRead(pin) == LOW;
  }

private:
  int pin;
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

  screen.initR(0x02);
  screen.fillScreen(0x0000);
  screen.setRotation(3);

  Serial.println("Sprig is ready");




  //left arrow on a key
  KeyBoardAction *actionA = new KeyBoardAction();
  actionA->add(Key(0xD8, PRESS_AND_RELEASE));
  actionsScreen.setAction(1, actionA);

  //right arrow on d key
  KeyBoardAction *actionD = new KeyBoardAction();
  actionD->add(Key(0xD7, PRESS_AND_RELEASE));
  actionsScreen.setAction(3, actionD);

  //up arrow on w key
  KeyBoardAction *actionW = new KeyBoardAction();
  actionW->add(Key(0xDA, PRESS_AND_RELEASE));
  actionsScreen.setAction(0, actionW);

  //down arrow on s key
  KeyBoardAction *actionS = new KeyBoardAction();
  actionS->add(Key(0xD9, PRESS_AND_RELEASE));
  actionsScreen.setAction(2, actionS);

  //press shift on l key
  KeyBoardAction *actionL = new KeyBoardAction();
  actionL->add(Key(0x81, PRESS));
  actionsScreen.setAction(7, actionL);

  //press ctrl+C on i key
  KeyBoardAction *actionI = new KeyBoardAction();
  actionI->add(Key(0x80, PRESS)).add(Key('c', PRESS_AND_RELEASE)).add(Key(0x80, RELEASE));
  actionsScreen.setAction(4, actionI);

  //press ctrl+X on j key
  KeyBoardAction *actionJ = new KeyBoardAction();
  actionJ->add(Key(0x80, PRESS)).add(Key('x', PRESS_AND_RELEASE)).add(Key(0x80, RELEASE));
  actionsScreen.setAction(5, actionJ);

  //press ctrl+V on k key
  KeyBoardAction *actionK = new KeyBoardAction();
  actionK->add(Key(0x80, PRESS)).add(Key('v', PRESS_AND_RELEASE)).add(Key(0x80, RELEASE));
  actionsScreen.setAction(6, actionK);

  for (int i = 0; i < 8; i++)
  {
    buttons[i] = Button(pins[i]);
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
