Wednesday, May 10, 2006
--------------------------

Implemented RTTI with ObjectType.
class InterfaceControl now inherits from BaseArea. remove m_BackgroundArea from InterfaceControl.

Tuesday, November 23, 2005
--------------------------
I have reach a milestone. I am no longer concern about the feasibility of the GUI. For the most part, it is completed.
Many things have been implemented:
	- file menu
	- status bar
	- modal window dialog
	- ...

At this stage, I am cleaning the project in order to package the GUI in a library. 
From there, I will start designing some applications.


Monday, August 1, 2005
----------------------
The ComboBox is done. For that purpose, the third parameter of OnEvent and ProcessEvent was designed: ProcessEventInfo.
ProcessEventInfo allow each base area OnEvent function to receive the event but not to process it. Previously if a mouse down
was not  inside a window, that window wouldn't propagate the event. Now, the event is propagated up to the OnEvent function and this
one won't process the event if ProcessEventInfo = eDoNotProcess. 
If ProcessEventInfo = eDoNotProcess, only some check about m_CaptureMouseDownAnyWhereElse are done if needed. This feature can also be
use to update timer on dynamic drawing objet attached to the widget (progressive appearance or dissapearance of a popup window for instance).

If a BaseArea object has m_CaptureMouseDownAnyWhereElse = true, then that object want to receive notification every time a mouse down 
happens outside of its geometry.

PushButton is working. ToolButton is on the way.


Friday, july 15, 2005
--------------------
Rearranged the the OpenGL window Client (moved to OpenGLEngine) and GfxServer.
New Table widget (later list box, tree list)
Added DevIL as the texture file reader.
Replace the FreeType code with Font.cpp and Fonts.hpp. This solution is more efficient and better regarding performance.
Various clean Up

Friday, may 20, 2005
--------------------
Fixed some text entry inconsistencies.
Cleanup of signals.
Added Signals and Functions for keyboard entry focus. 

Sunday, May 15, 2005
--------------------
Vertical and Horizontal scrollbars are working. Some minor inconsistencies still remain, but they will be fixed.
Added debug feature for layout:
	- fixed margins
	- drawing of layouts borders with #define DEBUG_LAYOUT


Friday, May 13, 2005
---------------------
Integers and Scrollbar don't fit together. All coordinates and width/height will be converted to float.

Wednesday, May 11, 2005
-----------------------
- Addd vertical and horizontal scrollbar.

2005-05-07
------------
- Added Drawing clipping region stack. This is a feature that clips a region against its parent before the region is drawn.
This is needed for the GUI interface. The GUI Areas are stacked inside other Areas. At the highest level, a window set the clipping region to 
its size. And area inside the Window will then set the clipping region to its own side before it is drawn(PushClippingRectangle). 
What the clipping features does is, it clips the new region against the previous one and pushes the resulting region on the stack and uses it to 
set glScissor. If the result of the intersection is null, the clipping region is set to a null rectangle (nothing is drawn). 
After drawing the area, call PopClippingRectangle to pop the current clipping region and restore the previous clipping region. 

PushClippingRectangle
	- If(there is a current clipping region on the stack)
		compute the intersection between the input region and the region on the stack.
	  Else
		clip the input region against the full window size. 
	- If(the intersection is not null)
		set the glScissor to the size and position of the intersection region.
	  Else
		set glScissor(0, 0, 0, 00 (nothing is drawn)
		
PushClippingRectangle
	- If(there is a current clipping region on the stack)
		- pop it
	- If(the stack is not empty)
		set the clipping region to the size and position of the region on top of the stack
	  Else
		set glScissor to the full window size. 
		

- The original generic template design provided by Loki is no longer par of the design. It has been replaced by a more classic 
architecture featuring inheritance and virtual functions.
Also, A new layout design has been implemented. It took some time and effort before a suitable design could be achieved. I call this design 
"Follow, Rebel then Lead" or FRL. A design document is definitely needed here. At least so I can remember what I did latter.
Some new widget have been implemented, most notably, the RGB color selector.

2005-04-05
------------

I am getting ride of the generic interface for BaseArea. I don't think I need it anymore. 
BaseArea as enough functionality and I can handle it directly to composite widget. 
The free List feature comming with template is not tht useful since there are only few BaseArea that compose a widget.
I will refactor the ValuatorImpl first.