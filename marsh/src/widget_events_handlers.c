/*
 *  Copyright (C) 2013 to 2014 by Felipe Lavratti
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

#include "helper/checks.h"
#include "helper/log.h"
#include "widget.h"
#include "widget_event.h"
#include "widget_tree.h"
#include "widget_private.h"
#include "event.h"
#include "area.h"

static bool code_is_interaction(event_code_t code)
{
	if (code == event_code_interaction_click)
		return true;

	if (code == event_code_interaction_release)
		return true;

	if (code == event_code_interaction_press)
		return true;

	return false;
}

static bool widget_sibling_area_contain_point(widget_t * widget, point_t point)
{
	if (!widget)
		return false;

	while(widget_left_sibling(widget))
	{
		widget = widget_left_sibling(widget);
	}

	while(widget)
	{
		if (area_contains_point(widget_area(widget), point))
			return true;

		widget = widget_right_sibling(widget);
	}

	return false;
}

static void default_interaction_event_consume(widget_t * widget, event_t * event)
{
	event_code_t code = event_code(event);

	PTR_CHECK(widget, __FUNCTION__);
	PTR_CHECK(event, __FUNCTION__);

	code = event_code(event);

	if (code == event_code_interaction_click)
		widget_click(widget);
	else if (code == event_code_interaction_press)
		widget_press(widget);
	else if (code == event_code_interaction_release)
		widget_release(widget);
}

enum e_widget_event_handler_result
default_interaction_event_handler(widget_t * widget, event_t * event)
{
	interaction_event_data_t * interaction_data;

	PTR_CHECK_RETURN(widget, __FUNCTION__, widget_event_not_consumed);
	PTR_CHECK_RETURN(event, __FUNCTION__, widget_event_not_consumed);

	if (!code_is_interaction(event_code(event)))
	{
		LOG_ERROR(__FUNCTION__, "Received invalid event.");
		return widget_event_not_consumed;
	}

	interaction_data = (interaction_event_data_t *)event_data(event);
	if (!interaction_data)
	{
		LOG_ERROR(__FUNCTION__, "Received incomplete event.");
		return widget_event_not_consumed;
	}

	if (area_contains_point(widget_area(widget), interaction_data->interaction_point))
	{
		if (widget_child(widget))
		{
			if (widget_sibling_area_contain_point(widget_child(widget), interaction_data->interaction_point))
			{
				return widget_event_not_consumed;
			}
		}

		default_interaction_event_consume(widget, event);
		return widget_event_consumed;
	}

	return widget_event_not_consumed;
}

enum e_widget_event_handler_result
default_delete_event_handler(widget_t * widget, event_t * event)
{
	PTR_CHECK_RETURN(widget, __FUNCTION__, widget_event_not_consumed);
	PTR_CHECK_RETURN(event, __FUNCTION__, widget_event_not_consumed);

	if (event_code(event) != event_code_delete)
	{
		LOG_ERROR(__FUNCTION__, "Received invalid event.");
		return widget_event_not_consumed;
	}

	widget_delete(widget);

	return widget_event_consumed;
}

enum e_widget_event_handler_result
default_draw_event_handler(widget_t * widget, event_t * event)
{
	PTR_CHECK_RETURN(widget, __FUNCTION__, widget_event_not_consumed);
	PTR_CHECK_RETURN(event, __FUNCTION__, widget_event_not_consumed);

	area_t * limiting_area;

	if (event_code(event) != event_code_draw)
	{
		LOG_ERROR(__FUNCTION__, "Received invalid event.");
		return widget_event_not_consumed;
	}

	if (!widget_visible(widget))
		return widget_event_consumed;

	if (widget_parent(widget))
		limiting_area = &widget_parent(widget)->tmp_canvas_area;
	else
		limiting_area = NULL;

	widget_draw(widget, limiting_area);

	widget->tmp_canvas_area = widget_compute_canvas_area(widget, limiting_area);

	return widget_event_consumed;
}

