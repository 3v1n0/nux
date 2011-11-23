
Automated Tests
---------------
Since the instruction for the simulated events are comming from a separate thread, the tests are very sensible
to timing and can generate false negatives. For this reason, in some location of the code, there are sleeping periodes that have been added to allow time for events to be processed and for results to be generated.

All test must be over in 20 seconds or less.

Description of tests:
---------------------
graphics_display_states:
	Test the graphics display states after it has been created.

empty_window button_xtest:
	Display an empty window and close it after 3 seconds.

button_xtest:
	Using XTest, love the mouse around the boundaries of a button view and simulate click and drag mouse events. The program close itself after a delay.

mouse_events_test:
	Test the following mouse events:
		- mouse down
		- mouse up
		- mouse click
		- mouse double-click
		- mouse drag
		- mouse enter
		- mouse leave
		- mouse move


mouse_button_tests:
	Make sure that only mouse button 1, 2 and 3 generate mouse-down and mouse-up events.
	The artificial button event 4, 5, 6... should not generate mouse-down or mouse-up events.

	
		