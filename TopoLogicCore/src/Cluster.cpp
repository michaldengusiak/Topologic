#include <Cluster.h>
#include <GlobalCluster.h>
#include <CellComplex.h>
#include <Cell.h>
#include <Vertex.h>
#include <Edge.h>
#include <Wire.h>
#include <Face.h>
#include <Shell.h>

#include <TopoDS_Builder.hxx>
#include <TopoDS_UnCompatibleShapes.hxx>
#include <TopoDS_FrozenShape.hxx>





#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_FrozenShape.hxx>
#include <TopoDS_UnCompatibleShapes.hxx>

#include <TopTools_MapOfShape.hxx>


#include <assert.h>

namespace TopoLogicCore
{
	Cluster* Cluster::ByTopology(const std::list<Topology*>& rkTopologies)
	{
		Cluster* pCluster = new Cluster(true);
		for(std::list<Topology*>::const_iterator kTopologyIterator = rkTopologies.begin();
			kTopologyIterator != rkTopologies.end();
			kTopologyIterator++)
		{
			pCluster->AddTopology(*kTopologyIterator);
		}
		return pCluster;
	}

	bool Cluster::AddTopology(Topology const * const kpkTopology, const bool kCheckIfInside)
	{
		if (kCheckIfInside)
		{
			if (IsInside(kpkTopology))
			{
				return false;
			}
		}

		// If this cluster is not the global cluster, it must have been inside the global cluster.
		// (Added during initialisation.) The free flag is therefore at this point false.
		// To add a new member to this cluster, unfreeze it first/set the flag to true.
		Cluster* pGlobalCluster = GlobalCluster::GetInstance().GetCluster();
		if (GetOcctShape()->IsNotEqual(*pGlobalCluster->GetOcctShape()))
		{
			GetOcctShape()->Free(true);
		}

		// Do the addition
		bool returnValue = true;
		try {
			m_occtBuilder.Add(*GetOcctShape(), *kpkTopology->GetOcctShape());
		}
		catch (TopoDS_UnCompatibleShapes &)
		{
			returnValue = false;
		}
		catch (TopoDS_FrozenShape &)
		{
			returnValue = false;
		}

		// Then reset the free flag to false, i.e. freeze the cluster.
		if (GetOcctShape()->IsNotEqual(*pGlobalCluster->GetOcctShape()))
		{
			GetOcctShape()->Free(false);
		}

		return returnValue;
	}

	bool Cluster::RemoveTopology(Topology const * const kpkTopology)
	{
		// If this cluster is not the global cluster, it must have been inside the global cluster.
		// (Added during initialisation.) The free flag is therefore at this point false.
		// To remove a new member to this cluster, unfreeze it first/set the flag to true.
		Cluster* pGlobalCluster = GlobalCluster::GetInstance().GetCluster();
		if (GetOcctShape()->IsNotEqual(*pGlobalCluster->GetOcctShape()))
		{
			GetOcctShape()->Free(true);
		}

		try {
			m_occtBuilder.Remove(*GetOcctShape(), *kpkTopology->GetOcctShape());

			return true;
		}
		catch (TopoDS_UnCompatibleShapes &)
		{
			return false;
		}
		catch (TopoDS_FrozenShape &)
		{
			return false;
		}

		// Then reset the free flag to false, i.e. freeze the cluster.
		if (GetOcctShape()->IsNotEqual(*pGlobalCluster->GetOcctShape()))
		{
			GetOcctShape()->Free(false);
		}
	}

	TopoDS_Shape* Cluster::GetOcctShape() const
	{
		assert(m_pOcctCompound != nullptr && "Cluster::m_pOcctCompound is null.");
		if (m_pOcctCompound == nullptr)
		{
			throw std::exception("Cluster::m_pOcctCompound is null.");
		}

		return m_pOcctCompound;
	}

	void Cluster::Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const
	{
		throw std::exception("No implementation for Cluster entity");
	}

	Cluster::Cluster(const bool kAddToGlobalCluster)
		: Topology(3)
		, m_pOcctCompound(new TopoDS_Compound())
		, m_occtBuilder(TopoDS_Builder())
	{
		m_occtBuilder.MakeCompound(*m_pOcctCompound);

		if (kAddToGlobalCluster)
		{
			GlobalCluster::GetInstance().Add(this);
		}
	}

	Cluster::Cluster(const TopoDS_Compound& rkOcctCompound, const bool kAddToGlobalCluster)
		: Topology(3)
		, m_pOcctCompound(new TopoDS_Compound(rkOcctCompound))
	{
		// This constructor does not initialise the compound with MakeCompound.

		if (kAddToGlobalCluster)
		{
			GlobalCluster::GetInstance().Add(this);
		}
	}

	Cluster::~Cluster()
	{
		GlobalCluster::GetInstance().Remove(this);
		delete m_pOcctCompound;
	}




	void Cluster::Shells(std::list<Shell*>& rShells) const
	{
		DownwardNavigation(rShells);
	}

	void Cluster::Edges(std::list<Edge*>& rEdges) const
	{
		DownwardNavigation(rEdges);
	}

	void Cluster::Faces(std::list<Face*>& rFaces) const
	{
		DownwardNavigation(rFaces);
	}

	void Cluster::Vertices(std::list<Vertex*>& rVertices) const
	{
		DownwardNavigation(rVertices);
	}

	void Cluster::Wires(std::list<Wire*>& rWires) const
	{
		DownwardNavigation(rWires);
	}

	void Cluster::Cells(std::list<Cell*>& rCells) const
	{
		DownwardNavigation(rCells);
	}

	void Cluster::CellComplexes(std::list<CellComplex*>& rCellComplexes) const
	{
		DownwardNavigation(rCellComplexes);
	}

	bool Cluster::IsInside(Topology const * const kpkTopology) const
	{
		const TopoDS_Shape& rkOcctAddedShape = *kpkTopology->GetOcctShape();
		TopTools_MapOfShape occtShapes;
		TopExp_Explorer occtExplorer;
		for (occtExplorer.Init(*GetOcctShape(), rkOcctAddedShape.ShapeType()); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& rkOcctCurrent = occtExplorer.Current();
			if (rkOcctAddedShape.IsSame(rkOcctCurrent))
			{
				return true;
			}
		}

		return false;
	}

}
