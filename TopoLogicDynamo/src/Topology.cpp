#include <msclr/marshal_cppstd.h>
#ifdef max
#undef max
#endif
#include "Topology.h"

#include <Cluster.h>
#include <CellComplex.h>
#include <Cell.h>
#include <Shell.h>
#include <Face.h>
#include <Wire.h>
#include <Edge.h>
#include <Vertex.h>
#include <Graph.h>
#include <Aperture.h>
#include <Context.h>
#include <TopologyFactoryManager.h>
#include <TopologyFactory.h>
#include <ApertureFactory.h>
#include <VertexFactory.h>
#include <EdgeFactory.h>
#include <WireFactory.h>
#include <FaceFactory.h>
#include <ShellFactory.h>
#include <CellFactory.h>
#include <CellComplexFactory.h>
#include <ClusterFactory.h>
#include <GraphFactory.h>
#include <ApertureFactory.h>
#include <AttributeManager.h>
#include <AttributeFactory.h>

#include <TopologicUtility/include/AttributeManager.h>
#include <TopologicUtility/include/IntAttribute.h>
#include <TopologicUtility/include/DoubleAttribute.h>
#include <TopologicUtility/include/StringAttribute.h>

namespace Topologic
{
	int Topology::Dimensionality::get()
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		return pCoreTopology->Dimensionality();
	}

	Topology^ Topology::ByGeometry(Autodesk::DesignScript::Geometry::Geometry^ geometry, double tolerance)
	{
		if (geometry == nullptr)
		{
			throw gcnew Exception("A null input is given.");
		}

		Autodesk::DesignScript::Geometry::Point^ dynamoPoint = dynamic_cast<Autodesk::DesignScript::Geometry::Point^>(geometry);
		if (dynamoPoint != nullptr)
		{
			return Vertex::ByPoint(dynamoPoint);
		}

		Autodesk::DesignScript::Geometry::Curve^ dynamoCurve = dynamic_cast<Autodesk::DesignScript::Geometry::Curve^>(geometry);
		if (dynamoCurve != nullptr)
		{
			// Do this first so that a polycurve (which is a curve) is not handled by Edge.
			Autodesk::DesignScript::Geometry::PolyCurve^ dynamoPolyCurve = dynamic_cast<Autodesk::DesignScript::Geometry::PolyCurve^>(geometry);
			if (dynamoPolyCurve != nullptr)
			{
				return Wire::ByPolyCurve(dynamoPolyCurve);
			}

			// If it is a curve which actually contains more than 1 curves, create a polyCurve first, because it has a NumberOfCurves property.
			List<Autodesk::DesignScript::Geometry::Curve^>^ dynamoCurves = gcnew List<Autodesk::DesignScript::Geometry::Curve^>();
			dynamoCurves->Add(dynamoCurve);
			dynamoPolyCurve = Autodesk::DesignScript::Geometry::PolyCurve::ByJoinedCurves(dynamoCurves, 0.0001);
			int numOfCurves = dynamoPolyCurve->NumberOfCurves;
			if (numOfCurves < 1)
			{
				throw gcnew Exception("The geometry is a curve by type but no curve is detected.");
			}
			else if (numOfCurves > 1)
			{
				Wire^ wire = Wire::ByPolyCurve(dynamoPolyCurve);
				delete dynamoPolyCurve;
				return wire;
			}

			return Edge::ByCurve(dynamoCurve);
		}

		Autodesk::DesignScript::Geometry::Surface^ dynamoSurface = dynamic_cast<Autodesk::DesignScript::Geometry::Surface^>(geometry);
		if (dynamoSurface != nullptr)
		{
			// Do this first so that a polySurface (which is a surface) is not handled by Face.
			Autodesk::DesignScript::Geometry::PolySurface^ dynamoPolySurface = dynamic_cast<Autodesk::DesignScript::Geometry::PolySurface^>(geometry);
			if (dynamoPolySurface != nullptr)
			{
				return Shell::ByPolySurface(dynamoPolySurface);
			}

			// If it is a surface which actually contains more than 1 surfaces, create a polySurface first, because it has a SurfaceCount method.
			List<Autodesk::DesignScript::Geometry::Surface^>^ surfaces = gcnew List<Autodesk::DesignScript::Geometry::Surface^>();
			surfaces->Add(dynamoSurface);
			dynamoPolySurface = Autodesk::DesignScript::Geometry::PolySurface::ByJoinedSurfaces(surfaces);
			int numOfSurfaces = dynamoPolySurface->SurfaceCount();
			if (numOfSurfaces < 1)
			{
				throw gcnew Exception("The geometry is a surface by type but no surface is detected.");
			}else if (numOfSurfaces > 1)
			{
				// This can be a shell or a cluster, so call Topology::ByPolySurface.
				Topology^ topology = Topology::ByPolySurface(dynamoPolySurface);
				delete dynamoPolySurface;
				return topology;
			}

			return Face::BySurface(dynamoSurface);
		}

		Autodesk::DesignScript::Geometry::Solid^ dynamoSolid = dynamic_cast<Autodesk::DesignScript::Geometry::Solid^>(geometry);
		if (dynamoSolid != nullptr)
		{
			return Cell::BySolid(dynamoSolid, tolerance);
		}

		throw gcnew NotImplementedException("This geometry is not currently handled.");
	}

	//Topology^ Topology::ByVertexIndex(List<array<double, 3>^>^ vertexCoordinates, List<List<int>^>^ vertexIndices)
	//{
	//	throw gcnew System::NotImplementedException();
	//	// TODO: insert return statement here
	//}

	List<Topology^>^ Topology::ByVerticesIndices(System::Collections::Generic::IEnumerable<Vertex^>^ vertices, System::Collections::Generic::IEnumerable<System::Collections::Generic::List<int>^>^ vertexIndices)
	{
		std::vector<TopologicCore::Vertex::Ptr> coreVertices;
		for each(Vertex^ pVertex in vertices)
		{
			TopologicCore::Vertex::Ptr pCoreVertex = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Vertex>(pVertex->GetCoreTopologicalQuery());
			coreVertices.push_back(pCoreVertex);
		}

		std::list<std::list<int>> coreIndices;
		for each(List<int>^ vertex1DIndices in vertexIndices)
		{
			std::list<int> coreVertex1DIndices;
			for each(int vertexIndex in vertex1DIndices)
			{
				if (vertexIndex < 0)
				{
					throw gcnew Exception("The index list contains a negative index.");
				}
				coreVertex1DIndices.push_back(vertexIndex);
			}
			coreIndices.push_back(coreVertex1DIndices);
		}
		std::list<TopologicCore::Topology::Ptr> pCoreTopologies;
		TopologicCore::Topology::ByVertexIndex(coreVertices, coreIndices, pCoreTopologies);

		List<Topology^>^ pTopologies = gcnew List<Topology^>();
		for (const TopologicCore::Topology::Ptr& kpCoreTopology : pCoreTopologies)
		{
			pTopologies->Add(Topology::ByCoreTopology(kpCoreTopology));
		}
		return pTopologies;
	}

	Topology ^ Topology::ByPolySurface(Autodesk::DesignScript::Geometry::PolySurface ^ polySurface)
	{
		List<Face^>^ pFaces = gcnew List<Face^>();
		for each(Autodesk::DesignScript::Geometry::Surface^ pDynamoSurface in polySurface->Surfaces())
		{
			pFaces->Add(Face::BySurface(pDynamoSurface));
		}
		return ByFaces(pFaces);
	}

	Topology^ Topology::ByFaces(System::Collections::Generic::IEnumerable<Face^>^ faces)
	{
		std::list<TopologicCore::Face::Ptr> coreFaces;
		for each(Face^ pFace in faces)
		{
			coreFaces.push_back(TopologicCore::Topology::Downcast<TopologicCore::Face>(pFace->GetCoreTopologicalQuery()));
		}

		TopologicCore::Topology::Ptr pCoreTopology = nullptr;
		try {
			pCoreTopology = TopologicCore::Topology::ByFaces(coreFaces);
		}
		catch (const std::exception& rkException)
		{
			throw gcnew Exception(gcnew String(rkException.what()));
		}

		return Topology::ByCoreTopology(pCoreTopology);
	}

	void RecursiveGeometry(Topology^ topology, List<Object^>^% output)
	{
		List<Object^>^ objects = gcnew List<Object^>();
		objects->Add(topology->Geometry_);

		List<Topology^>^ subContents = topology->SubContents;
		List<Object^>^ subContentGeometries = gcnew List<Object^>();
		for each(Topology^ subContent in subContents)
		{
			List<Object^>^ dynamoThisGeometries = gcnew List<Object^>();
			RecursiveGeometry(subContent, subContentGeometries);
		}

		if (subContentGeometries->Count > 0)
		{
			objects->Add(subContentGeometries);
		}

		output->Add(objects);
	}

	Object^ Topology::Geometry::get()
	{
		List<Object^>^ output = gcnew List<Object^>();
		RecursiveGeometry(this, output);
		return output;
	}

	bool Topology::ExportToBRep(String^ path)
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::string cppPath = msclr::interop::marshal_as<std::string>(path);
		return pCoreTopology->SaveToBrep(cppPath);
	}

	Topology^ Topology::ImportFromBRep(String^ path)
	{
		std::string cppPath = msclr::interop::marshal_as<std::string>(path);
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::Topology::LoadFromBrep(cppPath);
		Topology^ pTopology = Topology::ByCoreTopology(pCoreTopology);
		return pTopology;
	}

	String^ Topology::Analyze()
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		return gcnew String(pCoreTopology->Analyze().c_str());
	}

	bool Topology::IsSame(Topology^ topology)
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::shared_ptr<TopologicCore::Topology> pOtherCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery());
		return pCoreTopology->IsSame(pOtherCoreTopology);
	}

	Topology ^ Topology::ClosestSimplestSubshape(Topology^ selector)
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::shared_ptr<TopologicCore::Topology> pCoreQueryTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(selector->GetCoreTopologicalQuery());

		std::shared_ptr<TopologicCore::Topology> pClosestLowestSubshape = pCoreTopology->ClosestSimplestSubshape(pCoreQueryTopology);
		return ByCoreTopology(pClosestLowestSubshape);
	}

	Topology ^ Topology::SelectSubtopology(Topology ^ selector, int typeFilter)
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::shared_ptr<TopologicCore::Topology> pCoreQueryTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(selector->GetCoreTopologicalQuery());

		std::shared_ptr<TopologicCore::Topology> pSelectedSubtopology = pCoreTopology->SelectSubtopology(pCoreQueryTopology, typeFilter);
		return ByCoreTopology(pSelectedSubtopology);
	}

	Topology ^ Topology::ShallowCopy()
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::shared_ptr<TopologicCore::Topology> pCoreCopyTopology = pCoreTopology->ShallowCopy();
		return Topology::ByCoreTopology(pCoreCopyTopology);

	}

	String^ Topology::TypeAsString::get()
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::string strType = pCoreTopology->GetTypeAsString();
		return gcnew String(strType.c_str());
	}

	int Topology::Type::get()
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::TopologyType topologyType = pCoreTopology->GetType();
		return topologyType;
	}

	Topology^ Topology::ByCoreTopology(const std::shared_ptr<TopologicCore::Topology>& kpCoreTopology)
	{
		if (kpCoreTopology == nullptr)
		{
			return nullptr;
		}
		String^ guid = gcnew String(kpCoreTopology->GetInstanceGUID().c_str());
		TopologyFactory^ topologyFactory = nullptr;
		try {
			topologyFactory = TopologyFactoryManager::Instance->Find(guid);
		}
		catch (...)
		{
			topologyFactory = TopologyFactoryManager::Instance->GetDefaultFactory(kpCoreTopology);
		}
		return topologyFactory->Create(TopologicCore::TopologyPtr(kpCoreTopology));
	}

	Object ^ Topology::CleanupGeometryOutput(List<Object^>^ geometry)
	{
		if (geometry->Count == 1)
		{
			return geometry[0];
		}

		return geometry;
	}

	generic <class T>
		where T: Topology
	T Topology::Copy()
	{
		TopologicCore::Topology::Ptr pCoreTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyTopology = pCoreTopology->DeepCopy();

		TopologicUtility::AttributeManager::GetInstance().CopyAttributes(pCoreTopology->GetOcctShape(), pCoreCopyTopology->GetOcctShape());
		
		Topology^ topology = ByCoreTopology(pCoreCopyTopology);
		return safe_cast<T>(topology);
	}

	/*generic <class T>
	T Topology::DeepCopy()
	{
		TopologicCore::Topology::Ptr pCoreTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyTopology = pCoreTopology->DeepCopy();

		TopologicUtility::AttributeManager::GetInstance().CopyAttributes(pCoreTopology->GetOcctShape(), pCoreCopyTopology->GetOcctShape());

		Topology^ topology = ByCoreTopology(pCoreCopyTopology);
		return safe_cast<T>(topology);
	}*/

	void Topology::RegisterFactory(const TopologicCore::Topology::Ptr & kpCoreTopology, TopologyFactory^ topologyFactory)
	{
		TopologyFactoryManager::Instance->Add(kpCoreTopology, topologyFactory);
	}

	void Topology::RegisterFactory(String^ guid, TopologyFactory^ topologyFactory)
	{
		TopologyFactoryManager::Instance->Add(guid, topologyFactory);
	}
	
	/*List<Topology^>^ Topology::HostTopology__::get()
	{
		throw gcnew NotImplementedException("Feature not yet implemented");
	}*/

	Topology::Topology()
	{
		// Register the factories
		static bool areFactoriesAdded = false;
		if(!areFactoriesAdded)
		{
			RegisterFactory(gcnew String(TopologicCore::VertexGUID::Get().c_str()), gcnew VertexFactory());
			RegisterFactory(gcnew String(TopologicCore::EdgeGUID::Get().c_str()), gcnew EdgeFactory());
			RegisterFactory(gcnew String(TopologicCore::WireGUID::Get().c_str()), gcnew WireFactory());
			RegisterFactory(gcnew String(TopologicCore::FaceGUID::Get().c_str()), gcnew FaceFactory());
			RegisterFactory(gcnew String(TopologicCore::ShellGUID::Get().c_str()), gcnew ShellFactory());
			RegisterFactory(gcnew String(TopologicCore::CellGUID::Get().c_str()), gcnew CellFactory());
			RegisterFactory(gcnew String(TopologicCore::CellComplexGUID::Get().c_str()), gcnew CellComplexFactory());
			RegisterFactory(gcnew String(TopologicCore::ClusterGUID::Get().c_str()), gcnew ClusterFactory());
			RegisterFactory(gcnew String(TopologicExtension::GraphGUID::Get().c_str()), gcnew GraphFactory());
			RegisterFactory(gcnew String(TopologicCore::ApertureGUID::Get().c_str()), gcnew ApertureFactory());
			areFactoriesAdded = true;
		}
	}

	Topology::~Topology()
	{

	}

	Topology^ Topology::SetKeysValues(List<String^>^ keys, List<Object^>^ values)
	{
		if (keys->Count == 0)
		{
			throw gcnew Exception("An empty list of keys is given.");
		}

		if (values->Count == 0)
		{
			throw gcnew Exception("An empty list of values is given.");
		}

		if (values->Count != keys->Count)
		{
			throw gcnew Exception("The keys and values lists do not have the same length.");
		}

		Dictionary<String^, Object^>^ dictionary = gcnew Dictionary<String^, Object^>();
		for (int i = 0; i < keys->Count; i++)
		{
			dictionary->Add(keys[i], values[i]);
		}

		// 1. Copy the core topology
		TopologicCore::Topology::Ptr pCoreTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyTopology = pCoreTopology->DeepCopy();
		Topology^ copyTopology = Topology::ByCoreTopology(pCoreCopyTopology);
		copyTopology->AddAttributesNoCopy(dictionary);

		return copyTopology;
	}


	Object ^ Topology::ValueAtKey(String ^ key)
	{
		TopologicCore::Topology::Ptr pCoreTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::string cppName = msclr::interop::marshal_as<std::string>(key);

		TopologicUtility::Attribute::Ptr pSupportAttribute = TopologicUtility::AttributeManager::GetInstance().Find(pCoreTopology->GetOcctShape(), cppName);
		if (pSupportAttribute == nullptr)
		{
			return nullptr;
		}

		AttributeFactory^ attributeFactory = AttributeManager::Instance->GetFactory(pSupportAttribute);
		return attributeFactory->CreateValue(pSupportAttribute);
	}

	Topology ^ Topology::AddAttributesNoCopy(Dictionary<String^, Object^>^ attributes)
	{
		TopologicCore::Topology::Ptr pCoreTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());

		for each(KeyValuePair<String^, Object^>^ entry in attributes)
		{
			System::Type^ entryValueType = entry->Value->GetType();
			AttributeManager::Instance->SetAttribute(this, entry->Key, entry->Value); 
		}
		return this;
	}

	Object ^ Topology::AttributeValue(String ^ name)
	{
		TopologicCore::Topology::Ptr pCoreTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::string cppName = msclr::interop::marshal_as<std::string>(name);

		TopologicUtility::Attribute::Ptr pSupportAttribute = TopologicUtility::AttributeManager::GetInstance().Find(pCoreTopology->GetOcctShape(), cppName);
		AttributeFactory^ attributeFactory = AttributeManager::Instance->GetFactory(pSupportAttribute);
		return attributeFactory->CreateValue(pSupportAttribute);
	}

	List<List<Object^>^>^ Topology::KeysValues::get()
	{
		TopologicCore::Topology::Ptr pCoreTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());

		std::map<std::string, std::shared_ptr<TopologicUtility::Attribute>> coreAttributes;
		bool isFound = TopologicUtility::AttributeManager::GetInstance().FindAll(pCoreTopology->GetOcctShape(), coreAttributes);
		if (!isFound)
		{
			return nullptr;
		}

		List<Object^>^ keys = gcnew List<Object^>();
		List<Object^>^ values = gcnew List<Object^>();
		for (const std::pair<std::string, TopologicUtility::Attribute::Ptr>& rkAttributePair : coreAttributes)
		{
			String^ key = gcnew String(rkAttributePair.first.c_str());
			keys->Add(key);

			AttributeFactory^ attributeFactory = AttributeManager::Instance->GetFactory(rkAttributePair.second);
			values->Add(attributeFactory->CreateValue(rkAttributePair.second));
		}

		List<List<Object^>^>^ keysValues = gcnew List<List<Object^>^>();
		keysValues->Add(keys);
		keysValues->Add(values);
		return keysValues;
	}

	Topology^ Topology::RemoveKeys(List<String^>^ keys)
	{
		Topology^ copyTopology = Copy<Topology^>();
		std::shared_ptr<TopologicCore::Topology> pCoreCopyTopology = 
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(copyTopology->GetCoreTopologicalQuery());
		for each(String^ key in keys)
		{
			std::string cppKey = msclr::interop::marshal_as<std::string>(key);
			TopologicUtility::AttributeManager::GetInstance().Remove(pCoreCopyTopology, cppKey);
		}

		return copyTopology;
	}

	List<Topology^>^ Topology::Contents::get()
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::list<std::shared_ptr<TopologicCore::Topology>> coreContents;
		pCoreTopology->Contents(coreContents);

		List<Topology^>^ pTopologies = gcnew List<Topology^>();

		for (const TopologicCore::Topology::Ptr& kpCoreContent : coreContents)
		{
			Topology^ topology = Topology::ByCoreTopology(kpCoreContent);
			pTopologies->Add(topology);
		}
		return pTopologies;
	}

	List<Topology^>^ Topology::SubContents::get()
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::list<std::shared_ptr<TopologicCore::Topology>> coreSubContents;
		pCoreTopology->SubContents(coreSubContents);

		List<Topology^>^ pTopologies = gcnew List<Topology^>();

		for (const TopologicCore::Topology::Ptr& kpCoreSubContent : coreSubContents)
		{
			Topology^ topology = Topology::ByCoreTopology(kpCoreSubContent);
			pTopologies->Add(topology);
		}
		return pTopologies;
	}

	List<Context^>^ Topology::Contexts::get()
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::list<std::shared_ptr<TopologicCore::Context>> rkCoreContexts;
		pCoreTopology->Contexts(rkCoreContexts);

		List<Context^>^ pContexts = gcnew List<Context^>();

		for (std::list<std::shared_ptr<TopologicCore::Context>>::const_iterator rkCoreContextIterator = rkCoreContexts.cbegin();
			rkCoreContextIterator != rkCoreContexts.cend();
			rkCoreContextIterator++)
		{
			pContexts->Add(gcnew Context(*rkCoreContextIterator));
		}

		return pContexts;
	}

	Topology^ Topology::AddContent(Topology^ content)
	{
		// 1. Copy this topology
		TopologicCore::Topology::Ptr pCoreParentTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyParentTopology = pCoreParentTopology->DeepCopy();
		
		// 2. Copy the content topology
		TopologicCore::Topology::Ptr pCoreContentTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(content->GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyContentTopology = pCoreContentTopology->DeepCopy();

		pCoreCopyParentTopology->AddContent(pCoreCopyContentTopology);

		// 5. Return the copy topology
		return Topology::ByCoreTopology(pCoreCopyParentTopology);
	}

	Topology ^ Topology::AddContent(Topology ^ topology, int typeFilter)
	{
		// 1. Copy this topology
		TopologicCore::Topology::Ptr pCoreParentTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyParentTopology = pCoreParentTopology->DeepCopy();

		// 2. Copy the content topology
		TopologicCore::Topology::Ptr pCoreContentTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyContentTopology = pCoreContentTopology->DeepCopy();

		pCoreCopyParentTopology->AddContent(pCoreCopyContentTopology, typeFilter);

		// 5. Return the copy topology
		return Topology::ByCoreTopology(pCoreCopyParentTopology);
	}

	Topology^ Topology::RemoveContent(Topology^ topology)
	{
		/*std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		pCoreTopology->RemoveContent(TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery()));
		return this;*/

		throw gcnew NotImplementedException();
	}

	Topology ^ Topology::AddApertures(System::Collections::Generic::IEnumerable<Topology^>^ apertureTopologies)
	{
		// 1. Copy this topology
		TopologicCore::Topology::Ptr pCoreParentTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyParentTopology = pCoreParentTopology->DeepCopy();

		// 2. Copy the aperture topology
		for each(Topology^ apertureTopology in apertureTopologies)
		{
			TopologicCore::Topology::Ptr pCoreApertureTopology =
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(apertureTopology->GetCoreTopologicalQuery());
			TopologicCore::Topology::Ptr pCoreCopyApertureTopology = pCoreApertureTopology->DeepCopy();
			TopologicCore::Aperture::Ptr pCoreAperture = TopologicCore::Aperture::ByTopologyContext(
				pCoreCopyApertureTopology,
				pCoreCopyParentTopology);
		}
		
		// 3. Add the aperture

		// 4. Return the copy parent topology
		return Topology::ByCoreTopology(pCoreCopyParentTopology);
	}

	Topology^ Topology::AddContext(Context^ context)
	{
		// 1. Copy this topology
		TopologicCore::Topology::Ptr pCoreInstanceTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyInstanceTopology = pCoreInstanceTopology->DeepCopy();

		// 2. Get the center of mass of the instanceTopology
		//TopologicCore::Vertex::Ptr pCoreInstanceCenterOfMass = pCoreInstanceTopology->CenterOfMass();

		// 3. Copy the context topology
		TopologicCore::Topology::Ptr pCoreContextTopology =
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(context->Topology->GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreCopyContextTopology = pCoreContextTopology->DeepCopy();

		// 4. Find the closest simplest topology of the copy topology
		//TopologicCore::Topology::Ptr closestSimplestSubshape = pCoreCopyContextTopology->ClosestSimplestSubshape(pCoreInstanceCenterOfMass);

		// 5. Add closestSimplestSubshape as the contex of the copyInstanceTopology
		//TopologicCore::Context::Ptr pCoreContext = TopologicCore::Context::ByTopologyParameters(closestSimplestSubshape, context->U(), context->V(), context->W());
		TopologicCore::Context::Ptr pCoreContext = TopologicCore::Context::ByTopologyParameters(pCoreCopyContextTopology, context->U(), context->V(), context->W());
		pCoreCopyInstanceTopology->AddContext(pCoreContext);

		//pCoreCopyContextTopology->AddContent(pCoreCopyInstanceTopology, true);

		// 7. Return the copy topology
		return Topology::ByCoreTopology(pCoreInstanceTopology);
	}

	Topology^ Topology::RemoveContext(Context^ context)
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		pCoreTopology->RemoveContext(TopologicCore::TopologicalQuery::Downcast<TopologicCore::Context>(context->GetCoreTopologicalQuery()));
		return this;
	}

	List<Topology^>^ Topology::SharedTopologies(Topology^ topology)
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::list<std::shared_ptr<TopologicCore::Topology>> coreSharedTopologies;
		pCoreTopology->SharedTopologies(TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery()), coreSharedTopologies);
		List<Topology^>^ pSharedTopologies = gcnew List<Topology^>();
		for (std::list<std::shared_ptr<TopologicCore::Topology>>::const_iterator kCoreSharedTopologyIterator = coreSharedTopologies.begin();
			kCoreSharedTopologyIterator != coreSharedTopologies.end();
			kCoreSharedTopologyIterator++)
		{
			pSharedTopologies->Add(Topology::ByCoreTopology(*kCoreSharedTopologyIterator));
		}
		return pSharedTopologies;
	}

	List<List<Topology^>^>^ Topology::PathsTo(Topology^ topology, Topology^ parentTopology, int maxLevel, int maxPaths)
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		std::shared_ptr<TopologicCore::Topology> pCoreParentTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(parentTopology->GetCoreTopologicalQuery());
		std::list<std::list<std::shared_ptr<TopologicCore::TopologicalQuery>>> corePaths;
		pCoreTopology->PathsTo(
			TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery()),
			pCoreParentTopology,
			maxLevel,
			maxPaths,
			corePaths);

		throw gcnew NotImplementedException();
	}

	Topology^ Topology::Difference(Topology^ topology)
	{
		TopologicCore::Topology::Ptr pCoreTopologyA = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreTopologyB = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pCoreCopyTopologyA = pCoreTopologyA->DeepCopy();
		TopologicCore::Topology::Ptr pCoreCopyTopologyB = pCoreTopologyB->DeepCopy();

		try{
			std::shared_ptr<TopologicCore::Topology> pDifferenceCoreTopology = pCoreCopyTopologyA->Difference(pCoreCopyTopologyB);
			return Topology::ByCoreTopology(pDifferenceCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::Impose(Topology^ tool)
	{
		TopologicCore::Topology::Ptr pCoreTopologyA = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreTopologyB = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(tool->GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pCoreCopyTopologyA = pCoreTopologyA->DeepCopy();
		TopologicCore::Topology::Ptr pCoreCopyTopologyB = pCoreTopologyB->DeepCopy();

		try{
			std::shared_ptr<TopologicCore::Topology> pImposeCoreTopology = pCoreCopyTopologyA->Impose(pCoreCopyTopologyB);
			return Topology::ByCoreTopology(pImposeCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::Imprint(Topology^ tool)
	{
		TopologicCore::Topology::Ptr pCoreTopologyA = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreTopologyB = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(tool->GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pCoreCopyTopologyA = pCoreTopologyA->DeepCopy();
		TopologicCore::Topology::Ptr pCoreCopyTopologyB = pCoreTopologyB->DeepCopy();

		try{
			std::shared_ptr<TopologicCore::Topology> pImprintCoreTopology = pCoreCopyTopologyA->Imprint(pCoreCopyTopologyB);
			return Topology::ByCoreTopology(pImprintCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::Intersect(Topology^ topology)
	{
		TopologicCore::Topology::Ptr pCoreTopologyA = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreTopologyB = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pCoreCopyTopologyA = pCoreTopologyA->DeepCopy();
		TopologicCore::Topology::Ptr pCoreCopyTopologyB = pCoreTopologyB->DeepCopy();

		try {
			std::shared_ptr<TopologicCore::Topology> pIntersectionCoreTopology = pCoreCopyTopologyA->Intersect(pCoreCopyTopologyB);
			return Topology::ByCoreTopology(pIntersectionCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}
	
	Topology^ Topology::Union(Topology^ topology)
	{
		TopologicCore::Topology::Ptr pCoreTopologyA = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreTopologyB = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pCoreCopyTopologyA = pCoreTopologyA->DeepCopy();
		TopologicCore::Topology::Ptr pCoreCopyTopologyB = pCoreTopologyB->DeepCopy();

		try{
			std::shared_ptr<TopologicCore::Topology> pUnionCoreTopology = pCoreCopyTopologyA->Union(pCoreCopyTopologyB);
			return Topology::ByCoreTopology(pUnionCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::SelfMerge()
	{
		TopologicCore::Topology::Ptr pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pMergeCoreTopology = pCoreTopology->SelfMerge();
		return Topology::ByCoreTopology(pMergeCoreTopology);
	}

	Topology^ Topology::Merge(Topology^ topology)
	{
		TopologicCore::Topology::Ptr pCoreTopologyA = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreTopologyB = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pCoreCopyTopologyA = pCoreTopologyA->DeepCopy();
		TopologicCore::Topology::Ptr pCoreCopyTopologyB = pCoreTopologyB->DeepCopy();
		
		try{
			std::shared_ptr<TopologicCore::Topology> pMergeCoreTopology = pCoreCopyTopologyA->Merge(pCoreCopyTopologyB);
			return Topology::ByCoreTopology(pMergeCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::Slice(Topology^ tool)
	{
		TopologicCore::Topology::Ptr pCoreTopologyA = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreTopologyB = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(tool->GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pCoreCopyTopologyA = pCoreTopologyA->DeepCopy();
		TopologicCore::Topology::Ptr pCoreCopyTopologyB = pCoreTopologyB->DeepCopy();

		try{
			std::shared_ptr<TopologicCore::Topology> pSliceCoreTopology = pCoreCopyTopologyA->Slice(pCoreCopyTopologyB);
			return Topology::ByCoreTopology(pSliceCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology ^ Topology::Divide(Topology ^ tool)
	{
		TopologicCore::Topology::Ptr pCoreTopologyA = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreTopologyB = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(tool->GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pCoreCopyTopologyA = pCoreTopologyA->DeepCopy();
		TopologicCore::Topology::Ptr pCoreCopyTopologyB = pCoreTopologyB->DeepCopy();

		try {
			std::shared_ptr<TopologicCore::Topology> pSliceCoreTopology = pCoreCopyTopologyA->Divide(pCoreCopyTopologyB);
			return Topology::ByCoreTopology(pSliceCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology^ Topology::XOR(Topology^ topology)
	{
		TopologicCore::Topology::Ptr pCoreTopologyA = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreTopologyB = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(topology->GetCoreTopologicalQuery());

		TopologicCore::Topology::Ptr pCoreCopyTopologyA = pCoreTopologyA->DeepCopy();
		TopologicCore::Topology::Ptr pCoreCopyTopologyB = pCoreTopologyB->DeepCopy();

		try{
			std::shared_ptr<TopologicCore::Topology> pSliceCoreTopology = pCoreCopyTopologyA->XOR(pCoreCopyTopologyB);
			return Topology::ByCoreTopology(pSliceCoreTopology);
		}
		catch (std::exception& e)
		{
			throw gcnew Exception(gcnew String(e.what()));
		}
	}

	Topology ^ Topology::Trim(Topology ^ trim)
	{
		throw gcnew System::NotImplementedException();
		// TODO: insert return statement here
	}

	List<Topology^>^ Topology::SubTopologies::get()
	{
		std::shared_ptr<TopologicCore::Topology> pCoreTopology = TopologicCore::TopologicalQuery::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());

		std::list<std::shared_ptr<TopologicCore::Topology>> coreTopologies;
		pCoreTopology->SubTopologies(coreTopologies);

		List<Topology^>^ pTopologies = gcnew List<Topology^>();
		for (std::list<std::shared_ptr<TopologicCore::Topology>>::const_iterator kTopologyIterator = coreTopologies.begin();
			kTopologyIterator != coreTopologies.end();
			kTopologyIterator++)
		{
			Topology^ pTopology = Topology::ByCoreTopology(*kTopologyIterator);
			pTopologies->Add(pTopology);
		}
		return pTopologies;
	}

	/*Topology^ Topology::Simplify()
	{
		TopologicCore::Topology::Ptr pCoreTopology = TopologicCore::Topology::Downcast<TopologicCore::Topology>(GetCoreTopologicalQuery());
		TopologicCore::Topology::Ptr pCoreSimplifiedTopology = pCoreTopology->DeepCopy();
		pCoreSimplifiedTopology->Simplify();
		return Topology::ByCoreTopology(pCoreSimplifiedTopology);
	}*/
}