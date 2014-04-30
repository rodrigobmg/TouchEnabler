TouchEnabler
============

This application adds touch controls to DirectX games.

Usage
=====
TouchEnabler.exe -> Select application exe -> Launch

The target application or game needs a TouchEnabler script (.tes) located in the same directory with the same name. For example, for Need for Speed Underground 2, you'd need a speed2.tes script located in C:\Program Files (x86)\EA GAMES\Need for Speed Underground 2\ or wherever the game is installed.

.tes scripts have a C-like syntax, and they are used to have different layouts and controller behaviors for each game. Each script must implement 4 functions:

void OnLoad(): the UI textures should be loaded in this function, as well as setting any parameters such as enabling the mouse as touch input (mostly for debugging purposes). This function is only called once, at the beggining of execution of the script.

void OnRender(): Any DrawSprite calls should be located here.

int OnPointerDown(int x, int y): This function is called whenever the user taps or starts a manipulation on the screen. The event coordinates are sent as the parameters. The script should return -1 if the event is not going to be processed, otherwise it should return an id which is then used as a parameter for the OnPointerUp and OnManipulation functions to which UI element was being interacted with.

void OnPointerUp(int id): This is called when the user stops pressing or interacting with an element.

Other functions that can be implemented:
void OnManipulation(int id, float tx, float ty, float rot, float scale): This function is called when the application detects a gesture such as pan, pinch to zoom, etc.

API
===
These functions and variables are provided by the host application to be used within the scripts:

void SetInfoText(string): This sets a debugging text shown in the upper-left corner of the window.
void MouseMove(int x, int y): This moves the cursor to the specified absolute coordinates.
void MouseClick(): Triggers a left mouse button click.
void AddTouchHandler(): (important) Adds a touch handler, there should be one for each UI element, i.e buttons.
void PressKey(int): Sets the specified key code as being pressed. (see key codes below)
void ReleaseKey(int): Releases the specified key code.
bool PointInCircle(float circle_x, float circle_y, float radius, float point_x, float point_y): Helper function for circle-shaped UI elements
int AddSprite(string texture_file, int width, int height): Load a texture to be used as a sprite. texture_file can be an absolute path or one relative to the target application. Returns the sprite id.
void DrawSprite(uint spite_id, float x, float y, float r, float g, float b, float a): (important) Draws the sprite at the specified coordinates (top-left system) and RGBA color.
void echo(string): Prints a string into the touchenabler.log file.
bool EnableMouseInPointer(bool): Wrapper for the WinAPI function.
void exit(int): Wrapper for the exit function. Quits the target application.

const uint dpiScaleX, dpiScaleY: Number of pixels per logical inch along the screen height.

Example
=======

////////////////////////////
//////// sample.tes ////////
////////////////////////////
const int x1 = 0; // Button coordinates
const int y1 = 0;

void OnLoad()
{
  AddSprite("button.png", 114, 114); // Load our button texture
	AddTouchHandler(); // Adds a touch handler, one for each button
}

void OnRender()
{
	DrawSprite(0, x1, y1, 255, 255, 255, 155); // Draw our button
}

int OnPointerDown(int x, int y)
{
  if(PointInCircle(x1, y1, 57, x, y))
	{
		PressKey(0x01); // Press ESC key, see key codes below.
		SetInfoText("Esc button being pressed");
		return 0; // We'll set 0 as the code belonging to our button.
	}
	return -1; // Event was outside button, then we're not gonna process it.
}

void OnPointerUp(int id)
{
  if(id == 0) // We got 0, so that means our button was released.
	{
		ReleaseKey(0x01); // Release ESC key
		SetInfoText("No button being pressed");
	}
}
////////////////////////////
////////////////////////////

Roadmap
=======
*Complete vJoystick implementation as it is better suited for FPS and racing games.
*Add Direct3D 7,8,10,11 support
*Visual UI editor

