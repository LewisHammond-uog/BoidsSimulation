#include "PrimitiveComponent.h"

//Typedefs
typedef Component PARENT;


PrimitiveComponent::PrimitiveComponent(Entity* a_pOwner) : PARENT(a_pOwner)
{
}


/// <summary>
/// Set the colour of the primitive
/// </summary>
/// <param name="a_v4Colour">New Color to set the primitive in the format (R: 0-1, G: 0-1, B: 0-1, A: 0-1)</param>
void PrimitiveComponent::SetColour(const glm::vec4 a_v4Colour)
{
	m_v4DrawColour = a_v4Colour;
}