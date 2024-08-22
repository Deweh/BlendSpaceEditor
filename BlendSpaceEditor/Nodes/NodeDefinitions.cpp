#include "NodeDefinitions.h"

namespace NodeDefinitions
{

	PinDef::PinDef(const char* _name,
		const char* _typeName,
		PinType _type) :
		name(_name),
		typeName(_typeName),
		type(_type)
	{}

	NodeDef::NodeDef(const char* _name,
		const char* _typeName,
		const std::vector<PinDef>& _inputs,
		const std::vector<PinDef>& _outputs,
		ImColor _color) :
		name(_name),
		typeName(_typeName),
		inputs(_inputs.begin(), _inputs.end()),
		outputs(_outputs.begin(), _outputs.end()),
		color(_color)
	{
		GetDefList().emplace_back(this);
	}

	void NodeDef::CopyToNode(const std::function<int()>& getNextId, Node& dest)
	{
		dest.id = getNextId();
		dest.name = name;
		dest.color = color;
		dest.def = this;

		for (auto& i : inputs) {
			dest.inputs.emplace_back(getNextId(), i.name.data(), i.type).def = &i;
		}

		for (auto& o : outputs) {
			dest.outputs.emplace_back(getNextId(), o.name.data(), o.type).def = &o;
		}
	}

	std::vector<NodeDef*>& GetDefList()
	{
		static std::vector<NodeDef*> list;
		return list;
	}

	// Start of Definitions

	static NodeDef FullAnimationNode{
		"Full Animation",
		"anim",
		{
			{ "File", "file", PinType::CustomString},
			{ "Sync ID", "syncId", PinType::CustomInt },
			{ "Speed Modifier (Optional)", "speedMod", PinType::Float }
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		},
		{ 147, 226, 74, 255 }
	};

	static NodeDef BlendSpace1DNode{
		"Blend Space 1D",
		"blend_1d",
		{
			{ "Pose 1", "1", PinType::Pose},
			{ "Pose 2", "2", PinType::Pose},
			{ "Value Input", "val", PinType::Float}
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		},
		{ 147, 226, 74, 255 }
	};

	static NodeDef AdditiveBlendNode{
		"Additive Blend",
		"blend_add",
		{
			{ "Additive Pose", "add", PinType::Pose},
			{ "Full Pose", "full", PinType::Pose},
			{ "Value Input", "val", PinType::Float}
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		},
		{ 147, 226, 74, 255 }
	};

	static NodeDef FixedValueNode{
		"Fixed Value",
		"fixed_val",
		{
			{ "Value", "val", PinType::CustomFloat},
		},
		{
			{ "Value Output", "output", PinType::Float}
		},
		{ 177, 3, 252, 255  }
	};

	static NodeDef VariableNode{
		"Variable",
		"var",
		{
			{ "Name", "name", PinType::CustomString},
			{ "Default Value", "defVal", PinType::CustomFloat}
		},
		{
			{ "Value Output", "output", PinType::Float}
		},
		{ 177, 3, 252, 255  }
	};

	static NodeDef LimitROCNode{
		"Limit Rate-of-Change",
		"limit_roc",
		{
			{ "Value Input", "input", PinType::Float},
			{ "Rate-of-Change/s", "roc", PinType::CustomFloat}
		},
		{
			{ "Value Output", "output", PinType::Float}
		},
		{ 177, 3, 252, 255  }
	};

	static NodeDef TransformRangeNode{
		"Transform Range",
		"transform_range",
		{
			{ "Value Input", "input", PinType::Float},
			{ "Old Min", "oldMin", PinType::CustomFloat},
			{ "Old Max", "oldMax", PinType::CustomFloat},
			{ "New Min", "newMin", PinType::CustomFloat},
			{ "New Max", "newMax", PinType::CustomFloat}
		},
		{
			{ "Value Output", "output", PinType::Float}
		},
		{ 177, 3, 252, 255  }
	};

	static NodeDef SmoothedRandomValueNode{
		"Smoothed Random Value",
		"smooth_rand",
		{
			{ "Smooth Duration Min", "dur_min", PinType::CustomFloat},
			{ "Smooth Duration Max", "dur_max", PinType::CustomFloat },
			{ "Differential Min", "diff_min", PinType::CustomFloat },
			{ "Differential Max", "diff_max", PinType::CustomFloat },
			{ "Delay Min", "delay_min", PinType::CustomFloat },
			{ "Delay Max", "delay_max", PinType::CustomFloat },
			{ "Edge Threshold", "edge", PinType::CustomFloat },
			{ "Sync ID", "syncId", PinType::CustomInt }
		},
		{
			{ "Value Output", "output", PinType::Float}
		},
		{ 177, 3, 252, 255  }
	};

	static NodeDef ActorNode{
		"Actor",
		"actor",
		{
			{ "Input Pose", "input", PinType::Pose}
		},
		{
		},
		{ 255, 0, 0, 255 }
	};

}