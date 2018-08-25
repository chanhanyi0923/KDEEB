R"(
#version 320 es

uniform mediump float uPointStrength;

out mediump vec4 fragColor;

void main()
{
    mediump float xDis = 2.0 * abs(abs(gl_PointCoord.x) - 0.5);
    mediump float yDis = 2.0 * abs(abs(gl_PointCoord.y) - 0.5);
    mediump float disSquare = xDis * xDis + yDis * yDis;
    mediump float result = max(0.0, 1.0 - disSquare * disSquare);
    fragColor = vec4(uPointStrength * result, 0.0, 0.0, 1.0);
}

)"
