#include "requirement.h"
#include "ability.h"

Requirement::Requirement(Ability* parent)
	: QObject(parent)
{
	if (!parent)
		qDebug() << "Warning: Requirement created without a parent.";
	else
		parent->addRequirement(this);
}
