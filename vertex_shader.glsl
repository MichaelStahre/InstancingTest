#version 330
in vec2 model_position;
in vec3 instance_position;
in vec2 instance_size;
in vec2 texCoord;
out vec2 fragTexCoord;

uniform mat4 proj;
void main() {
  fragTexCoord = texCoord;

  // There're two gl_Position lines below. Uncomment one, comment out the other. The first is the one I'd want to use. The second is for testing the attribute data.

  // This line builds an already translated and scaled matrix. I did the math, it should check out. It think it behaves as if all the instance data is zeroed. including its scale. Instance_size.z doesn't exist and should always be 1.0, model_position.z doesn't exist and should always be 0.0. I put in these values in their appropriate places anyway, for some misguided attempt at clarity.
  //gl_Position = proj * (vec4(instance_size.x*(model_position.x+instance_position.x), instance_size.y*(model_position.y+instance_position.y), 1.0*(0.0+instance_position.z),  instance_size.x*model_position.x+instance_size.y+model_position.y+1.0*1.0));



  // This line is here to confirm if the instance data really is as zeroed as it seems.
  // The first element in the vec4 offsets each instance by its instance id on the x axis, so we can see that they're all here.
  // The second element in vec4 can be used to test the values of instance_position and instance_size.
  // Just substitute instance_position.x for instance_position.y, instance_size.x or instance_size.y.
  // Note that main.cpp sets instance_position.z to 0.0 anyway.
  gl_Position = proj * vec4(model_position.x + gl_InstanceID*0.1, model_position.y + instance_position.x*gl_InstanceID, 0.0, 1.0);

  // Based on tests with this second gl_Position line, it seems instance_position always is (nonzero, zero, zero) and instance_size is (nonzero, zero),
  // no matter what data is in the vertex buffer.
  // The nonzero values of both instance_position and instance_size seem to be the same.
  // I have verified that the correct instance data is stored in the instance buffer and looks perfectly good using gDEBugger, but the shader nevertheless seems to receive this strange data instead
}