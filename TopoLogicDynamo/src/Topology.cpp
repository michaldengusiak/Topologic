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
#include <TopoLogicCore/include/Topology.h>

#include <TopoDS_Shape.hxx>

namespace TopoLogic
{
	int Topology::Dimensionality::get()
	{
		return GetCoreTopology()->Dimensionality();
	}

	bool Topology::Locked::get()
	{
		return GetCoreTopology()->Locked();
	}

	Topology^ Topology::SetLocked(bool value)
	{
		GetCoreTopology()->Locked(value);
		return this;
	}

	bool Topology::SaveToBRep(String^ path)
	{
		std::string cppPath = msclr::interop::marshal_as<std::string>(path);
		return GetCoreTopology()->SaveToBrep(cppPath);
	}

	bool Topology::LoadFromBRep(String^ path)
	{
		std::string cppPath = msclr::interop::marshal_as<std::string>(path);
		return GetCoreTopology()->LoadFromBrep(cppPath);
	}

	String^ Topology::Analyze()
	{
		return gcnew String(GetCoreTopology()->Analyze().c_str());
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

	List<Topology^>^ Topology::MemberOf()
	{
		throw gcnew System::NotImplementedException();
		// TODO: insert return statement here
	}

	List<Topology^>^ Topology::Members()
	{
		throw gcnew System::NotImplementedException();
		// TODO: insert return statement here
	}

	Dictionary<String^, Object^>^ Topology::BooleanImages(Topology^ topology)
	{
		TopoLogicCore::Topology* pCoreTopologyA = GetCoreTopology();
		TopoLogicCore::Topology* pCoreTopologyB = topology->GetCoreTopology();

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
		TopoLogicCore::Topology* pCoreTopologyA = GetCoreTopology();
		TopoLogicCore::Topology* pCoreTopologyB = topology->GetCoreTopology();

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
		TopoLogicCore::Topology* pCoreTopologyA = GetCoreTopology();
		TopoLogicCore::Topology* pCoreTopologyB = topology->GetCoreTopology();

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
		TopoLogicCore::Topology* pCoreTopologyA = GetCoreTopology();
		TopoLogicCore::Topology* pCoreTopologyB = topology->GetCoreTopology();

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
		TopoLogicCore::Topology* pCoreTopologyA = GetCoreTopology();
		TopoLogicCore::Topology* pCoreTopologyB = topology->GetCoreTopology();

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
		TopoLogicCore::Topology* pCoreTopologyA = GetCoreTopology();
		TopoLogicCore::Topology* pCoreTopologyB = topology->GetCoreTopology();

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
		TopoLogicCore::Topology* pCoreTopologyA = GetCoreTopology();
		TopoLogicCore::Topology* pCoreTopologyB = topology->GetCoreTopology();

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
		TopoLogicCore::Topology* pCoreTopologyA = GetCoreTopology();
		TopoLogicCore::Topology* pCoreTopologyB = topology->GetCoreTopology();

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
		TopoLogicCore::Topology* pCoreTopologyA = GetCoreTopology();
		TopoLogicCore::Topology* pCoreTopologyB = topology->GetCoreTopology();

		try{
			TopoLogicCore::Topology* pSliceCoreTopology = pCoreTopologyA->XOR(pCoreTopologyB);
			return Topology::ByCoreTopology(pSliceCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}
}