// Adapted from https://github.com/Boomslangnz/FFBArcadePlugin

#include "dllmain.h"
#include "helpers.h"
#include "poll.h"

extern InputData *inputData;
namespace ffb {
int oldGear;

void
update () {
	auto collisions = *(f32 *)ASLR (0x141F2526C);
	auto speed      = *(i32 *)ASLR (0x141F2612C);
	auto gear       = inputData->GetGearIndex ();

	if (speed <= 0) return;

	if (0 < collisions) {
		if (0.209 <= collisions && 0.311 >= collisions) SetRumble (0, collisions * 65535, 150);
		else SetRumble (0, collisions * 65535, 150);
	} else if (0 > collisions) {
		if (-0.209 >= collisions && -0.311 <= collisions) SetRumble (0, collisions * -1.0 * 65535, 150);
		else SetRumble (0, collisions * -1.0 * 65535, 150);
	}

	if (oldGear != gear && gear > 0) SetRumble (0, 0.4 * 65535, 150);
	oldGear = gear;
}
} // namespace ffb
