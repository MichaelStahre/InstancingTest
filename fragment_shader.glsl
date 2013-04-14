#version 330
in vec2 fragTexCoord;
out vec4 outColor;
void main() {
  outColor = vec4(0.0, 0.0, 0.0, 0.0);

  // This just creates a fancy-looking faded outline. I originally wrote this to show the hitboxes of the sprites, here I'm using it to verify that there're actually multiple sprites.
  float border = 0.05;
  if (outColor.w == 0) {
    if(fragTexCoord.x <= border && fragTexCoord.y <= border)
      outColor = vec4(0.6, 0.7, 0.6, (fragTexCoord.x/border * fragTexCoord.y/border));

    else if(fragTexCoord.x >= (1.0-border) && fragTexCoord.y >= (1.0-border))
      outColor = vec4(0.6, 0.7, 0.6, (1.0-fragTexCoord.x)/border * (1.0-fragTexCoord.y)/border);

    else if(fragTexCoord.x >= (1.0-border) && fragTexCoord.y <= border)
      outColor = vec4(0.6, 0.7, 0.6, (1.0-fragTexCoord.x)/border * fragTexCoord.y/border);

    else if(fragTexCoord.x <= border && fragTexCoord.y >= (1.0-border))
      outColor = vec4(0.6, 0.7, 0.6, fragTexCoord.x/border * (1.0-fragTexCoord.y)/border);

    else if(fragTexCoord.x <= border)
      outColor = vec4(0.6, 0.7, 0.6, (fragTexCoord.x/border));

    else if(fragTexCoord.y <= border)
      outColor = vec4(0.6, 0.7, 0.6, (fragTexCoord.y/border));

    else if(fragTexCoord.x >= 1.0-border)
      outColor = vec4(0.6, 0.7, 0.6, (1.0-fragTexCoord.x)/border);

    else if(fragTexCoord.y >= 1.0-border)
      outColor = vec4(0.6, 0.7, 0.6, (1.0-fragTexCoord.y)/border);
  }
}