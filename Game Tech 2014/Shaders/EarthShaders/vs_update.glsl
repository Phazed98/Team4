#version 330 core

in float p_type;
in vec3 p_position;
in vec3 p_velocity;
in float p_age;
in float p_index;

 out particle{
	 float Type0;
	 vec3 Position0;
	 vec3 Velocity0;
	 float Age0;
	 float Index0;
 } OUT;

void main()
{
	OUT.Type0 = p_type;
	OUT.Position0 = p_position;
	OUT.Velocity0 = p_velocity;
	OUT.Age0 = p_age;
	OUT.Index0 = p_index;
}