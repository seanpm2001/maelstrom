
#include "UIElementTitle.h"
#include "load.h"

UIElementType UIElementTitle::s_elementType;


UIElementTitle::UIElementTitle(UIPanel *panel, const char *name) :
	UIElementTexture(panel, name)
{
}

bool
UIElementTitle::Load(rapidxml::xml_node<> *node)
{
	rapidxml::xml_attribute<> *attr;

	attr = node->first_attribute("id", 0, false);
	if (!attr) {
		SetError("Element '%s' missing attribute 'id'", node->name());
		return false;
	}

	SDL_Texture *texture = Load_Title(m_screen, atoi(attr->value()));
	if (!texture) {
		SetError("Unable to load title %d", atoi(attr->value()));
		return false;
	}
	SetTexture(texture);

	return UIElementTexture::Load(node);
}
