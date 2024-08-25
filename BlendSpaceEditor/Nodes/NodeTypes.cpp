#include "NodeTypes.h"
#include "NodeDefinitions.h"

void Node::ToJson(nlohmann::json& obj)
{
	auto& inLinks = obj["inputs"];
	auto& values = obj["values"];

	for (auto& i : inputs) {
		if (i.type < PinType::CustomStart)
		{
			auto& connected = std::get<NodeInputConnection>(i.connected);
			auto& curInput = inLinks[i.def->typeName];
			curInput.push_back(connected.nodeId.Get());
			curInput.push_back(connected.typeName);
		} else {
			switch (i.type) {
			case PinType::CustomFloat: values[i.def->typeName] = std::get<NodeFloatCustomValueConnection>(i.connected).value; break;
			case PinType::CustomInt: values[i.def->typeName] = std::get<NodeIntCustomValueConnection>(i.connected).value; break;
			case PinType::CustomString: values[i.def->typeName] = std::get<NodeStringCustomValueConnection>(i.connected).value; break;
			}
		}
	}

	obj["id"] = id.Get();
	obj["type"] = def->typeName;

	auto nodePos = ed::GetNodePosition(id);
	auto& pos = obj["pos"];
	pos.push_back(nodePos.x);
	pos.push_back(nodePos.y);

	if (values.empty()) {
		obj.erase("values");
	}
	if (inLinks.empty()) {
		obj.erase("inputs");
	}
}

void Node::CompactJsonIds(nlohmann::json& arr)
{
	std::unordered_map<size_t, size_t> compactedIds;
	size_t lastCompactedId = 0;

	for (auto& n : arr) {
		size_t newId = ++lastCompactedId;
		compactedIds[n["id"]] = newId;
		n["id"] = newId;
	}

	for (auto& n : arr)
	{
		if (auto inputs = n.find("inputs"); inputs != n.end()) {
			for (auto& i : *inputs) {
				i[0] = compactedIds[i[0]];
			}
		}
	}
}

bool Node::FromJson(nlohmann::json& obj, size_t& maxId)
{
	static auto& defs = NodeDefinitions::GetDefList();
	NodeDefinitions::NodeDef* targetDef = nullptr;
	std::string_view targetTypeName = obj["type"];
	for (auto& d : defs) {
		if (d->typeName == targetTypeName) {
			targetDef = d;
			break;
		}
	}

	if (!targetDef) {
		throw std::exception{ "Node has unknown type." };
	}

	size_t targetId = obj["id"];
	maxId = std::max(targetId, maxId);

	if (maxId > INT32_MAX) {
		throw std::exception{ "[N] Node ID exceeds maximum value." };
	}

	targetDef->CopyToNode([targetId]() -> int {
		return targetId;
	}, *this);
	Build();

	auto& pos = obj["pos"];
	ed::SetNodePosition(id, ImVec2(pos[0], pos[1]));
	auto& inLinks = obj["inputs"];
	auto& values = obj["values"];

	for (auto& i : inputs) {
		if (i.type < PinType::CustomStart)
		{	
			if (auto linkIter = inLinks.find(i.def->typeName); linkIter != inLinks.end()) {
				auto& connected = std::get<NodeInputConnection>(i.connected);
				size_t linkNodeId = (*linkIter)[0];
				std::string_view lDestTypeName = (*linkIter)[1];
				connected.nodeId = linkNodeId;
				connected.typeName = lDestTypeName;
			}
		}
		else {
			auto valIter = values.find(i.def->typeName);
			if (valIter != values.end()) {
				switch (i.type) {
				case PinType::CustomFloat: std::get<NodeFloatCustomValueConnection>(i.connected).value = *valIter; break;
				case PinType::CustomInt: std::get<NodeIntCustomValueConnection>(i.connected).value = *valIter; break;
				case PinType::CustomString: std::get<NodeStringCustomValueConnection>(i.connected).value = *valIter; break;
				}
			}
		}
	}

	return true;
}
