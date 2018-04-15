#pragma once

#include "Attribute.h"
#include "Utilities.h"
#include "TopologicalQuery.h"

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPAlgo_CellsBuilder.hxx>
#include <Standard_Handle.hxx>
#include <Geom_Geometry.hxx>
#include <Standard_GUID.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TDF_Label.hxx>

#include <list>
#include <map>
#include <memory>

class TopoDS_Shape;

namespace TopoLogicCore
{
	enum TopologyType
	{
		TOPOLOGY_VERTEX = 0,
		TOPOLOGY_EDGE = 1,
		TOPOLOGY_WIRE = 2,
		TOPOLOGY_FACE = 3,
		TOPOLOGY_SHELL = 4,
		TOPOLOGY_CELL = 5,
		TOPOLOGY_CELLCOMPLEX = 6,
		TOPOLOGY_CLUSTER = 7,
		TOPOLOGY_APERTURE = 8
	};

	enum TopologyRelationshipType
	{
		REL_CONSTITUENT = 0,
		REL_APERTURE = 1,
		REL_OTHER_NON_CONSTITUENT = 2
	};

	class Face;
	class Vertex;
	class Context;

	/// <summary>
	/// A Topology is an abstract superclass that constructors, properties and methods used by other subclasses that extend it.
	/// </summary>
	class Topology : public TopologicalQuery, public std::enable_shared_from_this<Topology>
	{
	public:
		typedef std::map<std::string, std::shared_ptr<Attribute>> AttributeMap;
		typedef AttributeMap::iterator AttributeMapIterator;
		typedef AttributeMap::const_iterator AttributeMapConstIterator;

