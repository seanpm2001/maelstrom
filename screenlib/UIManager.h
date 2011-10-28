/*
    SCREENLIB:  A framebuffer library based on the SDL library
    Copyright (C) 1997  Sam Lantinga

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

#ifndef _UIManager_h
#define _UIManager_h

#include "SDL.h"
#include "../utils/array.h"
#include "UIArea.h"
#include "UITemplates.h"

class FrameBuf;
class UIManager;
class UIPanel;
class UIElement;

typedef UIPanel *(*UIPanelFactory)(UIManager *ui, const char *type, const char *name, const char *delegate);
typedef UIElement *(*UIElementFactory)(UIPanel *panel, const char *name);
typedef void (*UISoundCallback)(void *, int soundID);

class UIManager : public UIArea
{
public:
	UIManager(FrameBuf *screen, UIPanelFactory panelFactory, UIElementFactory elementFactory);
	~UIManager();

	FrameBuf *GetScreen() const {
		return m_screen;
	}
	UIPanelFactory GetPanelFactory() const {
		return m_panelFactory;
	}
	UIElementFactory GetElementFactory() const {
		return m_elementFactory;
	}
	const UITemplates *GetTemplates() const {
		return &m_templates;
	}

	void SetLoadPath(const char *path);
	bool LoadTemplates(const char *file);
	UIPanel *LoadPanel(const char *name);
	UIPanel *GetPanel(const char *name, bool allowLoad = true);
	UIPanel *GetCurrentPanel();

	/* These are called by the UIPanel class */
	void AddPanel(UIPanel *panel) {
		if (!m_panels.find(panel)) {
			m_panels.add(panel);
		}
	}
	void RemovePanel(UIPanel *panel) {
		m_visible.remove(panel);
		m_panels.remove(panel);
	}

	void ShowPanel(UIPanel *panel);
	void ShowPanel(const char *name) {
		ShowPanel(GetPanel(name));
	}
	void HidePanel(UIPanel *panel);
	void HidePanel(const char *name) {
		HidePanel(GetPanel(name));
	}
	void DeletePanel(UIPanel *panel);
	void DeletePanel(const char *name) {
		DeletePanel(GetPanel(name));
	}

	void SetSoundCallback(UISoundCallback callback, void *param) {
		m_soundCallback = callback;
		m_soundCallbackParam = param;
	}
	void PlaySound(int soundID) {
		if (m_soundCallback) {
			m_soundCallback(m_soundCallbackParam, soundID);
		}
	}
			
	void Draw(bool fullUpdate = true);
	bool HandleEvent(const SDL_Event &event);

protected:
	UIPanelFactory m_panelFactory;
	UIElementFactory m_elementFactory;
	UISoundCallback m_soundCallback;
	void *m_soundCallbackParam;
	char *m_loadPath;
	UITemplates m_templates;
	array<UIPanel *> m_panels;
	array<UIPanel *> m_visible;
};

#endif // _UIManager_h
