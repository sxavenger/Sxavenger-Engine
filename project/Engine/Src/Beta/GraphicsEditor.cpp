#include "GraphicsEditor.h"

GraphicsEditor* GraphicsEditor::GetInstance() {
	static GraphicsEditor instance;
	return &instance;
}