	public:
		virtual ~Topology();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <returns></returns>
		static std::shared_ptr<Topology> ByOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="pGeometry"></param>
		/// <returns></returns>
		static std::shared_ptr<Topology> ByGeometry(Handle(Geom_Geometry) pGeometry);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpContext"></param>
		/// <returns></returns>
		static std::shared_ptr<Topology> ByContext(const std::shared_ptr<Context>& kpContext);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkVertexCoordinates"></param>
		/// <param name="rkVertexIndices"></param>
		/// <returns></returns>
		static std::shared_ptr<Topology> ByVertexIndex(const std::list<std::array<double, 3>>& rkVertexCoordinates, const std::list<std::list<int>>& rkVertexIndices);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkVertices"></param>
		/// <param name="rkVertexIndices"></param>
		/// <returns></returns>
		static std::shared_ptr<Topology> ByVertexIndex(const std::list<std::shared_ptr<Vertex>>& rkVertices, const std::list<std::list<int>>& rkVertexIndices);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API void AddContent(const std::shared_ptr<Topology>& rkTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API void RemoveContent(const std::shared_ptr<Topology>& rkTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkContext"></param>
		/// <returns></returns>
		TOPOLOGIC_API void AddContext(const std::shared_ptr<Context>& rkContext);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkContext"></param>
		/// <returns></returns>
		TOPOLOGIC_API void RemoveContext(const std::shared_ptr<Context>& rkContext);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTopology"></param>
		/// <param name="rkSharedTopologies"></param>
		TOPOLOGIC_API void SharedTopologies(const std::shared_ptr<Topology>& kpTopology, std::list<std::shared_ptr<Topology>>& rkSharedTopologies) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTopology"></param>
		/// <param name="kMaxLevels"></param>
		/// <param name="kMaxPaths"></param>
		/// <param name="rkPaths"></param>
		TOPOLOGIC_API void PathsTo(const std::shared_ptr<Topology>& kpTopology, const int kMaxLevels, const int kMaxPaths, std::list<std::list<std::shared_ptr<TopologicalQuery>>>& rkPaths) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="kArgumentImagesInArguments"></param>
		/// <param name="kArgumentImagesInTools"></param>
		/// <param name="kToolsImagesInArguments"></param>
		/// <param name="kToolsImagesInTools"></param>
		/// <returns></returns>
		TOPOLOGIC_API void BooleanImages(
			const std::shared_ptr<Topology>& kpOtherTopology,
			std::list<std::shared_ptr<Topology>>& kArgumentImagesInArguments,
			std::list<std::shared_ptr<Topology>>& kArgumentImagesInTools,
			std::list<std::shared_ptr<Topology>>& kToolsImagesInArguments,
			std::list<std::shared_ptr<Topology>>& kToolsImagesInTools);

		/// <summary>
		/// Does not perform cells builder.
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="rOcctCellsBuilder"></param>
		/// <param name="kOcctArgumentImagesInArguments"></param>
		/// <param name="kOcctArgumentImagesInTools"></param>
		/// <param name="kOcctToolsImagesInArguments"></param>
		/// <param name="kOcctToolsImagesInTools"></param>
		void BooleanImages(
			const std::shared_ptr<Topology>& kpOtherTopology,
			BOPAlgo_CellsBuilder& rOcctCellsBuilder,
			BOPCol_ListOfShape& rOcctExclusivelyArgumentImages,
			BOPCol_ListOfShape& rOcctExclusivelyToolImages,
			BOPCol_ListOfShape& rOcctSharedImages,
			BOPCol_ListOfShape& rOcctExclusivelyArgumentImageFaces,
			BOPCol_ListOfShape& rOcctExclusivelyToolImageFaces,
			BOPCol_ListOfShape& rOcctSharedImageFaces,
			BOPCol_ListOfShape& kOcctArgumentImagesInArguments,
			BOPCol_ListOfShape& kOcctArgumentImagesInTools,
			BOPCol_ListOfShape& kOcctToolsImagesInArguments,
			BOPCol_ListOfShape& kOcctToolsImagesInTools);


		TOPOLOGIC_API void BooleanParts(
			const std::shared_ptr<Topology>& kpOtherTopology,
			std::list<std::shared_ptr<Topology>>& rSpaceBetween_A_A_and_B_A,
			std::list<std::shared_ptr<Topology>>& rSpaceBetween_B_A_and_A_B,
			std::list<std::shared_ptr<Topology>>& rSpaceBetween_A_B_and_B_B);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpkOtherTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API std::shared_ptr<Topology> Difference(const std::shared_ptr<Topology>& kpOtherTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpkOtherTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API std::shared_ptr<Topology> Impose(const std::shared_ptr<Topology>& kpOtherTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpkOtherTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API std::shared_ptr<Topology> Imprint(const std::shared_ptr<Topology>& kpOtherTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpkOtherTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API std::shared_ptr<Topology> Intersection(const std::shared_ptr<Topology>& kpOtherTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpkOtherTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API std::shared_ptr<Topology> Merge(const std::shared_ptr<Topology>& kpOtherTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpkOtherTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API std::shared_ptr<Topology> Slice(const std::shared_ptr<Topology>& kpOtherTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpkOtherTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API std::shared_ptr<Topology> Union(const std::shared_ptr<Topology>& kpOtherTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpkOtherTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API std::shared_ptr<Topology> XOR(const std::shared_ptr<Topology>& kpOtherTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctGeometries"></param>
		virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		std::list<std::shared_ptr<Topology>>& Contents()
		{
			return m_contents;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		const std::list<std::shared_ptr<Topology>>& Contents() const
		{
			return m_contents;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		std::list<std::shared_ptr<Context>>& Contexts()
		{
			return m_contexts;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		const std::list<std::shared_ptr<Context>>& Contexts() const
		{
			return m_contexts;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rContents"></param>
		TOPOLOGIC_API void ContentsV2(std::list<std::shared_ptr<Topology>>& rContents) const;


		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkPath"></param>
		/// <returns></returns>
		TOPOLOGIC_API bool SaveToBrep(const std::string& rkPath) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkPath"></param>
		/// <returns></returns>
		static TOPOLOGIC_API std::shared_ptr<Topology> LoadFromBrep(const std::string& rkPath);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API std::string Analyze();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int Dimensionality() const
		{
			return m_dimensionality;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual std::shared_ptr<TopoDS_Shape> GetOcctShape() const = 0;

		virtual TopologyType GetType() const = 0;

		/// <summary>
		/// Returns all sub-entities that have no other parents than this topology, i.e. do not belong to other entities.
		/// </summary>
		/// <param name="rImmediateMembers">The immediate members</param>
		TOPOLOGIC_API void ImmediateMembers(std::list<std::shared_ptr<Topology>>& rImmediateMembers) const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API AttributeMap& Attributes() { return m_attributeMap; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API const AttributeMap& Attributes() const { return m_attributeMap; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		std::string GetTypeAsString() const;

		void SetInGlobalCluster(const bool kInGlobalCluster)
		{
			m_isInGlobalCluster = kInGlobalCluster;
		}

		bool IsInGlobalCluster() const
		{
			return m_isInGlobalCluster;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kOcctShapeType"></param>
		/// <param name="rAncestors"></param>
		template <class Subclass>
		void UpwardNavigation(std::list<std::shared_ptr<Subclass>>& rAncestors) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kOcctShapeType"></param>
		/// <param name="rMembers"></param>
		template <class Subclass>
		void DownwardNavigation(std::list<std::shared_ptr<Subclass>>& rMembers) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTopology"></param>
		void AddIngredientTo(const std::shared_ptr<Topology>& kpTopology);

		/// <summary>
		/// Used when deleted
		/// </summary>
		/// <param name="kpTopology"></param>
		void RemoveIngredientTo(const std::shared_ptr<Topology>& kpTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTopology"></param>
		/// <returns></returns>
		bool IsIngredientTo(const std::shared_ptr<Topology>& kpTopology) const;
		
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool IsIngredient() const;

		/*/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTopology"></param>
		void AddIngredient(Topology * const kpTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTopology"></param>
		void RemoveIngredient(Topology * const kpTopology);*/

		/// <summary>
		/// 
		/// </summary>
		/// <param name="pTopology"></param>
		/// <param name="kRelationshipType"></param>
		void AddChildLabel(std::shared_ptr<Topology>& pTopology, const TopologyRelationshipType kRelationshipType);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TDF_Label& GetOcctLabel() { return m_occtLabel; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		const TDF_Label& GetOcctLabel() const { return m_occtLabel; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctLabel"></param>
		void SetOcctLabel(const TDF_Label& rkOcctLabel) { m_occtLabel = rkOcctLabel; }

	protected:
		Topology(const int kDimensionality);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <returns></returns>
		static TopoDS_CompSolid MakeCompSolid(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkShape"></param>
		/// <param name="rImmediateMembers"></param>
		static void ImmediateMembers(const TopoDS_Shape& rkShape, BOPCol_ListOfShape& rImmediateMembers);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkShape"></param>
		/// <param name="kLevel"></param>
		/// <returns></returns>
		static std::string Analyze(const TopoDS_Shape& rkShape, const int kLevel = 0);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <param name="rUnionArguments"></param>
		static void AddUnionInternalStructure(const TopoDS_Shape& rkOcctShape, BOPCol_ListOfShape& rUnionArguments);

		template <class Subclass>
		static TopAbs_ShapeEnum CheckOcctShapeType();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="rOcctCellsBuilder"></param>
		void AddBooleanOperands(
			const std::shared_ptr<Topology>& kpOtherTopology,
			BOPAlgo_CellsBuilder& rOcctCellsBuilder,
			BOPCol_ListOfShape& rOcctCellsBuildersOperandsA,
			BOPCol_ListOfShape& rOcctCellsBuildersOperandsB,
			BOPCol_DataMapOfShapeShape& rOcctMapFaceToFixedFaceA,
			BOPCol_DataMapOfShapeShape& rOcctMapFaceToFixedFaceB);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="kBooleanFlag"></param>
		void BooleanOperation(
			const std::shared_ptr<Topology>& kpOtherTopology,
			BOPAlgo_CellsBuilder& rOcctCellsBuilder,
			BOPCol_ListOfShape& rOcctCellsBuildersOperandsA,
			BOPCol_ListOfShape& rOcctCellsBuildersOperandsB,
			BOPCol_DataMapOfShapeShape& rOcctMapFaceToFixedFaceA,
			BOPCol_DataMapOfShapeShape& rOcctMapFaceToFixedFaceB);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctCellsBuilder"></param>
		/// <returns></returns>
		std::shared_ptr<Topology> GetBooleanResult(
			const std::shared_ptr<Topology>& kpOtherTopology,
			BOPAlgo_CellsBuilder& rOcctCellsBuilder,
			BOPCol_DataMapOfShapeShape& rOcctMapFaceToFixedFaceA,
			BOPCol_DataMapOfShapeShape& rOcctMapFaceToFixedFaceB);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctCellsBuilder"></param>
		/// <param name="rOcctMapFaceToFixedFaceA"></param>
		/// <param name="rOcctMapFaceToFixedFaceB"></param>
		/// <returns></returns>
		std::shared_ptr<Topology> ManageBooleanLabels(
			const std::shared_ptr<Topology>& kpOtherTopology,
			BOPAlgo_CellsBuilder& rOcctCellsBuilder,
			BOPCol_DataMapOfShapeShape& rOcctMapFaceToFixedFaceA,
			BOPCol_DataMapOfShapeShape& rOcctMapFaceToFixedFaceB);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <returns></returns>
		TopoDS_Shape FixBooleanOperandCell(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <returns></returns>
		TopoDS_Shape FixBooleanOperandShell(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <param name="rMapFaceToFixedFace"></param>
		/// <returns></returns>
		TopoDS_Shape FixBooleanOperandFace(const TopoDS_Shape& rkOcctShape, BOPCol_DataMapOfShapeShape& rMapFaceToFixedFace);

		AttributeMap m_attributeMap; // to be replaced by OCCT OCAF
		TDF_Label m_occtLabel;
		int m_dimensionality;
		bool m_isInGlobalCluster;

		// TODO: may cause cyclic dependencies, may need weak_ptr
		std::list<std::shared_ptr<Topology>> m_contents;
		std::list<std::shared_ptr<Context>> m_contexts;

		// TODO: may cause cyclic dependencies, may need weak_ptr
		std::list<std::shared_ptr<Topology>> m_ingredientTo;
		std::list<std::shared_ptr<Topology>> m_ingredients;
	};

	template <class Subclass>
	void Topology::UpwardNavigation(std::list<std::shared_ptr<Subclass>>& rAncestors) const
	{
		static_assert(std::is_base_of<Topology, Subclass>::value, "Subclass not derived from Topology");
		
		TopAbs_ShapeEnum occtShapeType = CheckOcctShapeType<Subclass>();

		TopTools_IndexedDataMapOfShapeListOfShape occtShapeMap;
		TopExp::MapShapesAndUniqueAncestors(
			*GlobalCluster::GetInstance().GetCluster()->GetOcctShape(),
			GetOcctShape()->ShapeType(),
			occtShapeType,
			occtShapeMap);

		const TopTools_ListOfShape& rkOcctAncestors = occtShapeMap.FindFromKey(*GetOcctShape());

		for (TopTools_ListOfShape::const_iterator kIterator = rkOcctAncestors.cbegin();
			kIterator != rkOcctAncestors.cend();
			kIterator++)
		{
			if (kIterator->ShapeType() == occtShapeType)
			{
				rAncestors.push_back(Downcast<Subclass>(ByOcctShape(*kIterator)));
			}
		}
	}

	template <class Subclass>
	void Topology::DownwardNavigation(std::list<std::shared_ptr<Subclass>>& rMembers) const
	{
		static_assert(std::is_base_of<Topology, Subclass>::value, "Subclass not derived from Topology");

		TopAbs_ShapeEnum occtShapeType = CheckOcctShapeType<Subclass>();
		TopTools_MapOfShape occtShapes;
		TopExp_Explorer occtExplorer;
		for (occtExplorer.Init(*GetOcctShape(), occtShapeType); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& occtCurrent = occtExplorer.Current();
			if (!occtShapes.Contains(occtCurrent))
			{
				occtShapes.Add(occtCurrent);
				rMembers.push_back(Downcast<Subclass>(ByOcctShape(occtCurrent)));
			}
		}
	}

	template <class Subclass>
	static TopAbs_ShapeEnum Topology::CheckOcctShapeType()
	{
		if (std::is_same<Subclass, Vertex>::value)
		{
			return TopAbs_VERTEX;
		} 
		else if (std::is_same<Subclass, Edge>::value)
		{
			return TopAbs_EDGE;
		}
		else if (std::is_same<Subclass, Wire>::value)
		{
			return TopAbs_WIRE;
		}
		else if (std::is_same<Subclass, Face>::value)
		{
			return TopAbs_FACE;
		}
		else if (std::is_same<Subclass, Shell>::value)
		{
			return TopAbs_SHELL;
		}
		else if (std::is_same<Subclass, Cell>::value)
		{
			return TopAbs_SOLID;
		}
		else if (std::is_same<Subclass, CellComplex>::value)
		{
			return TopAbs_COMPSOLID;
		}
		else if (std::is_same<Subclass, Cluster>::value)
		{
			return TopAbs_COMPOUND;
		}
		
		throw std::exception("Other subclasses are invalid.");
	}
}