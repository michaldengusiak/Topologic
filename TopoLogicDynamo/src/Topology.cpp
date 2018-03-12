#include <msclr/marshal_cppstd.h>

#include "Topology.h"

#include <Cluster.h>
#include <CellComplex.h>
#include <Cell.h>
#include <Shell.h>
#include <Face.h>
#include <Wire.h>
#include <Edge.h>
#include <Vertex.h>
#include <Attribute.h>
#include <Context.h>

#include <TopoDS_Shape.hxx>

namespace TopoLogic
{
	int Topology::Dimensionality::get()
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		return pCoreTopology->Dimensionality();
	}

	Topology^ Topology::ByGeometry(Autodesk::DesignScript::Geometry::Geometry^ geometry)
	{
		throw gcnew System::NotImplementedException();
		// TODO: insert return statement here
	}

	Topology^ Topology::ByContext()
	{
		throw gcnew System::NotImplementedException();
		// TODO: insert return statement here
	}

	Topology^ Topology::ByVertexIndex(List<array<double, 3>^>^ vertexCoordinates, List<List<int>^>^ vertexIndices)
	{
		throw gcnew System::NotImplementedException();
		// TODO: insert return statement here
	}

	Topology^ Topology::ByVertexIndex(List<Vertex^>^ vertexCoordinates, List<List<int>^>^ vertexIndices)
	{
		throw gcnew System::NotImplementedException();
		// TODO: insert return statement here
	}

	Dictionary<String^, Attribute^>^ Topology::Attributes::get()
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		const TopoLogicCore::Topology::AttributeMap& rkCoreAttributes = pCoreTopology->Attributes();

		Dictionary<String^, Attribute^>^ pAttributes = gcnew Dictionary<String^, Attribute^>();
		for (TopoLogicCore::Topology::AttributeMap::const_iterator rkCoreAttributeIterator = rkCoreAttributes.cbegin();
			rkCoreAttributeIterator != rkCoreAttributes.cend();
			rkCoreAttributeIterator++)
		{
			pAttributes->Add(gcnew String(rkCoreAttributeIterator->first.c_str()), gcnew Attribute(rkCoreAttributeIterator->second));
		}

		return pAttributes;
	}

	bool Topology::SaveToBRep(String^ path)
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		std::string cppPath = msclr::interop::marshal_as<std::string>(path);
		return pCoreTopology->SaveToBrep(cppPath);
	}

	bool Topology::LoadFromBRep(String^ path)
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		std::string cppPath = msclr::interop::marshal_as<std::string>(path);
		return pCoreTopology->LoadFromBrep(cppPath);
	}

	String^ Topology::Analyze()
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		return gcnew String(pCoreTopology->Analyze().c_str());
	}

	Topology^ Topology::ByCoreTopology(TopoLogicCore::Topology * const kpCoreTopology)
	{
		TopAbs_ShapeEnum occtShapeType = kpCoreTopology->GetOcctShape()->ShapeType();
		switch (occtShapeType)
		{
		case TopAbs_COMPOUND: return gcnew Cluster(TopoLogicCore::Topology::Downcast<TopoLogicCore::Cluster>(kpCoreTopology));
		case TopAbs_COMPSOLID: return gcnew CellComplex(TopoLogicCore::Topology::Downcast<TopoLogicCore::CellComplex>(kpCoreTopology));
		case TopAbs_SOLID: return gcnew Cell(TopoLogicCore::Topology::Downcast<TopoLogicCore::Cell>(kpCoreTopology));
		case TopAbs_SHELL: return gcnew Shell(TopoLogicCore::Topology::Downcast<TopoLogicCore::Shell>(kpCoreTopology));
		case TopAbs_FACE: return gcnew Face(TopoLogicCore::Topology::Downcast<TopoLogicCore::Face>(kpCoreTopology));
		case TopAbs_WIRE: return gcnew Wire(TopoLogicCore::Topology::Downcast<TopoLogicCore::Wire>(kpCoreTopology));
		case TopAbs_EDGE: return gcnew Edge(TopoLogicCore::Topology::Downcast<TopoLogicCore::Edge>(kpCoreTopology));
		case TopAbs_VERTEX: return gcnew Vertex(TopoLogicCore::Topology::Downcast<TopoLogicCore::Vertex>(kpCoreTopology));
		default:
			throw std::exception("Topology::ByCoreTopology: unknown topology.");
		}
	}

	Topology::Topology()
	{

	}

	Topology::~Topology()
	{

	}

	List<Topology^>^ Topology::Contents()
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		const std::list<TopoLogicCore::Topology*>& rkCoreContents = pCoreTopology->Contents();

		List<Topology^>^ pTopologies = gcnew List<Topology^>();

		for (std::list<TopoLogicCore::Topology*>::const_iterator rkCoreContentIterator = rkCoreContents.cbegin();
			rkCoreContentIterator != rkCoreContents.cend();
			rkCoreContentIterator++)
		{
			pTopologies->Add(Topology::ByCoreTopology(*rkCoreContentIterator));
		}

		return pTopologies;
	}

	List<Context^>^ Topology::Contexts()
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		const std::list<TopoLogicCore::Context*>& rkCoreContexts = pCoreTopology->Contexts();

		List<Context^>^ pContexts = gcnew List<Context^>();

		for (std::list<TopoLogicCore::Context*>::const_iterator rkCoreContextIterator = rkCoreContexts.cbegin();
			rkCoreContextIterator != rkCoreContexts.cend();
			rkCoreContextIterator++)
		{
			pContexts->Add(gcnew Context(*rkCoreContextIterator));
		}

		return pContexts;
	}

	Topology^ Topology::AddContent(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		pCoreTopology->AddContent(TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery()));
		return this;
	}

	Topology^ Topology::RemoveContent(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		pCoreTopology->RemoveContent(TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery()));
		return this;
	}

	Topology^ Topology::AddContext(Context^ context)
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		pCoreTopology->AddContext(TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Context>(context->GetCoreTopologicalQuery()));
		return this;
	}

	Topology^ Topology::RemoveContext(Context^ context)
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		pCoreTopology->RemoveContext(TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Context>(context->GetCoreTopologicalQuery()));
		return this;
	}

	List<Topology^>^ Topology::SharedTopologies(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		std::list<TopoLogicCore::Topology*> coreSharedTopologies;
		pCoreTopology->SharedTopologies(TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery()), coreSharedTopologies);
		List<Topology^>^ pSharedTopologies = gcnew List<Topology^>();
		for (std::list<TopoLogicCore::Topology*>::const_iterator kCoreSharedTopologyIterator = coreSharedTopologies.begin();
			kCoreSharedTopologyIterator != coreSharedTopologies.end();
			kCoreSharedTopologyIterator++)
		{
			pSharedTopologies->Add(Topology::ByCoreTopology(*kCoreSharedTopologyIterator));
		}
		return pSharedTopologies;
	}

	List<List<Topology^>^>^ Topology::PathsTo(Topology^ topology, int maxLevel, int maxPaths)
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		std::list<std::list<TopoLogicCore::TopologicalQuery*>> corePaths;
		pCoreTopology->PathsTo(
			TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery()), 
			maxLevel,
			maxPaths,
			corePaths);

		throw gcnew NotImplementedException();
	}

	Dictionary<String^, Object^>^ Topology::BooleanImages(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		if (pCoreTopology == nullptr)
		{
			throw gcnew Exception("Not a topology");
		}
		TopoLogicCore::Topology* pCoreTopologyA = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		TopoLogicCore::Topology* pCoreTopologyB = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery());

		std::list<TopoLogicCore::Topology*> coreArgumentsInArguments;
		std::list<TopoLogicCore::Topology*> coreArgumentsInTools;
		std::list<TopoLogicCore::Topology*> coreToolsInArguments;
		std::list<TopoLogicCore::Topology*> coreToolsInTools;
		pCoreTopologyA->BooleanImages(pCoreTopologyB, coreArgumentsInArguments, coreArgumentsInTools, coreToolsInArguments, coreToolsInTools);

		List<Topology^>^ pArgumentsInArguments = gcnew List<Topology^>();
		List<Object^>^ pGeometryArgumentsInArguments = gcnew List<Object^>();
		for (std::list<TopoLogicCore::Topology*>::const_iterator kCoreTopology = coreArgumentsInArguments.begin();
			kCoreTopology != coreArgumentsInArguments.end();
			kCoreTopology++)
		{
			Topology^ pTopology = Topology::ByCoreTopology(*kCoreTopology);
			pArgumentsInArguments->Add(pTopology);
			pGeometryArgumentsInArguments->Add(pTopology->Geometry);
		}

		List<Topology^>^ pArgumentsInTools = gcnew List<Topology^>();
		List<Object^>^ pGeometryArgumentsInTools = gcnew List<Object^>();
		for (std::list<TopoLogicCore::Topology*>::const_iterator kCoreTopology = coreArgumentsInTools.begin();
			kCoreTopology != coreArgumentsInTools.end();
			kCoreTopology++)
		{
			Topology^ pTopology = Topology::ByCoreTopology(*kCoreTopology);
			pArgumentsInTools->Add(pTopology);
			pGeometryArgumentsInTools->Add(pTopology->Geometry);
		}

		List<Topology^>^ pToolsInArguments = gcnew List<Topology^>();
		List<Object^>^ pGeometryToolsInArguments = gcnew List<Object^>();
		for (std::list<TopoLogicCore::Topology*>::const_iterator kCoreTopology = coreToolsInArguments.begin();
			kCoreTopology != coreToolsInArguments.end();
			kCoreTopology++)
		{
			Topology^ pTopology = Topology::ByCoreTopology(*kCoreTopology);
			pToolsInArguments->Add(pTopology);
			pGeometryToolsInArguments->Add(pTopology->Geometry);
		}

		List<Topology^>^ pToolsInTools = gcnew List<Topology^>();
		List<Object^>^ pGeometryToolsInTools = gcnew List<Object^>();
		for (std::list<TopoLogicCore::Topology*>::const_iterator kCoreTopology = coreToolsInTools.begin();
			kCoreTopology != coreToolsInTools.end();
			kCoreTopology++)
		{
			Topology^ pTopology = Topology::ByCoreTopology(*kCoreTopology);
			pToolsInTools->Add(pTopology);
			pGeometryToolsInTools->Add(pTopology->Geometry);
		}

		Dictionary<String^, Object^>^ pDictionary = gcnew Dictionary<String^, Object^>();
		pDictionary->Add("ArgumentImagesInArguments", pArgumentsInArguments);
		pDictionary->Add("ArgumentImagesInTools", pArgumentsInTools);
		pDictionary->Add("ToolImagesInArguments", pToolsInArguments);
		pDictionary->Add("ToolImagesInTools", pToolsInTools);
		pDictionary->Add("GeometryArgumentImagesInArguments", pGeometryArgumentsInArguments);
		pDictionary->Add("GeometryArgumentImagesInTools", pGeometryArgumentsInTools);
		pDictionary->Add("GeometryToolImagesInArguments", pGeometryToolsInArguments);
		pDictionary->Add("GeometryToolImagesInTools", pGeometryToolsInTools);
		return pDictionary;
	}

	Topology^ Topology::Difference(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopologyA = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		TopoLogicCore::Topology* pCoreTopologyB = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery());

		try{
			TopoLogicCore::Topology* pDifferenceCoreTopology = pCoreTopologyA->Difference(pCoreTopologyB);
			return Topology::ByCoreTopology(pDifferenceCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::Impose(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopologyA = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		TopoLogicCore::Topology* pCoreTopologyB = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery());

		try{
			TopoLogicCore::Topology* pImposeCoreTopology = pCoreTopologyA->Impose(pCoreTopologyB); 
			return Topology::ByCoreTopology(pImposeCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::Imprint(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopologyA = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		TopoLogicCore::Topology* pCoreTopologyB = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery());

		try{
			TopoLogicCore::Topology* pImprintCoreTopology = pCoreTopologyA->Imprint(pCoreTopologyB);
			return Topology::ByCoreTopology(pImprintCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::Intersection(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopologyA = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		TopoLogicCore::Topology* pCoreTopologyB = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery());

		try {
			TopoLogicCore::Topology* pIntersectionCoreTopology = pCoreTopologyA->Intersection(pCoreTopologyB);
			return Topology::ByCoreTopology(pIntersectionCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}
	
	Topology^ Topology::Union(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopologyA = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		TopoLogicCore::Topology* pCoreTopologyB = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery());

		try{
			TopoLogicCore::Topology* pUnionCoreTopology = pCoreTopologyA->Union(pCoreTopologyB);
			return Topology::ByCoreTopology(pUnionCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::Merge(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopologyA = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		TopoLogicCore::Topology* pCoreTopologyB = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery());

		try{
			TopoLogicCore::Topology* pMergeCoreTopology = pCoreTopologyA->Merge(pCoreTopologyB);
			return Topology::ByCoreTopology(pMergeCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::Slice(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopologyA = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		TopoLogicCore::Topology* pCoreTopologyB = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery());

		try{
			TopoLogicCore::Topology* pSliceCoreTopology = pCoreTopologyA->Slice(pCoreTopologyB);
			return Topology::ByCoreTopology(pSliceCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::XOR(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopologyA = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());
		TopoLogicCore::Topology* pCoreTopologyB = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(topology->GetCoreTopologicalQuery());

		try{
			TopoLogicCore::Topology* pSliceCoreTopology = pCoreTopologyA->XOR(pCoreTopologyB);
			return Topology::ByCoreTopology(pSliceCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	List<Topology^>^ Topology::ImmediateMembers()
	{
		TopoLogicCore::Topology* pCoreTopology = TopoLogicCore::TopologicalQuery::Downcast<TopoLogicCore::Topology>(GetCoreTopologicalQuery());

		std::list<TopoLogicCore::Topology*> coreTopologies;
		pCoreTopology->ImmediateMembers(coreTopologies);

		List<Topology^>^ pTopologies = gcnew List<Topology^>();
		for (std::list<TopoLogicCore::Topology*>::const_iterator kTopologyIterator = coreTopologies.begin();
			kTopologyIterator != coreTopologies.end();
			kTopologyIterator++)
		{
			Topology^ pTopology = Topology::ByCoreTopology(*kTopologyIterator);
			pTopologies->Add(pTopology);
		}
		return pTopologies;
	}
}