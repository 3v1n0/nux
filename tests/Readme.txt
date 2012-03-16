
Automated Tests
---------------
Since the instruction for the simulated events are comming from a separate thread, the tests are very sensible
to timing and can generate false negatives. For this reason, in some location of the code, there are sleeping periodes that have been added to allow time for events to be processed and for results to be generated.

All test must be over in 20 seconds or less.

Note on the Test: When using the nux::TestView class, a small white square at the top left corner of the TestView area indicate 
that the view has keyboard focus.

Description of tests:
---------------------
graphics_display_states:
	Test the graphics display states after it has been created.

empty_window:
	Display an empty window and close it after 3 seconds.

xtest-button:
	Using XTest, love the mouse around the boundaries of a button view and simulate click and drag mouse events. The program close itself after a delay.

xtest-mouse-events:
	Test the following mouse events:
		- mouse down
		- mouse up
		- mouse click
		- mouse double-click
		- mouse drag
		- mouse enter
		- mouse leave
		- mouse move


xtest-mouse-buttons:
	Make sure that only physical mouse button 1, 2 and 3 generate mouse-down and mouse-up events.
	The artificial button event 4 and 5 should not generate mouse-down or mouse-up events.
	
xtest-hgrid-key-navigation
  Make sure that the key navigation works well in a GridHLayout. 
  
xtest-hlayout-key-navigation
  Make sure that the key naviagation works well in a HLayout.
  
xtest-vlayout-key-navigation
  Make sure that the key navigation works well in a VLayout.

xtest-focus-on-mouse-down
  Make sure that AcceptKeyNavFocusOnMouseDown works well.

xtest-focus-on-mouse-enter
  Make sure that AcceptKeyNavFocusOnMouseEnter works well.

xtest-keynav-direction
  Test key navigation. Especially test the direction a key nav is coming from when the view is getting the focus.

xtest-text-entry
  Simulate various operations on the text entry

xtest-text-entry-logic
  Test the state of TextEntry::text_input_mode_ before and after some text is typed in.