Known bugs, limitations
=======================
*BUG: The game can't be minimized, alt-tabbed or device reset (such as changing the resolution or visual settings) as it is gonna crash due to the Direct3D hook code.
*BUG: The TouchEnabler application must be restarted after launching a game as it won't work in the successive attempts.
*Limitation: Only Direct3D 9 is being supported at this time.
*Limitation: Sprite textures and the .TES script can only be located in the same directory as the target executable.


DirectX keyboard scan codes
===========================
#define DIK_ESCAPE          0x01
#define DIK_1               0x02
#define DIK_2               0x03
#define DIK_3               0x04
#define DIK_4               0x05
#define DIK_5               0x06
#define DIK_6               0x07
#define DIK_7               0x08
#define DIK_8               0x09
#define DIK_9               0x0A
#define DIK_0               0x0B
#define DIK_MINUS           0x0C    /* - on main keyboard */
#define DIK_EQUALS          0x0D
#define DIK_BACK            0x0E    /* backspace */
#define DIK_TAB             0x0F
#define DIK_Q               0x10
#define DIK_W               0x11
#define DIK_E               0x12
#define DIK_R               0x13
#define DIK_T               0x14
#define DIK_Y               0x15
#define DIK_U               0x16
#define DIK_I               0x17
#define DIK_O               0x18
#define DIK_P               0x19
#define DIK_LBRACKET        0x1A
#define DIK_RBRACKET        0x1B
#define DIK_RETURN          0x1C    /* Enter on main keyboard */
#define DIK_LCONTROL        0x1D
#define DIK_A               0x1E
#define DIK_S               0x1F
#define DIK_D               0x20
#define DIK_F               0x21
#define DIK_G               0x22
#define DIK_H               0x23
#define DIK_J               0x24
#define DIK_K               0x25
#define DIK_L               0x26
#define DIK_SEMICOLON       0x27
#define DIK_APOSTROPHE      0x28
#define DIK_GRAVE           0x29    /* accent grave */
#define DIK_LSHIFT          0x2A
#define DIK_BACKSLASH       0x2B
#define DIK_Z               0x2C
#define DIK_X               0x2D
#define DIK_C               0x2E
#define DIK_V               0x2F
#define DIK_B               0x30
#define DIK_N               0x31
#define DIK_M               0x32
#define DIK_COMMA           0x33
#define DIK_PERIOD          0x34    /* . on main keyboard */
#define DIK_SLASH           0x35    /* / on main keyboard */
#define DIK_RSHIFT          0x36
#define DIK_MULTIPLY        0x37    /* * on numeric keypad */
#define DIK_LMENU           0x38    /* left Alt */
#define DIK_SPACE           0x39
#define DIK_CAPITAL         0x3A
#define DIK_F1              0x3B
#define DIK_F2              0x3C
#define DIK_F3              0x3D
#define DIK_F4              0x3E
#define DIK_F5              0x3F
#define DIK_F6              0x40
#define DIK_F7              0x41
#define DIK_F8              0x42
#define DIK_F9              0x43
#define DIK_F10             0x44
#define DIK_NUMLOCK         0x45
#define DIK_SCROLL          0x46    /* Scroll Lock */
#define DIK_NUMPAD7         0x47
#define DIK_NUMPAD8         0x48
#define DIK_NUMPAD9         0x49
#define DIK_SUBTRACT        0x4A    /* - on numeric keypad */
#define DIK_NUMPAD4         0x4B
#define DIK_NUMPAD5         0x4C
#define DIK_NUMPAD6         0x4D
#define DIK_ADD             0x4E    /* + on numeric keypad */
#define DIK_NUMPAD1         0x4F
#define DIK_NUMPAD2         0x50
#define DIK_NUMPAD3         0x51
#define DIK_NUMPAD0         0x52
#define DIK_DECIMAL         0x53    /* . on numeric keypad */
#define DIK_F11             0x57
#define DIK_F12             0x58

