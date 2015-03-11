#include "Button.h"
Button::Button(Mesh* m)
{
	mesh = m;
}

Button::~Button()
{
	delete mesh;
}

bool Button::checkAndDraw(bool highlighted)
{
	if (highlighted)
	{
		mesh->SetTexture(highlightTexture);
	}
	else
	{
		mesh->SetTexture(defaultTexture);
	}
	mesh->Draw();
	return true;
}