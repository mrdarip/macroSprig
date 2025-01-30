# Sprig macro

Create your own macro board with the Sprig console!!

currently it only supports creating a "screen" of macros and they are hard-flashed to the sprig memory.

## Installation

Hold the boot button on the sprig and plug it into your computer. It will show up as a USB drive. Drag the `.uf2` file onto the drive.

## Editing the macros

Edit main.ino to change the macros.
At the end of the void setup function, you can add your own macros.

The syntax is:
```cpp
KeyBoardAction *variableToStoreTheMacro = new KeyBoardAction();

actionJ->add(Key(char or keycode, PRESS | RELEASE | PRESSANDRELEASE)).add(...);

actionsScreen.setAction(pin, variableToStoreTheMacro);
```

you can chain the `add` method to add multiple keys to the macro to create more complex macros.
as an example:
```cpp
KeyBoardAction *actionK = new KeyBoardAction();
  actionK->add(Key(KEY_LEFT_CTRL, PRESS)).add(Key('v', PRESS_AND_RELEASE)).add(Key(KEY_LEFT_CTRL, RELEASE));
    actionsScreen.setAction(4, actionK);
```

sets the macro on pin 4 (k) to
1. press (without release) the left control key
2. press and release the 'v' key
3. release the left control key