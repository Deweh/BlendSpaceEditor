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
		NodeCategoryType _category,
		const std::vector<PinDef>& _inputs,
		const std::vector<PinDef>& _outputs) :
		name(_name),
		typeName(_typeName),
		category(_category),
		inputs(_inputs.begin(), _inputs.end()),
		outputs(_outputs.begin(), _outputs.end())
	{
		using enum NodeCategoryType;
		switch (_category) {
		case PoseCreators:
		case PoseModifiers:
			color = { 147, 226, 74, 255 };
			break;
		case ValueCreators:
		case ValueModifiers:
			color = { 177, 3, 252, 255 };
			break;
		case VectorCreators:
		case VectorModifiers:
			color = { 252, 227, 0, 255 };
			break;
		case DataCreators:
			color = { 0, 227, 252, 255 };
			break;
		case Actor:
			color = { 255, 0, 0, 255 };
			break;
		default:
			color = { 255, 255, 255, 255 };
		}
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
		NodeCategoryType::PoseCreators,
		{
			{ "File", "file", PinType::CustomString},
			{ "Sync ID", "syncId", PinType::CustomInt },
			{ "Speed Modifier (Optional)", "speedMod", PinType::Float }
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		}
	};

	static NodeDef StaticPoseNode{
		"Static Pose",
		"pose",
		NodeCategoryType::PoseCreators,
		{
			{ "File", "file", PinType::CustomString}
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		},
	};

	static NodeDef BasePoseNode{
		"Base Game Pose",
		"base_pose",
		NodeCategoryType::PoseCreators,
		{
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		},
	};

	static NodeDef BlendSpace1DNode{
		"Blend Space 1D",
		"blend_1d",
		NodeCategoryType::PoseModifiers,
		{
			{ "Pose 1", "1", PinType::Pose},
			{ "Pose 2", "2", PinType::Pose},
			{ "Value Input", "val", PinType::Float}
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		}
	};

	static NodeDef AdditiveBlendNode{
		"Additive Blend",
		"blend_add",
		NodeCategoryType::PoseModifiers,
		{
			{ "Additive Pose", "add", PinType::Pose},
			{ "Full Pose", "full", PinType::Pose},
			{ "Value Input", "val", PinType::Float}
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		},
	};

	static NodeDef IKTwoBoneAdjNode{
		"IK Two Bone",
		"ik_2b_adj",
		NodeCategoryType::PoseModifiers,
		{
			{ "Input Pose", "pose", PinType::Pose },
			{ "Start Bone", "start_node", PinType::CustomString},
			{ "Mid Bone", "mid_node", PinType::CustomString},
			{ "End Bone", "end_node", PinType::CustomString},
			{ "Mid Axis X", "mid_x", PinType::CustomFloat},
			{ "Mid Axis Y", "mid_y", PinType::CustomFloat},
			{ "Mid Axis Z", "mid_z", PinType::CustomFloat},
			{ "IK Target", "target", PinType::Vector}
		},
		{
			{ "Output Pose", "output", PinType::Pose }
		}
	};

	static NodeDef IKOneBoneNode{
		"IK One Bone",
		"ik_1b",
		NodeCategoryType::PoseModifiers,
		{
			{ "Input Pose", "pose", PinType::Pose },
			{ "Bone Name", "bone", PinType::CustomString},
			{ "Up Axis X", "up_x", PinType::CustomFloat},
			{ "Up Axis Y", "up_y", PinType::CustomFloat},
			{ "Up Axis Z", "up_z", PinType::CustomFloat},
			{ "Forward Axis X", "forward_x", PinType::CustomFloat},
			{ "Forward Axis Y", "forward_y", PinType::CustomFloat},
			{ "Forward Axis Z", "forward_z", PinType::CustomFloat},
			{ "IK Target", "target", PinType::Vector},
			{ "Bone End Offset", "offset", PinType::Vector}
		},
		{
			{ "Output Pose", "output", PinType::Pose }
		}
	};

	static NodeDef SpringBoneNode{
		"Spring Bone",
		"spring_bone",
		NodeCategoryType::PoseModifiers,
		{
			{ "Input Pose", "pose", PinType::Pose },
			{ "Bone Name", "bone", PinType::CustomString},
			{ "Linear Spring Properties (Optional)", "linearProps", PinType::Data},
			{ "Angular Spring Properties (Optional)", "angularProps", PinType::Data},
			{ "Linear Constraint (Optional)", "linearConstr", PinType::Data},
			{ "Angular Constraint (Optional)", "angularConstr", PinType::Data},
		},
		{
			{ "Output Pose", "output", PinType::Pose }
		}
	};

	static NodeDef SetBoneRotationNode{
		"Set Bone Rotation",
		"set_bone_rot",
		NodeCategoryType::PoseModifiers,
		{
			{ "Input Pose", "pose", PinType::Pose},
			{ "Bone Name", "bone", PinType::CustomString},
			{ "Model Space", "is_ms", PinType::CustomBool},
			{ "Input Rotation", "rot", PinType::Vector}
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		}
	};

	static NodeDef SetBonePositionNode{
		"Set Bone Position",
		"set_bone_pos",
		NodeCategoryType::PoseModifiers,
		{
			{ "Input Pose", "pose", PinType::Pose},
			{ "Bone Name", "bone", PinType::CustomString},
			{ "Model Space", "is_ms", PinType::CustomBool},
			{ "Input Position", "position", PinType::Vector}
		},
		{
			{ "Output Pose", "output", PinType::Pose}
		}
	};

	static NodeDef FixedValueNode{
		"Fixed Value",
		"fixed_val",
		NodeCategoryType::ValueCreators,
		{
			{ "Value", "val", PinType::CustomFloat},
		},
		{
			{ "Value Output", "output", PinType::Float}
		}
	};

	static NodeDef VariableNode{
		"Variable",
		"var",
		NodeCategoryType::ValueCreators,
		{
			{ "Name", "name", PinType::CustomString},
			{ "Default Value", "defVal", PinType::CustomFloat}
		},
		{
			{ "Value Output", "output", PinType::Float}
		}
	};

	static NodeDef SmoothedRandomValueNode{
		"Smoothed Random Value",
		"smooth_rand",
		NodeCategoryType::ValueCreators,
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
		}
	};

	static NodeDef LimitROCNode{
		"Limit Rate-of-Change",
		"limit_roc",
		NodeCategoryType::ValueModifiers,
		{
			{ "Value Input", "input", PinType::Float},
			{ "Rate-of-Change/s", "roc", PinType::CustomFloat}
		},
		{
			{ "Value Output", "output", PinType::Float}
		}
	};

	static NodeDef SmoothValNode{
		"Smooth Value",
		"smooth_val",
		NodeCategoryType::ValueModifiers,
		{
			{ "Value Input", "input", PinType::Float},
			{ "Percent/s", "percent", PinType::CustomFloat}
		},
		{
			{ "Value Output", "output", PinType::Float}
		}
	};

	static NodeDef TransformRangeNode{
		"Transform Range",
		"transform_range",
		NodeCategoryType::ValueModifiers,
		{
			{ "Value Input", "input", PinType::Float},
			{ "Old Min", "oldMin", PinType::CustomFloat},
			{ "Old Max", "oldMax", PinType::CustomFloat},
			{ "New Min", "newMin", PinType::CustomFloat},
			{ "New Max", "newMax", PinType::CustomFloat}
		},
		{
			{ "Value Output", "output", PinType::Float}
		}
	};

	static NodeDef GetBoneRotationNode{
		"Get Bone Rotation",
		"get_bone_rot",
		NodeCategoryType::VectorCreators,
		{
			{ "Input Pose", "pose", PinType::Pose},
			{ "Bone Name", "bone", PinType::CustomString},
			{ "Model Space", "is_ms", PinType::CustomBool}
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef GetBonePositionNode{
		"Get Bone Position",
		"get_bone_pos",
		NodeCategoryType::VectorCreators,
		{
			{ "Input Pose", "input", PinType::Pose},
			{ "Bone Name", "bone", PinType::CustomString},
			{ "Model Space", "ms", PinType::CustomBool}
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef FixedVectorNode{
		"Fixed Vector",
		"fixed_vec",
		NodeCategoryType::VectorCreators,
		{
			{ "X", "x", PinType::CustomFloat},
			{ "Y", "y", PinType::CustomFloat},
			{ "Z", "z", PinType::CustomFloat},
			{ "W", "w", PinType::CustomFloat},
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef MakeVectorNode{
		"Make Vector",
		"make_vec",
		NodeCategoryType::VectorCreators,
		{
			{ "X", "x", PinType::Float},
			{ "Y", "y", PinType::Float},
			{ "Z", "z", PinType::Float},
			{ "W", "w", PinType::Float},
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef LocalToModelNode{
		"Local to Model Space",
		"local_to_model",
		NodeCategoryType::VectorModifiers,
		{
			{ "Input Pose", "pose", PinType::Pose},
			{ "Parent Bone Name", "parentBone", PinType::CustomString},
			{ "Local Position", "vec", PinType::Vector}
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef AddVectorsNode{
		"Add Vectors",
		"add_vecs",
		NodeCategoryType::VectorModifiers,
		{
			{ "Vector 1", "1", PinType::Vector},
			{ "Vector 2", "2", PinType::Vector}
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef SubtractVectorsNode{
		"Subtract Vectors",
		"sub_vecs",
		NodeCategoryType::VectorModifiers,
		{
			{ "Vector 1", "1", PinType::Vector},
			{ "Vector 2", "2", PinType::Vector}
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef DivideVectorsNode{
		"Divide Vectors",
		"div_vecs",
		NodeCategoryType::VectorModifiers,
		{
			{ "Dividend Vector", "1", PinType::Vector},
			{ "Divisor Vector", "2", PinType::Vector}
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef MultiplyVectorsNode{
		"Multiply Vectors",
		"mult_vecs",
		NodeCategoryType::VectorModifiers,
		{
			{ "Vector 1", "1", PinType::Vector},
			{ "Vector 2", "2", PinType::Vector}
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef AddRotationVectorsNode{
		"Add Rotation Vectors",
		"add_rot_vecs",
		NodeCategoryType::VectorModifiers,
		{
			{ "Vector 1", "1", PinType::Vector},
			{ "Vector 2", "2", PinType::Vector}
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef SubtractRotationVectorsNode{
		"Subtract Rotation Vectors",
		"sub_rot_vecs",
		NodeCategoryType::VectorModifiers,
		{
			{ "Vector 1", "1", PinType::Vector},
			{ "Vector 2", "2", PinType::Vector}
		},
		{
			{ "Vector Output", "output", PinType::Vector}
		}
	};

	static NodeDef SpringPropsNode{
		"Spring Properties",
		"spring_props",
		NodeCategoryType::DataCreators,
		{
			{ "Up Axis X", "up_x", PinType::CustomFloat},
			{ "Up Axis Y", "up_y", PinType::CustomFloat},
			{ "Up Axis Z", "up_z", PinType::CustomFloat},
			{ "Stiffness", "stiff", PinType::Float},
			{ "Damping", "damp", PinType::Float},
			{ "Mass", "mass", PinType::Float},
			{ "Stretch Resistance (Optional)", "resist", PinType::Float},
			{ "Gravity (Optional)", "gravity", PinType::Vector},
		},
		{
			{ "Data Out", "output", PinType::Data}
		}
	};

	static NodeDef LinearBoxConstrNode{
		"Linear Box Constraint",
		"linear_box_constr",
		NodeCategoryType::DataCreators,
		{
			{ "Box Minimum", "min", PinType::Vector},
			{ "Box Maximum", "max", PinType::Vector},
			{ "Bounce (Optional)", "bounce", PinType::Float},
			{ "Spring (Optional)", "spring", PinType::Data},
		},
		{
			{ "Data Out", "output", PinType::Data}
		}
	};

	static NodeDef LinearSphereConstrNode{
		"Linear Sphere Constraint",
		"linear_sphere_constr",
		NodeCategoryType::DataCreators,
		{
			{ "Radius", "radius", PinType::Float},
			{ "Bounce (Optional)", "bounce", PinType::Float},
			{ "Spring (Optional)", "spring", PinType::Data},
		},
		{
			{ "Data Out", "output", PinType::Data}
		}
	};

	static NodeDef AngularConeConstrNode{
		"Angular Cone Constraint",
		"angle_cone_constr",
		NodeCategoryType::DataCreators,
		{
			{ "Half Angle", "halfAngle", PinType::Float},
			{ "Bounce (Optional)", "bounce", PinType::Float},
			{ "Spring (Optional)", "spring", PinType::Data},
		},
		{
			{ "Data Out", "output", PinType::Data}
		}
	};

	static NodeDef ActorNode{
		"Actor",
		"actor",
		NodeCategoryType::Actor,
		{
			{ "Input Pose", "input", PinType::Pose}
		},
		{
		}
	};

}