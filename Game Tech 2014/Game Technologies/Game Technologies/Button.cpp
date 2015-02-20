#include "Button.h"
Button::Button(Mesh* m, Vector2 min, Vector2 max)
{
	mesh = m;
	minPos = min;
	maxPos = max;
}

bool Button::checkAndDraw(Vector2 mouseVec, bool mouseClicked)
{
	bool pressed = false;
	if (mouseVec.x > minPos.x && mouseVec.x < maxPos.x)
	{
		if (mouseVec.y > minPos.y && mouseVec.y < maxPos.y)
		{
			if (mouseClicked)
			{
				mesh->SetTexture(pressedTexture);
				pressed = true;
			}
			else
			{
				mesh->SetTexture(highlightTexture);
			}
		}
		else
		{
			mesh->SetTexture(defaultTexture);
		}
	}
	else
	{
		mesh->SetTexture(defaultTexture);
	}
	mesh->Draw();
	return pressed;
}