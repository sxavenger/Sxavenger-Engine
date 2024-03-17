#include "Particle.h"

//=========================================================================================
// static variables
//=========================================================================================

const char* Particle::billBoardTypeStr_[BillBoardType::kCountOfBillBoardType] 
	= { "BillBoard_None", "BillBoard_2D", "BillBoard_3D" };

////////////////////////////////////////////////////////////////////////////////////////////
// Particle methods
////////////////////////////////////////////////////////////////////////////////////////////

void Particle::SetElements(const Element& element) {
	elements_.push_back(element);
}

void Particle::SetOnImGuiBillBoardType() {
	if (ImGui::BeginCombo("BillBoardType", billBoardTypeStr_[billBoardType_])) {
		for (int i = 0; i < BillBoardType::kCountOfBillBoardType; ++i) {
			bool isSelected = (i == billBoardType_);

			if (ImGui::Selectable(billBoardTypeStr_[i], isSelected)) {
				billBoardType_ = static_cast<BillBoardType>(i);
			}
		}

		ImGui::EndCombo();
	}
}

void Particle::TermBase() {
	elements_.clear();
}
