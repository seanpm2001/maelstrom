/*
    Maelstrom: Open Source version of the classic game by Ambrosia Software
    Copyright (C) 1997-2011  Sam Lantinga

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#include "screenlib/SDL_FrameBuf.h"

#include "MacDialog.h"

#define EXPAND_STEPS 30


UIElementType MacDialog::s_elementType;


MacDialog::MacDialog(UIManager *ui, const char *name) :
	UIDialog(ui, name)
{
	m_colors[COLOR_BLACK] = m_screen->MapRGB(0x00, 0x00, 0x00);
	m_colors[COLOR_DARK] = m_screen->MapRGB(0x66, 0x66, 0x99);
	m_colors[COLOR_MEDIUM] = m_screen->MapRGB(0xBB, 0xBB, 0xBB);
	m_colors[COLOR_LIGHT] = m_screen->MapRGB(0xCC, 0xCC, 0xFF);
	m_colors[COLOR_WHITE] = m_screen->MapRGB(0xFF, 0xFF, 0xFF);
#ifdef FAST_ITERATION
	m_expand = false;
#else
	m_expand = true;
#endif
	m_step = 0;
}

bool
MacDialog::Load(rapidxml::xml_node<> *node, const UITemplates *templates)
{
	if (!UIDialog::Load(node, templates)) {
		return false;
	}

	LoadBool(node, "expand", m_expand);

	return true;
}

void
MacDialog::Show()
{
	if (m_expand) {
		m_step = 0;
	} else {
		m_step = EXPAND_STEPS;
	}

	UIDialog::Show();
}

void
MacDialog::Draw()
{
	int x, y, w, h;
	int maxx, maxy;

	if (m_step < EXPAND_STEPS) {
		w = (Width()*m_step)/EXPAND_STEPS;
		h = (Height()*m_step)/EXPAND_STEPS;
		x = X() + Width()/2 - (w/2);
		y = Y() + Height()/2 - (h/2);
	} else {
		w = Width();
		h = Height();
		x = X();
		y = Y();
	}

	/* The border is 4 pixels around the area of the dialog */
	w += 8;
	h += 8;
	x -= 4;
	y -= 4;
	maxx = x+w-1;
	maxy = y+h-1;

	/* Draw the dialog border and background color */
	m_screen->DrawLine(x, y, maxx, y, m_colors[COLOR_LIGHT]);
	m_screen->DrawLine(x, y, x, maxy, m_colors[COLOR_LIGHT]);
	m_screen->DrawLine(x, maxy, maxx, maxy, m_colors[COLOR_DARK]);
	m_screen->DrawLine(maxx, y, maxx, maxy, m_colors[COLOR_DARK]);
	m_screen->DrawRect(x+1, y+1, w-2, h-2, m_colors[COLOR_MEDIUM]);
	m_screen->DrawLine(x+2, y+2, maxx-2, y+2, m_colors[COLOR_DARK]);
	m_screen->DrawLine(x+2, y+2, x+2, maxy-2, m_colors[COLOR_DARK]);
	m_screen->DrawLine(x+3, maxy-2, maxx-2, maxy-2, m_colors[COLOR_LIGHT]);
	m_screen->DrawLine(maxx-2, y+3, maxx-2, maxy-2, m_colors[COLOR_LIGHT]);
	m_screen->DrawRect(x+3, y+3, w-6, h-6, m_colors[COLOR_BLACK]);
	m_screen->FillRect(x+4, y+4, w-8, h-8, m_colors[COLOR_WHITE]);

	/* Don't draw the controls until we've finished expanding */
	if (m_step < EXPAND_STEPS) {
		++m_step;
		return;
	}

	UIDialog::Draw();
}
