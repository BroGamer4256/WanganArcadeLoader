#pragma once
#include "helpers.h"

#pragma pack(8)
struct InputData {
	char version[103];
	u8 unk_67[288];
	bool Test;
	bool Start;
	bool Service;
	bool TestUp;
	bool TestDown;
	u8 unk_18C[2];
	bool TestEnter;
	u8 unk_18F[1];
	bool GearIsTopRow;
	bool GearIsBottomRow;
	bool GearIsLeftColumn;
	bool GearIsRightColumn;
	u8 unk_19C[2];
	bool Perspective;
	bool Intrude;
	u8 unk_1A0[241];
	u8 Wheel;
	u8 unk_28A[1];
	u8 Gas;
	u8 unk_28C[1];
	u8 Brake;
	u8 unk_28E[26];
	bool Coin;
	u8 unk_2A9[67];

	InputData () {
		memset (this, 0, sizeof (InputData));
		this->Wheel = INT8_MAX;
	}

	int GetGearIndex () {
		if (this->GearIsLeftColumn && this->GearIsTopRow) return 1;
		else if (this->GearIsLeftColumn && this->GearIsBottomRow) return 2;
		else if (!this->GearIsLeftColumn && !this->GearIsRightColumn && this->GearIsTopRow) return 3;
		else if (!this->GearIsLeftColumn && !this->GearIsRightColumn && this->GearIsBottomRow) return 4;
		else if (this->GearIsRightColumn && this->GearIsTopRow) return 5;
		else if (this->GearIsRightColumn && this->GearIsBottomRow) return 6;
		else return 0;
	}

	void SetGearIndex (int index) {
		if (index > 6) return;
		this->GearIsTopRow      = false;
		this->GearIsBottomRow   = false;
		this->GearIsLeftColumn  = false;
		this->GearIsRightColumn = false;
		switch (index) {
		case 1:
			this->GearIsTopRow     = true;
			this->GearIsLeftColumn = true;
			break;
		case 2:
			this->GearIsBottomRow  = true;
			this->GearIsLeftColumn = true;
			break;
		case 3: this->GearIsTopRow = true; break;
		case 4: this->GearIsBottomRow = true; break;
		case 5:
			this->GearIsTopRow      = true;
			this->GearIsRightColumn = true;
			break;
		case 6:
			this->GearIsBottomRow   = true;
			this->GearIsRightColumn = true;
			break;
		default: break;
		}
	}
};
