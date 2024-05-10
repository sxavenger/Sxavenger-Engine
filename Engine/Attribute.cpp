#include "Attribute.h"

#include "Console.h"

void Attribute::SetThisAttribute(const std::string& name) {
	name_ = name;
	Console::GetInstance()->SetAttribute(this);
}
