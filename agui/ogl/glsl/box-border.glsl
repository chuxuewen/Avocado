#version 300 es
#include "box_.glsl"

out vec4 f_color;

#define vertex0 vec2(vertex_ac.x - border_width.x, vertex_ac.y - border_width.y)
#define vertex1 vec2(vertex_ac.z + border_width.z, vertex_ac.y - border_width.y)
#define vertex2 vec2(vertex_ac.z + border_width.z, vertex_ac.w + border_width.w)
#define vertex3 vec2(vertex_ac.x - border_width.x, vertex_ac.w + border_width.w)
#define vertex4 vec2(vertex_ac.x, vertex_ac.y)
#define vertex5 vec2(vertex_ac.z, vertex_ac.y)
#define vertex6 vec2(vertex_ac.z, vertex_ac.w)
#define vertex7 vec2(vertex_ac.x, vertex_ac.w)

// 使用4顶点的实例绘制, 一个实例绘制一条边

void main() {
  
  vec2 v;
  
  switch ( gl_InstanceID ) {
    case 0:
      switch ( gl_VertexID ) {
        case 0:   v = vertex3; break;
        case 1:   v = vertex0; break;
        case 2:   v = vertex4; break;
        default:  v = vertex7; break;
      }
      f_color = vec4(border_left_color.rgb, border_left_color.a * opacity);
      break;
    case 1:
      switch ( gl_VertexID ) {
        case 0:   v = vertex0; break;
        case 1:   v = vertex1; break;
        case 2:   v = vertex5; break;
        default:  v = vertex4; break;
      }
      f_color = vec4(border_top_color.rgb, border_top_color.a * opacity);
      break;
    case 2:
      switch ( gl_VertexID ) {
        case 0:   v = vertex1; break;
        case 1:   v = vertex2; break;
        case 2:   v = vertex6; break;
        default:  v = vertex5; break;
      }
      f_color = vec4(border_right_color.rgb, border_right_color.a * opacity);
      break;
    default:
      switch ( gl_VertexID ) {
        case 0:   v = vertex2; break;
        case 1:   v = vertex3; break;
        case 2:   v = vertex7; break;
        default:  v = vertex6; break;
      }
      f_color = vec4(border_bottom_color.rgb, border_bottom_color.a * opacity);
      break;
  }
  
  gl_Position = root_matrix * view_matrix * vec4(v.xy, 0.0, 1.0);
}

#frag
#version 300 es

in  lowp vec4 f_color;
out lowp vec4 FragColor;

void main() {
  FragColor = f_color;
}

