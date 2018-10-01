#pragma once

#include "AttributeFactory.h"

namespace Topologic
{
	ref class DoubleAttributeFactory : AttributeFactory
	{
	public:
		[IsVisibleInDynamoLibrary(false)]
		virtual Attribute^ Create(String^ key, Object^ value) override;

		[IsVisibleInDynamoLibrary(false)]
		virtual bool CheckType(Type^ type) override;
	protected:
	};
}