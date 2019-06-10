#include <lamos.h>
#include "wheel.h"
#include "usart1.h"

namespace
{

class PID {
public:
  PID(int p, int in, int id, int d) { P = p; In = in; Id = id; acc = last = 0; }
  int control(int x, int target) {
    int error = x - target;
    int diff = x - last;
    last = x; acc += error;
    if (acc > MAX) acc = MAX;
    if (acc < -MAX) acc = -MAX;
    return - P * error - In * (acc >> Id) - D * diff;
  }
private:
  int P, In, Id, D, acc, last;
  enum {MAX=2000000000};
} pid_speed(1000,10,0,0), pid_track(1000,50,0,0);

class Speed {
public:
  Speed(void) { speed = 0; }
  int log(int i) {
    speed <<= 4; speed |= i;
    i = speed;
    int j = 0;
    while (i) { j += i & 0xf; i >>= 4; }
    return j;
  }
private:
  int speed;
} left, right;

struct {unsigned char left, right; unsigned short floor;} s;

} //anonymous

void wheel(wheel_struct &val) {
  s.floor = val.floor;
  int lt = left.log(val.left);
  int rt = right.log(val.right);
  s.left = lt; s.right = rt;
  int cs = pid_speed.control(lt + rt, 4);
  int ct = pid_track.control(lt, rt);
  lt = cs + ct; rt = cs -ct;
  val.left = lt > 0 ? lt : 0;
  val.right = rt > 0 ? rt : 0;
}

Timer display(200, []() {
  usart1_tx((char*)&s, 4);
  return 200U;
});
