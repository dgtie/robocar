#ifndef I2CM_H_
#define I2CM_H_

void i2cm_write(char addr, int n, char *c);  // reg, data...
void i2cm_read(char addr, char reg, int n, char *c, void (*f)(void)=0);

namespace user
{
bool i2cmWrite(char addr, int n, char *c);  // reg, data...
bool i2cmRead(char addr, char reg, int n, char *c);
}

#endif /* I2CM_H_ */
