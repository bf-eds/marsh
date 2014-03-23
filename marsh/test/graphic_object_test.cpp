/*
 *  Copyright (C) 2013 by Felipe Lavratti
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in the
 *  Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 *  Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 *  AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

extern "C" {
#include "color.h"
#include "color_private.h"
#include "widget.h"
#include "widget_interface.h"
#include "widget_private.h"
}

#include "mocks/terminal_intercepter.h"

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetector.h"

static bool called = false;
static void call(void *){
	called = true;
}

TEST_GROUP(Widget)
{
	widget_t * cut;
	widget_interface_t * owner;

	void setup()
	{
		marshmallow_terminal_output = output_intercepter;
		owner = widget_interface_new(NULL, NULL, NULL);
		cut = widget_new(owner);
		called = false;
	}

	void teardown()
	{
		widget_delete(cut);
		widget_interface_delete(owner);
		marshmallow_terminal_output = _stdout_output_impl;
	}
};

TEST(Widget, instance)
{
	CHECK_EQUAL(cut->dim.size_set, false);
	CHECK_EQUAL(cut->dim.pos_start_set, false);
	CHECK_EQUAL(cut->dim.pos_end_set, false);

	CHECK_EQUAL((void*)0, cut->interface->owner_instance);
	CHECK_EQUAL((void(*)(void*))0, cut->interface->draw);
	CHECK_EQUAL((void(*)(void*))0, cut->interface->destroy);

//	widget_delete(cut);
//	widget_delete(cut);
//	STRCMP_CONTAINS("widget", intercepted_output[2]);
//	STRCMP_CONTAINS("Invalid Instance", intercepted_output[0]);
//
//	widget_interface_delete(owner);
//	widget_interface_delete(owner);
//	STRCMP_CONTAINS("widget_interface", intercepted_output[2]);
//	STRCMP_CONTAINS("Invalid Instance", intercepted_output[0]);
}

TEST(Widget, destroy)
{
	widget_t * cut2;
	widget_interface_t * owner2;
	owner2 = widget_interface_new(this, (void(*)(void*))2, call);
	cut2 = widget_new(owner2);
	widget_delete_interface(cut2);
	widget_delete(cut2);
	widget_interface_delete(owner2);
	CHECK_TRUE(called);
}
