#include <el/WindowImpl_linux.hpp>
#include <stdlib.h>

// #include <iostream>
// using std::cout;
// using std::endl;

using namespace engineLibrary;

// TODO: i don't know what's going on here
void WindowImpl::initWindow() {
	const auto c = xcb_connect (nullptr, nullptr);

	/* get the first screen */
	const auto screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

	/* create the window */
	const auto windowID = xcb_generate_id(c);
	const auto mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	const uint32_t values[] = {
		screen->white_pixel,
		XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS,
	};
	xcb_create_window (c,                  /* connection    */
			XCB_COPY_FROM_PARENT,          /* depth         */
			windowID,                      /* window Id     */
			screen->root,                  /* parent window */
			0, 0,                          /* x, y          */
			800, 600,                      /* width, height */
			10,                            /* border_width  */
			XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class         */
			screen->root_visual,           /* visual        */
			mask, values);                 /* masks         */

	/* map the window on the screen */
	xcb_map_window (c, windowID);

	xcb_flush (c);

	this->_surfaceInfo.connection = c;
	this->_surfaceInfo.window = windowID;
}

static bool open = true;

bool WindowImpl::isOpen() const {
	return open; // TODO: fix asap
}

void WindowImpl::processMessages() {
	xcb_generic_event_t *event;
	while ( (event = xcb_poll_for_event (this->_surfaceInfo.connection)) ) {
		if((event->response_type & ~0x80) == XCB_KEY_PRESS) open = false;
		free(event);
	}
}

VulkanSurfaceInfo WindowImpl::getSurfaceInfo() {
	return this->_surfaceInfo;
}
