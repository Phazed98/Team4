#include "Button.h"
Button::Button(Mesh* m)
{
	mesh = m;
}

bool Button::checkAndDraw(bool highlighted, VertexShader &vertex, FragmentShader &fragment)
{
	if (highlighted)
	{
	//	mesh->SetDefaultTexture(*highlightTexture);
	}
	else
	{
	//	mesh->SetDefaultTexture(*defaultTexture);
	}
//	mesh->

	//Renderer::GetRenderer()->SetTextureSampler(fragment->GetParameter("texture"), GetDefaultTexture());
	mesh->Draw(vertex, fragment);
	return true;
}
