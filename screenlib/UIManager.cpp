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

#include "SDL_FrameBuf.h"
#include "UIManager.h"
#include "UIPanel.h"


UIManager::UIManager(FrameBuf *screen, UIElementFactory factory) : UIArea(screen)
{
	m_elementFactory = factory;
	m_soundCallback = NULL;
	m_soundCallbackParam = NULL;
	m_loadPath = new char[2];
	strcpy(m_loadPath, ".");
}

UIManager::~UIManager()
{
	/* Deleting the panels will remove them from the manager */
	while (m_panels.length() > 0) {
		delete m_panels[m_panels.length()-1];
	}
}

void
UIManager::SetLoadPath(const char *path)
{
	delete[] m_loadPath;
	m_loadPath = new char[strlen(path)+1];
	strcpy(m_loadPath, path);
}

static const char *GetLine(char *&text)
{
	while (*text == '\r' || *text == '\n') {
		++text;
	}
	if (!*text) {
		return NULL;
	}

	const char *line = text;
	while (*text && *text != '\r' && *text != '\n') {
		++text;
	}
	if (*text) {
		*text++ = '\0';
	}
	return line;
}

UIPanel *
UIManager::LoadPanel(const char *name)
{
	UIPanel *panel;

	panel = GetPanel(name, false);
	if (!panel) {
		char file[1024];

		sprintf(file, "%s/%s.xml", m_loadPath, name);
		panel = new UIPanel(this, name);
		if (!panel->Load(file)) {
			SetError("%s", panel->Error());
			delete panel;
			return false;
		}
	}
	return panel;
}

UIPanel *
UIManager::GetPanel(const char *name, bool allowLoad)
{
	for (unsigned i = 0; i < m_panels.length(); ++i) {
		if (strcmp(name, m_panels[i]->GetName()) == 0) {
			return m_panels[i];
		}
	}
	if (allowLoad) {
		return LoadPanel(name);
	}
	return NULL;
}

UIPanel *
UIManager::GetCurrentPanel()
{
	if (m_visible.length() > 0) {
		return m_visible[m_visible.length()-1];
	}
	return NULL;
}

void
UIManager::ShowPanel(UIPanel *panel)
{
	if (panel && !m_visible.find(panel)) {
		/* If this is fullscreen, then hide any previous fullscreen panel */
		if (panel->IsFullscreen()) {
			for (unsigned int i = m_visible.length(); i--; ) {
				if (m_visible[i]->IsFullscreen()) {
					HidePanel(m_visible[i]);
					break;
				}
			}
		}

		m_visible.add(panel);
		panel->Show();
		if (panel->IsFullscreen()) {
			Draw();
			m_screen->FadeIn();
		}
	}
}

void
UIManager::HidePanel(UIPanel *panel)
{
	if (panel && m_visible.remove(panel)) {
		panel->Hide();
		if (panel->IsFullscreen()) {
			m_screen->FadeOut();

			for (unsigned int i = m_visible.length(); i--; ) {
				if (m_visible[i]->IsFullscreen()) {
					m_visible[i]->Show();
					Draw();
					m_screen->FadeIn();
					break;
				}
			}
		}

	}
}

void
UIManager::DeletePanel(UIPanel *panel)
{
	if (panel) {
		HidePanel(panel);
		delete panel;
	}
}

void
UIManager::Draw(bool fullUpdate)
{
	if (fullUpdate) {
		m_screen->Clear();
	}
	for (unsigned i = 0; i < m_visible.length(); ++i) {
		UIPanel *panel = m_visible[i];

		panel->Draw();

		if (panel->IsFullscreen()) {
			break;
		}
	}
	if (fullUpdate) {
		m_screen->Update();
	}
}

bool
UIManager::HandleEvent(const SDL_Event &event)
{
	for (unsigned i = m_visible.length(); i--; ) {
		UIPanel *panel = m_visible[i];

		if (panel->HandleEvent(event)) {
			return true;
		}
		if (panel->IsFullscreen()) {
			break;
		}
	}
	return false;
}
