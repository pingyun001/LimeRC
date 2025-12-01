#ifndef __LIME_CAN_FLITER_H
#define __LIME_CAN_FLITER_H

#include "main.h"


void Lime_can_filter_init(void);
void CAN_cmd_chassis(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);

uint8_t Lime_CanIsOnline(void);                     //≈–∂œCAN «∑Ò…œœﬂ

#endif  //__LIME_CAN_FLITER_H

