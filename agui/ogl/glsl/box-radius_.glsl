#include "box_.glsl"

#define PIx15 4.71238898038469   // PI * 1.5
#define PI_2 1.5707963267948966  // PI / 2
#define PI_40 0.07853981633974483  // PI / 2 / 20

vec4 vertex() {

  float index = float(gl_VertexID % 21);
  
  float r_x, r_y, x, y;
  vec2 v;
  
  int direction = gl_VertexID / 21;
  
  switch ( direction ) {
    case 0:   // 左上角
      r_x = max(radius_size.x - border_width.x, 0.); // x轴半径
      r_y = max(radius_size.x - border_width.y, 0.); // y轴半径
      x = (1. - cos(PI_40 * index)) * r_x;   // 距离原始顶点的x轴偏移
      y = (sin(PI_40 * index) - 1.) * -r_y;  // 距离原始顶点的y轴偏移
      v = vec2(vertex_ac.x + x, vertex_ac.y + y); // 得到真实坐标
      break;
    case 1:   // 右上角
      r_x = max(radius_size.y - border_width.z, 0.); // x轴半径
      r_y = max(radius_size.y - border_width.y, 0.); // y轴半径
      x = (cos(PI_2 - PI_40 * index) - 1.) * r_x;
      y = (sin(PI_2 - PI_40 * index) - 1.) * -r_y;
      v = vec2(vertex_ac.z + x, vertex_ac.y + y);
      break;
    case 2:   // 右下角
      r_x = max(radius_size.z - border_width.z, 0.);
      r_y = max(radius_size.z - border_width.w, 0.);
      x = (cos(PI_40 * index) - 1.) * r_x;
      y = (sin(PI_40 * index) - 1.) * r_y;
      v = vec2(vertex_ac.z + x, vertex_ac.w + y);
      break;
    default:  // 左下角
      r_x = max(radius_size.w - border_width.x, 0.);
      r_y = max(radius_size.w - border_width.w, 0.);
      x = (cos(PIx15 - PI_40 * index) + 1.) * r_x;
      y = (sin(PIx15 - PI_40 * index) + 1.) * -r_y;
      v = vec2(vertex_ac.x + x, vertex_ac.w + y);
      break;
  }
  
  return vec4(v.xy, 0.0, 1.0);
}
