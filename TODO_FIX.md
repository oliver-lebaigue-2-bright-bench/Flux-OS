# GUI Keyboard Fix TODO List

## Issues Identified:
1. Missing `enable_mouse()` declaration in `gui.h`
2. Keyboard polling in GUI event loop may not be working properly
3. Misleading "press any key" message in kernel.c
4. Need better keyboard controller initialization

## Fixes Completed:

### 1. Add enable_mouse() declaration to gui.h
- ✅ Added function declaration to gui.h mouse input section
- ✅ Also added mouse_disable() and mouse_set_position() declarations

### 2. Improve keyboard initialization in keyboard.c
- ✅ Added proper PS/2 controller initialization
- ✅ Added keyboard buffer clearing with timeout
- ✅ Added keyboard enable command (0xF4) for data reporting
- ✅ Added ACK handling

### 3. Fix misleading message in kernel.c
- ✅ Changed "Press any key to start GUI" to "Starting GUI now..."

### 4. Improve keyboard polling in desktop.c gui_run()
- ✅ Added PS/2 controller configuration
- ✅ Added keyboard port enable/disable commands
- ✅ Added proper status port checking
- ✅ Added buffer clearing before keyboard initialization

### 5. Rebuild and test
- ✅ Build completed successfully with no warnings

