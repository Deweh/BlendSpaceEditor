#pragma once
#include "NodeTypes.h"
#include <span>
#include <functional>

namespace NodeDefinitions
{
	struct PinDef
	{
		std::string_view typeName;
		std::string_view name;
		PinType type;

		PinDef(const char* _name,
			const char* _typeName,
			PinType _type);
	};

	struct NodeDef
	{
		NodeDef(const char* _name,
			const char* _typeName,
			const std::vector<PinDef>& _inputs,
			const std::vector<PinDef>& _outputs,
			ImColor _color = { 1.0f, 1.0f, 1.0f, 1.0f });

		void CopyToNode(const std::function<int()>& getNextId, Node& dest);

		std::string_view typeName;
		std::string_view name;
		std::vector<PinDef> inputs;
		std::vector<PinDef> outputs;
		ImColor color;
	};

	std::vector<NodeDef*>& GetDefList();
}