#ifndef WHEEL_H_
#define WHEEL_H_

typedef struct {unsigned short left, right; unsigned floor;} wheel_struct;
typedef enum {STOP, FORWARD, BACKWARD, LEFT, RIGHT} wheel_mode;

void wheel(wheel_mode);

// user please provide
// void wheel(wheel_struct&);
// input wheel count in the struct
// ouput wheel control to the struct

#endif /* WHEEL_H_ */
